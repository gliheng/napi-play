#include <node_api.h>
#include "counter.h"

napi_value Init(napi_env env, napi_value exports) {
  return Counter::Init(env, exports);
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
