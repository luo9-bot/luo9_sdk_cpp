#ifndef __HEAD_H__
#define __HEAD_H__

#ifdef _WIN32
    #ifdef LUO9_SDK_EXPORT
        #define LUO9_API __declspec(dllexport)
    #else
        #define LUO9_API __declspec(dllimport)
    #endif
#else
    #define LUO9_API __attribute__((visibility("default")))
#endif


#endif // __HEAD_H__
