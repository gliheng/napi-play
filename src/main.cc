#include <node_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "napi_assert.h"

typedef struct {
  char* path;
  char* data;
  napi_async_work request;
  napi_deferred deferred;
} FileCommand;

void execute(napi_env env, void* data) {
  FileCommand* cmd = (FileCommand*) data;
  FILE* file = fopen(cmd->path, "r");
  size_t len = 1000;
  char* buf = (char*) calloc(1, len + 1);
  fread(buf, len, 1, file);
  cmd->data = buf;
}

void complete(napi_env env, napi_status status, void* data) {
  FileCommand* cmd = (FileCommand*) data;
  if (status == napi_ok) {
    napi_value ret;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, cmd->data, NAPI_AUTO_LENGTH, &ret));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, cmd->deferred, ret));
    free(cmd->data);
  } else {
    napi_value msg, error;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "Canceled", NAPI_AUTO_LENGTH, &msg));
    NAPI_CALL_RETURN_VOID(env, napi_create_error(env, nullptr, msg, &error));
    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, cmd->deferred, error));
  }
  free(cmd->path);
  NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, cmd->request));
  delete cmd;
}

napi_value Method(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr));

  napi_valuetype type;
  NAPI_CALL(env, napi_typeof(env, argv, &type));

  if (type != napi_string) {
    NAPI_CALL(env, napi_throw_type_error(env, nullptr, "Invalid arguments"));
    return nullptr;
  }

  size_t len = 0;
  NAPI_CALL(env, napi_get_value_string_utf8(env, argv, nullptr, 0, &len));
  char* path = (char*) malloc(len + 1);
  NAPI_CALL(env, napi_get_value_string_utf8(env, argv, path, len + 1, &len));

  FileCommand* cmd = new FileCommand { path };

  napi_value promise;
  NAPI_CALL(env, napi_create_promise(env, &cmd->deferred, &promise));

  napi_value name;
  NAPI_CALL(env, napi_create_string_latin1(env, "async work", NAPI_AUTO_LENGTH, &name));
  NAPI_CALL(env, napi_create_async_work(env, nullptr, name, execute, complete, (void*) cmd, &cmd->request));
  NAPI_CALL(env, napi_queue_async_work(env, cmd->request));
  return promise;
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("readFile", Method);
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
