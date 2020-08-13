#include <node_api.h>
#include "napi_assert.h"
#include <uv.h>
#include <cstdlib>

typedef struct AsyncData {
  napi_env env;
  napi_ref callback;
} AsyncData;


void OnTimer(uv_timer_t *handle) {
  AsyncData *async = (AsyncData *)handle->data;
  napi_env env = async->env;
  napi_value undefined, callback;
  napi_handle_scope scope;
  NAPI_CALL_RETURN_VOID(env, napi_open_handle_scope(env, &scope));
  NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
  NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, async->callback, &callback));
  NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, 0, nullptr, nullptr));
  NAPI_CALL_RETURN_VOID(env, napi_close_handle_scope(env, scope));
}

napi_value SetInterval(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
  
  napi_valuetype type;
  NAPI_CALL(env, napi_typeof(env, argv[0], &type));
  if (type != napi_function) {
    NAPI_CALL(env, napi_throw_error(env, nullptr, "Invalid parameter"));
    return nullptr;
  }

  NAPI_CALL(env, napi_typeof(env, argv[1], &type));
  if (type != napi_number) {
    NAPI_CALL(env, napi_throw_error(env, nullptr, "Invalid parameter"));
    return nullptr;
  }

  AsyncData *async = (AsyncData *)malloc(sizeof(AsyncData));
  async->env = env;

  NAPI_CALL(env, napi_create_reference(env, argv[0], 1, &async->callback));

  int32_t interval = 0;
  NAPI_CALL(env, napi_get_value_int32(env, argv[1], &interval));
  
  struct uv_loop_s *loop;
  NAPI_CALL(env, napi_get_uv_event_loop(env, &loop));

  uv_timer_t *handle = (uv_timer_t *)malloc(sizeof(uv_timer_t));
  handle->data = async;

  uv_timer_init(loop, handle);
  uv_timer_start(handle, OnTimer, 0, interval);

  return nullptr;
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("setInterval", SetInterval);
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
