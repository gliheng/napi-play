#ifndef _COUNTER_H_
#define _COUNTER_H_

#include <node_api.h>

class Counter {
  public:
    static napi_value Init(napi_env env, napi_value exports);
  private:
    explicit Counter(int value_ = 0);
    ~Counter();
    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value Inc(napi_env env, napi_callback_info info);
    static napi_value Dec(napi_env env, napi_callback_info info);
    static napi_value GetValue(napi_env env, napi_callback_info info);
    static napi_value SetValue(napi_env env, napi_callback_info info);
    static napi_ref constructor;
    int value_;
    napi_env env_;
    napi_ref wrapper_;
};

#endif // _COUNTER_H_