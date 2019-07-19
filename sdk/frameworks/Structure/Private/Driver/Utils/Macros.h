//
//  Macros.h
//  360
//
//  Created by Vikas Reddy on 7/6/11.
//  Copyright 2011 Occipital, LLC. All rights reserved.
//

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

union Word {
    uint32_t word;
    uint8_t bytes[4];
};

#define TO_CSTR(x) #x

#ifdef __cplusplus
extern "C" {
#endif
    void _AssertVPrint(const char* statement, const char* file, int line, const char* function, int result, const char* format, va_list printArgs);
    void _AssertPrint(const char* statement, const char* file, int line, const char* function, int result,  const char* format, ...);
    bool _verifyGLState(const char* file, const char* function, int line);
    void _PrintFourChar(uint32_t t);
#ifdef __cplusplus
}
#endif

/* taken from http://clang.llvm.org/docs/LanguageExtensions.html#builtinmacros */
#ifndef __has_builtin
#define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif

/* Branch prediction hints */
#if defined(__GNUC__) || __has_builtin(__builtin_expect)
#define likely(x)   __builtin_expect( (x) ,1)
#define unlikely(x) __builtin_expect( (x) ,0)
#else
#warning "You should consider adding a definition for the branch prediction macros for your compiler."
#define likely(x) x
#define unlikely(x) x
#endif

/* other MIN/MAX variables declare temp variables that shadow each other so you can't safely chain them:
 MIN(MAX(x,y),z) will create three vars named "a" and three vars named "b" */
#undef MAX
#define MAX(a,b) ((a >= b) ? a : b)

#undef MIN
#define MIN(a,b) ((a <= b) ? a : b)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(*a))
#endif

#ifndef CLAMP
#define CLAMP(_target_occipital, _minimum_occipital, _maximum_occipital) ((unlikely( _target_occipital < _minimum_occipital )) ? _minimum_occipital : (unlikely( _target_occipital > _maximum_occipital )) ? _maximum_occipital : _target_occipital)
#endif

/* http://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments */
#define NUMARGS(...)  (sizeof((void**){__VA_ARGS__})/sizeof(void*))

#ifndef ARRAY_SET
#define ARRAY_SET(a, ...) { __typeof__(*a) _temporary_array_occipital[] = {__VA_ARGS__};  for (int _i_occipital = 0; _i_occipital < ARRAY_SIZE(_temporary_array_occipital); _i_occipital++) { a[_i_occipital] = _temporary_array_occipital[_i_occipital]; } }
#endif

#define EnsureCalledEveryMS(ms) { static int32_t _occipital_lastCalled = 0; \
    int32_t _occipital_now_time = OXNetTimestampCurrentMS(); \
    if (_occipital_now_time - _occipital_lastCalled > ms && 0 != _occipital_lastCalled) { \
        DebugSuicide("%s took to long to be called (took %d ms, was supposed to take %d).\n", __PRETTY_FUNCTION__, _occipital_now_time - _occipital_lastCalled, ms); \
    } \
    _occipital_lastCalled = _occipital_now_time; \
}

#ifdef ASSERT_NO_DIE
#define AssertDie()
#else
#define AssertDie() abort()
#endif

#ifdef DEBUG
/* Meant to check for conditions during debug (for example, checking to make sure the state is sane) 
 Turned off during release mode (TODO)
 */
#define DebugAssert(x) \
do { \
bool assert_result_occipital = (x); \
if (unlikely(!assert_result_occipital)) { \
_AssertPrint(TO_CSTR(x), __FILE__, __LINE__, __PRETTY_FUNCTION__, assert_result_occipital, NULL); \
AssertDie(); \
} \
} while (0);
#define DebugAssertCmp(x, operator, y, printf_format) \
do { \
bool assert_result_occipital = (x) operator (y); \
if (unlikely(!assert_result_occipital)) { \
_AssertPrint(TO_CSTR(x), __FILE__, __LINE__, __PRETTY_FUNCTION__, assert_result_occipital, printf_format " " TO_CSTR(operator) " " printf_format "\n" TO_CSTR(x) "=" printf_format "\t" TO_CSTR(y) "=" printf_format, (x), (y), (x), (y)); \
AssertDie(); \
} \
} while (0);

