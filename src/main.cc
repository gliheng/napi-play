#include <node_api.h>
#include "napi_assert.h"
#include <stdlib.h>

napi_value Method(napi_env env, napi_callback_info info) {
  size_t argc = 0;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr));
  napi_value* argv = (napi_value*) malloc(argc * sizeof(napi_value));

  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

  int64_t total = 0;
  for (int i = 0; i < argc; i++) {
    int32_t v = 0;
    NAPI_CALL(env, napi_get_value_int32(env, argv[i], &v));
    total += v;
  }

  napi_value ret;
  NAPI_CALL(env, napi_create_int64(env, total, &ret));
  free(argv);
  return ret;
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("add", Method);
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
