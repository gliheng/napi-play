#include <node_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "napi_assert.h"

napi_value GetRandom(napi_env env, napi_callback_info info) {
  srand(time(0));

  napi_value value;
  int num = rand();
  napi_create_int32(env, num, &value);
  return value;
}

napi_value GetObject(napi_env env, napi_callback_info info) {
  napi_value fooValue;
  NAPI_CALL(env, napi_create_string_utf8(env, "fooValue here", NAPI_AUTO_LENGTH, &fooValue));

  napi_value barValue;
  NAPI_CALL(env, napi_create_string_utf8(env, "barValue here", NAPI_AUTO_LENGTH, &barValue));


  napi_value arr;
  NAPI_CALL(env, napi_create_array(env, &arr));
  napi_value push;
  NAPI_CALL(env, napi_get_named_property(env, arr, "push", &push));
  napi_value num;
  NAPI_CALL(env, napi_create_int32(env, 11, &num));
  // call push function
  NAPI_CALL(env, napi_call_function(env, arr, push, 1, &num, nullptr));


  napi_value obj;
  NAPI_CALL(env, napi_create_object(env, &obj));
  napi_property_descriptor descriptors[] = {
    { "foo", NULL, NULL, NULL, NULL, fooValue, napi_enumerable, NULL },
    { "bar", NULL, NULL, NULL, NULL, barValue, napi_enumerable, NULL },
    { "array", NULL, NULL, NULL, NULL, arr, napi_enumerable, NULL },
    { "rand", NULL, NULL, GetRandom, NULL, NULL, napi_enumerable, NULL },
  };
  NAPI_CALL(env, napi_define_properties(env, obj, 3, descriptors));

  napi_value key;
  napi_create_string_latin1(env, "foo", NAPI_AUTO_LENGTH, &key);

  bool yes;
  NAPI_CALL(env, napi_has_property(env, obj, key, &yes));
  printf("object has propery foo? %d\n", yes);

  struct timeval tv;
  gettimeofday(&tv, NULL);
  double time_in_mill = 
         (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; // convert tv_sec & tv_usec to millisecond

  napi_value dateValue;
  NAPI_CALL(env, napi_create_date(env, time_in_mill, &dateValue));
  NAPI_CALL(env, napi_set_named_property(env, obj, "date", dateValue));

  return obj;
}

#define DECLARE_NAPI_METHOD(name, func)        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
    DECLARE_NAPI_METHOD("getObject", GetObject),
  };
  NAPI_CALL(env, napi_define_properties(env, exports, 1, desc));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
