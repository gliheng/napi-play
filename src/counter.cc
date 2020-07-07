#include "counter.h"
#include "napi_assert.h"

napi_ref Counter::constructor;

Counter::Counter(int value)
  : value_(value), env_(nullptr), wrapper_(nullptr) {}

Counter::~Counter() {
  napi_delete_reference(env_, wrapper_);
}

void Counter::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  reinterpret_cast<Counter*>(nativeObject)->~Counter();
}

#define DECLARE_NAPI_METHOD(name, func)    \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Counter::Init(napi_env env, napi_value exports) {
  napi_property_descriptor properties[] = {
    { "value", 0, 0, GetValue, SetValue, 0, napi_default, 0 },
    DECLARE_NAPI_METHOD("inc", Inc),
    DECLARE_NAPI_METHOD("dec", Dec),
  };

  napi_value cons;
  NAPI_CALL(env, napi_define_class(env, "Counter", NAPI_AUTO_LENGTH, New, nullptr, 3, properties, &cons));
  NAPI_CALL(env, napi_create_reference(env, cons, 1, &constructor));
  NAPI_CALL(env, napi_set_named_property(env, exports, "Counter", cons));
  return exports;
}

napi_value Counter::New(napi_env env, napi_callback_info info) {
  napi_value target;
  NAPI_CALL(env, napi_get_new_target(env, info, &target));
  bool is_constructor = target != nullptr;

  // Invoked as constructor
  if (is_constructor) {
    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr));
    
    int value = 0;

    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
    
    if (valuetype == napi_number) {
      NAPI_CALL(env, napi_get_value_int32(env, args[0], &value));
    }

    Counter* obj = new Counter(value);

    obj->env_ = env;
    NAPI_CALL(env, napi_wrap(env,
      jsthis,
      reinterpret_cast<void*>(obj),
      Counter::Destructor,
      nullptr,  // finalize_hint
      &obj->wrapper_
    ));

    return jsthis;
  }
  napi_throw_error(env, nullptr, "Cannot invoke as a function");
  return nullptr;
}

napi_value Counter::GetValue(napi_env env, napi_callback_info info) {
  napi_value jsthis;
  NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));

  Counter* obj;
  NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));

  napi_value num;
  NAPI_CALL(env, napi_create_int32(env, obj->value_, &num));

  return num;
}

napi_value Counter::SetValue(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value value;
  napi_value jsthis;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &value, &jsthis, nullptr));

  Counter* obj;
  NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));
  
  NAPI_CALL(env, napi_get_value_int32(env, value, &obj->value_));
  
  return nullptr;
}

napi_value Counter::Inc(napi_env env, napi_callback_info info) {
  napi_value jsthis;
  NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));

  Counter* obj;
  NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));

  obj->value_ += 1;

  napi_value num;
  NAPI_CALL(env, napi_create_int32(env, obj->value_, &num));

  return num;
}

napi_value Counter::Dec(napi_env env, napi_callback_info info) {
  napi_value jsthis;
  NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));

  Counter* obj;
  NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));

  obj->value_ -= 1;

  napi_value num;
  NAPI_CALL(env, napi_create_int32(env, obj->value_, &num));

  return num;
}