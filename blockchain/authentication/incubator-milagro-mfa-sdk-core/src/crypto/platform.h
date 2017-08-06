/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

#ifndef PLATFORM_H
#define PLATFORM_H

#if _WIN64 /* Windows 64-bit build */
#define WORD_LENGTH 64
#define OS "Windows"
#elif _WIN32 /* Windows 32-bit build */
#define WORD_LENGTH 32
#define OS "Windows"
#elif __linux && __x86_64 /* Linux 64-bit build*/
#define WORD_LENGTH 64
#define OS "Linux"
#include <stdint.h>
#undef unsign32
#define unsign32 uint32_t
#define __int32 int32_t
#define __int64 int64_t
#elif __linux /* Linux 32-bit build */
#define WORD_LENGTH 32
#define OS "Linux"
#undef unsign32
#define unsign32 uint32_t
#define __int32 int32_t
#define __int64 int64_t
#elif __APPLE__
#define WORD_LENGTH 32
#define OS "Apple"
typedef int32_t __int32;
#undef unsign32
typedef uint32_t unsign32;
#else /* 32-bit C-Only build - should work on any little Endian processor */
#define WORD_LENGTH 32
#define OS "Universal"
#endif

#endif /* PLATFORM_H */
