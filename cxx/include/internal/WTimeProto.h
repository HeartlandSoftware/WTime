/**
 * WTimeProto.h
 *
 * Copyright 2016-2019 Heartland Software Solutions Inc.
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

#include <cstdint>
#include "worldlocation.h"
#include "Times.h"
#include "wtime.pb.h"
#include "times_internal.h"

#include <boost/algorithm/string/predicate.hpp>

namespace HSS_Time
{
	namespace Serialization
	{
		struct TIMES_API DeserializationData
		{
			std::uint64_t time;
			std::int64_t offset;
			std::int64_t dst;
		};

		class TIMES_API TimeSerializer
		{
		public:
			static HSS::Times::WTime* serializeTime(const WTime& time);

			static DeserializationData deserializeTime(const HSS::Times::WTime& time);

			static WTime* deserializeTime(const HSS::Times::WTime& time, const WTimeManager* manager);

			static HSS::Times::WTimeSpan* serializeTimeSpan(const WTimeSpan& span);

			static WTimeSpan* deserializeTimeSpan(const HSS::Times::WTimeSpan& span);
		};
	}
}