/**
 * WTime.h
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

#include "internal/worldlocation.h"
#include "internal/Times.h"
#include "internal/SunriseSunsetCalc.h"

#if !defined(_MANAGED) && defined(GOOGLE_PROTOBUF_VERSION)
#include "internal/WTimeProto.h"
#endif
