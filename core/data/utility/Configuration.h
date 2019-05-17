#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <assert.h>
#include <string>
namespace pop{
// Disable silly warnings on some Microsoft VC++ compilers.
#ifdef _MSC_VER
#define NOMINMAX
//#pragma warning(push)
#pragma warning(disable: 4311)
#pragma warning(disable: 4312)
#pragma warning(disable: 4800)
#pragma warning(disable: 4804)
#pragma warning(disable: 4996)
#pragma warning(disable: 4290)
#pragma warning(disable: 4244)
#pragma warning(disable: 4251)
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif

#if defined(unix)        || defined(__unix)      || defined(__unix__) \
 || defined(linux)       || defined(__linux)     || defined(__linux__) \
 || defined(sun)         || defined(__sun) \
 || defined(BSD)         || defined(__OpenBSD__) || defined(__NetBSD__) \
 || defined(__FreeBSD__) || defined __DragonFly__ \
 || defined(sgi)         || defined(__sgi) \
 || defined(__MACOSX__)  || defined(__APPLE__) \
 || defined(__CYGWIN__)
#define Pop_OS 1
#define UNICODE 1
#elif defined(_MSC_VER) || defined(WIN32)  || defined(_WIN32) || defined(__WIN32__) \
   || defined(WIN64)    || defined(_WIN64) || defined(__WIN64__)
#define Pop_OS 2
#define UNICODE 1
#else
#define Pop_OS 0
#endif



#if Pop_OS==2
    #define POP_EXPORTS __declspec(dllexport)
#else
    #define POP_EXPORTS
#endif

#ifdef Debug
#define POP_DbgAssert(expr) assert(expr)
#else
#define POP_DbgAssert(expr)
#endif


#ifdef Debug
#   define POP_DbgAssertMessage(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)
#else
#   define POP_DbgAssertMessage(condition, message) do { (void)(condition);(void) (message); } while (0)
#endif

template<typename Function, typename F>
struct FunctionTypeTraitsSubstituteF
{
    typedef F Result;
};



template<typename Function, int DIM>
struct FunctionTypeTraitsSubstituteDIM
{
    typedef Function Result;
};
template<
        typename Function1,
        typename Function2
        >
void FunctionAssert(const  Function1 & ,const  Function2 &  ,std::string )
{
}
}
#endif // CONFIGURATION_H
