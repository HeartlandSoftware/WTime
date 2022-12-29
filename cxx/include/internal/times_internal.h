/**
 * times_internal.h
 *
 * Copyright 2016-2022 Heartland Software Solutions Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the license at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the LIcense is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "intel_check.h"
#include "config.h"
#include "hssconfig/config.h"


#ifdef HAVE_CSTDINT
#include <cstdint>
#define INTNM std
#ifndef NULL
#define NULL  nullptr
#endif
#elif defined HAVE_STDINT_H
#include <stdint.h>
#define INTNM 
#ifndef NULL
#define NULL 0
#endif
#else
#error No StdInt definition.
#endif

#ifdef HAVE_CSTDIO
#include <cstdio>
#if defined(_MSC_VER) && _MSC_VER < 1900
#define tm_snprintf(str, len, frm, ...) c99_snprintf(str, len, frm, __VA_ARGS__)
#else
#define tm_snprintf(str, len, frm, ...) snprintf(str, len, frm, __VA_ARGS__)
#endif
#elif HAVE_STDIO_H
#include <stdio.h>
#define tm_snprintf(str, len, frm, args...) sprintf(str, frm, args)
#else
#error No StdIO definition.
#endif

#ifdef _MSC_VER
#ifdef _AFXDLL
#define TIMES_WINDOWS 1
#else
#undef _UNICODE
#endif
#include <tchar.h>

#if _MSC_VER < 1900
extern INTNM::int32_t c99_vsnprintf(char *outBuf, size_t size, const char* format, va_list ap);
extern INTNM::int32_t c99_snprintf(char *outBuf, size_t size, const char* format, ...);
#endif //_MSC_VER < 1900

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#define MSVC_COMPILER 1
#endif //_MSC_VER && !__INTEL_COMPILER
#define __stricmp _stricmp
#define __strdup _strdup
#define __strtok strtok_s
#else //_MSC_VER

#define __stricmp strcasecmp
#define __strdup strdup
#define __strtok strtok_r

#ifdef _UNICODE
#ifndef _TCHAR_DEFINED
typedef wchar_t TCHAR;
#define _TCHAR_DEFINED
#endif //_TCHAR_DEFINED

#define __TEXT(q)	L##q

#else //_UNICODE
#ifndef _TCHAR_DEFINED
typedef char TCHAR;
#define _TCHAR_DEFINED
#endif // _TCHAR_DEFINED

#define __TEXT(q)	q

#endif //_UNICODE

#ifndef _T
#define _TEXT(x)	__TEXT(x)
#define _T(x)		__TEXT(x)
#endif

#endif

#if defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
#define INTEL_COMPILER 1
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__)
#  define TIMES_STATIC 0
#  ifdef TIMES_EXPORT
#    ifdef __GNUC__
#      define TIMES_API __attribute__((dllexport))
#      define NO_THROW __attribute__((nothrow))
#    else
#      define TIMES_API __declspec(dllexport)
#      define NO_THROW __declspec(nothrow)
#    endif
#  else
#    ifdef __GNUC__
#      define TIMES_API __attribute__((dllimport))
#      define NO_THROW __attribute__((nothrow))
#    else
#      define TIMES_API __declspec(dllimport)
#      define NO_THROW __declspec(nothrow)
#    endif
#  endif
#else
#  define TIMES_STATIC 1
#  define NO_THROW __attribute__((nothrow))
#  if __GNUC__ >= 4
#    define TIMES_API __attribute__((visibility("default")))
#  else
#    define TIMES_API
#  endif
#endif
