#pragma once
//#define P_WINDOWS
#define P_ANDROID
//#define P_LINUX
#ifdef P_WINDOWS
#define LOGI(...) printf(__VA_ARGS__)
#endif // P_WINDOWS
#ifdef P_ANDROID
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define fatalerror assert
#endif // DEBUG

#ifdef P_WINDOWS
#include <assert.h>
#define fatalerror assert
#endif // P_WINDOWS


