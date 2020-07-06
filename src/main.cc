#include <node_api.h>
#include "napi_assert.h"

napi_value Method(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value func;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &func, nullptr, nullptr));

  napi_valuetype type;
  NAPI_CALL(env, napi_typeof(env, func, &type));

  if (type != napi_function) {
    NAPI_CALL(env, napi_throw_type_error(env, nullptr, "Must pass a function"));
    return nullptr;
  }

  napi_value global;
  NAPI_CALL(env, napi_get_global(env, &global));

  napi_value ret;
  NAPI_CALL(env, napi_call_function(env, global, func, 0, nullptr, &ret));
  return ret;
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("hello", Method);
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
