#include <node_api.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "napi_assert.h"

typedef struct {
  napi_threadsafe_function connect_cbk;
  napi_threadsafe_function data_cbk;
} async_socket_job;

void * connect_socket(void* arg) {
  async_socket_job *job = (async_socket_job *) arg;
  napi_acquire_threadsafe_function(job->connect_cbk);
  napi_call_threadsafe_function(job->connect_cbk, nullptr, napi_tsfn_nonblocking);
  napi_release_threadsafe_function(job->connect_cbk, napi_tsfn_abort);

  napi_acquire_threadsafe_function(job->data_cbk);
  for (int i = 0; i < 5; i++) {
    int *result = (int *) malloc(sizeof(int));
    *result = i;
    napi_call_threadsafe_function(job->data_cbk, result, napi_tsfn_nonblocking);
    sleep(1);
  }
  napi_release_threadsafe_function(job->data_cbk, napi_tsfn_abort);

  free(job);
  return nullptr;
}

void call_js(napi_env env, napi_value js_callback, void* context, void* data) {
  int *i = (int *)data;
  napi_value global, value;
  NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, *i, &value));
  NAPI_CALL_RETURN_VOID(env, napi_get_global(env, &global));
  NAPI_CALL_RETURN_VOID(env, napi_call_function(env, global, js_callback, 1, &value, nullptr));
  free(i);
}

pthread_t thread;
static napi_value Connect(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
  
  napi_valuetype type;
  NAPI_CALL(env, napi_typeof(env, argv[0], &type));
  if (type != napi_string) {
    NAPI_CALL(env, napi_throw_error(env, nullptr, "Invalid parameter"));
    return nullptr;
  }

  NAPI_CALL(env, napi_typeof(env, argv[1], &type));
  if (type != napi_object) {
    NAPI_CALL(env, napi_throw_error(env, nullptr, "Invalid parameter"));
    return nullptr;
  }

  napi_value connect_cbk, data_cbk;
  NAPI_CALL(env, napi_get_named_property(env, argv[1], "connect", &connect_cbk));
  NAPI_CALL(env, napi_get_named_property(env, argv[1], "data", &data_cbk));

  async_socket_job *job = (async_socket_job *)malloc(sizeof(async_socket_job));
  napi_value async_name1, async_name2;
  NAPI_CALL(env, napi_create_string_utf8(env,
                                    "connnect async job",
                                    NAPI_AUTO_LENGTH, &async_name1));
  NAPI_CALL(env, napi_create_string_utf8(env,
                                    "data async job",
                                    NAPI_AUTO_LENGTH, &async_name2));

  NAPI_CALL(env, napi_create_threadsafe_function(
    env, connect_cbk, nullptr, async_name1, 0, 1,
    nullptr, nullptr, nullptr, nullptr, &job->connect_cbk));

  NAPI_CALL(env, napi_create_threadsafe_function(
    env, data_cbk, nullptr, async_name2, 0, 1,
    nullptr, nullptr, nullptr, call_js, &job->data_cbk));

  if (pthread_create(&thread, nullptr, &connect_socket, job) != 0) {
    NAPI_CALL(env, napi_throw_error(env, nullptr, "Thread creation error"));
  }
  if (pthread_detach(thread) != 0) {
    NAPI_CALL(env, napi_throw_error(env, nullptr, "Thread creation error"));
  }

  return nullptr;
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("connect", Connect);
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
