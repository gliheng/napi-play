#include <node_api.h>
#include "napi_assert.h"

napi_value Method(napi_env env, napi_callback_info info) {
  napi_value world;
  NAPI_CALL(env, napi_create_string_utf8(env, "world", 5, &world));
  return world;
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_value new_exports;
  NAPI_CALL(env, napi_create_function(env, "", NAPI_AUTO_LENGTH, Method, nullptr, &new_exports));
  return new_exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