/* Ex: DebugAssertM(kitty != cat, "kitty:%d cat:%d", kitty, cat); */
#define DebugAssertM(x, ...) \
do { \
bool assert_result_occipital = (x); \
if (unlikely(!assert_result_occipital)) { \
_AssertPrint(TO_CSTR(x), __FILE__, __LINE__, __PRETTY_FUNCTION__, assert_result_occipital, __VA_ARGS__); \
AssertDie(); \
} \
} while (0);

/* If you have a conspiracy theory or outlandish assertion--something that 
 should be very obvious, use the ParanoidAssert */
#define ParanoidAssert(x) DebugAssert(x)
#define DebugSuicide(...) { DebugAssertM(false, __VA_ARGS__); }
#define DebugPrintFourChar(x) _PrintFourChar(x);
#define verifyGLState() do { if (unlikely( !_verifyGLState(__FILE__, __PRETTY_FUNCTION__, __LINE__) )) { AssertDie(); } } while (0);
#define DebugPerror(x) { fprintf(stderr, "%s:%s:%d: ", __FILE__, __PRETTY_FUNCTION__, __LINE__); perror(x); }
#define DebugCode(x) x
/* AssertZero in particular is meant for jenking, where you check the return code of library functions
 AssertZero(fcntl) or AssertZero(connect) -- prints out perror, code in hex, decimal, and four char error codes (popular with Apple libs)
 */
#define AssertZero(x) {  int _occipital_result = (x); if (unlikely(_occipital_result != 0)) { printf("AssertZero returned %d four char:", _occipital_result); _PrintFourChar(_occipital_result); \
    abort(); } }
/* Like printf, but also prints out function, line, and file so you can delete it later */
#define PrintVar(var, format) do { CDebugLog("%s:%d:%s=" format "\n", __PRETTY_FUNCTION__, __LINE__, TO_CSTR(var), var); } while (0);

/* Use with caution -- creates a static variable */
#define PrintVarIfChanged(var, format) { static __typeof__(var) occipital_changed_##var = 0;  if (occipital_changed_##var != var) { PrintVar(var, format); occipital_changed_##var = var; }  }

/* DemoAssert is meant to say "You should use a traditional check on this return code later
 but right now I'm in demo mode, and I want to know if it crashes".  For example, in production,
 one would want to check the return value of connect() and take appropriate action
 However, during a demo, you want it to return zero, so: DemoAssert(0 == connect()) */
#define DemoAssert(x) DebugAssert(x)
/* Zero or die is meant to have the inside statement executed in all modes */
#define ZeroOrDie(x) AssertZero(x)
#else
#define DebugAssert(x) 
#define DebugAssertM(...)
#define ParanoidAssert(x)
#define DebugAssertCmp(x, operator, y, printf_format)
#define DebugSuicide(...)
#define DebugPrintFourChar(x)
#define verifyGLState()
#define DebugPerror(x)
#define DebugCode(x)
/* AssertZero was meant to be left in release mode and simply propagate error codes */
#define PropagateReturnCode(x, line) { int _temporary_occipital_return##line = (x); if (unlikely( 0 != _temporary_occipital_return##line )) { return _temporary_occipital_return##line; } }
#define AssertZero(x) PropagateReturnCode(x, __LINE__)
#define PrintVar(var, format)
#define PrintVarIfChanged(var, format)
/* Zero or die is meant to be left on in all modes */
#define ZeroOrDie(x) { if (unlikely( 0 != (x) )) { abort(); } }

#endif

/* you can't have two "case 0:" */
#define CompilerAssert(condition) { switch(condition ? 1 : 0) case 0: case (condition ? 1 : 0): break; }

/* safely add things cast to uintptr_t, add them, and then cast them back to (void*) */
#define PTR_ADD(x, y) ((void*) (((uintptr_t)x) + ((uintptr_t)y)))
#define PTR_SUBTRACT(x, y) ((void*) (((uintptr_t)x) - ((uintptr_t)y)))
#ifdef DEBUG
#define CDebugLog(...) printf(__VA_ARGS__)
#define CDebugLogEverySecond(...) { static time_t _temporary_last = 0; time_t _temporary_now; time(&_temporary_now); if (_temporary_last != _temporary_now) { printf(__VA_ARGS__); _temporary_last = _temporary_now; } }

#ifdef oc_NSDbg
#define DebugLog(...) oc_NSDbg(__VA_ARGS__)
#else
#define DebugLog(...) NSLog(__VA_ARGS__)
#endif
#else
#define CDebugLogEverySecond(...)
#define CDebugLog(...) 
#define DebugLog(...)
#endif
