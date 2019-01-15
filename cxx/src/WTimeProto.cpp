/**
 * WTimeProto.cpp
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

#include "WTimeProto.h"


HSS::Times::WTime* HSS_Time::Serialization::TimeSerializer::serializeTime(const WTime& time)
{
	auto ret = new HSS::Times::WTime();
	ret->set_time(time.ToString(WTIME_FORMAT_STRING_ISO8601));
	auto timezone = new google::protobuf::StringValue();
	if (time.GetTimeManager()) {
		if (time.GetTimeManager()->m_worldLocation.m_startDST == time.GetTimeManager()->m_worldLocation.m_endDST &&
			time.GetTimeManager()->m_worldLocation.m_timezone == WTimeSpan(0))
		{
			timezone->set_value("UTC");
		}
		else
		{
			auto zone = time.GetTimeManager()->m_worldLocation.CurrentTimeZone(0);
			if (zone)
				timezone->set_value(zone->m_code);
			else
			{
				timezone->set_value(time.GetTimeManager()->m_worldLocation.m_timezone.ToString(WTIME_FORMAT_EXCLUDE_SECONDS));
				if (time.GetTimeManager()->m_worldLocation.m_endDST != time.GetTimeManager()->m_worldLocation.m_startDST &&
					time.GetTimeManager()->m_worldLocation.m_amtDST.GetTotalSeconds() > 0)
				{
					auto daylight = new google::protobuf::StringValue();
					daylight->set_value(time.GetTimeManager()->m_worldLocation.m_amtDST.ToString(WTIME_FORMAT_EXCLUDE_SECONDS));
					ret->set_allocated_daylight(daylight);
				}
			}
		}
		ret->set_allocated_timezone(timezone);
	}
	return ret;
}

auto HSS_Time::Serialization::TimeSerializer::deserializeTime(const HSS::Times::WTime& time) -> DeserializationData
{
	DeserializationData retval;

	WorldLocation location;
	location.m_timezone = WTimeSpan(0);
	location.m_startDST = WTimeSpan(0);
	location.m_endDST = WTimeSpan(0);
	location.m_amtDST = WTimeSpan(0);
	WorldLocation location0(location);
	location.m_timezone = WTimeSpan(0, 0, 0, -1);
	WTimeManager manager(location0);
	WTime wtime(&manager);
	wtime.ParseDateTime(time.time(), WTIME_FORMAT_STRING_ISO8601, &location);
	WorldLocation location2(location);
	if (location2.m_timezone.GetTotalSeconds() == -1)
		location2.m_timezone = WTimeSpan(0);
	if (time.has_timezone())
	{
		//parse the optional DST value
		std::int64_t daylight = 0;
		std::int16_t daylightFlag = -1;
		if (time.has_daylight())
		{
			if (std::any_of(time.daylight().value().begin(), time.daylight().value().end(), ::isdigit))
			{
				WTimeSpan span;
				span.ParseTime(time.daylight().value());
				if (span.GetTotalSeconds() == 0)
					daylightFlag = 0;
				else if (span.GetTotalSeconds() == 3600 || span.GetTotalSeconds() == -3600)
				{
					daylightFlag = 1;
					daylight = span.GetTotalSeconds();
				}
				else
				{
					daylightFlag = 2;
					daylight = span.GetTotalSeconds();
				}
			}
			else
			{
				if (boost::iequals(time.daylight().value(), "LDT") || boost::iequals(time.daylight().value(), "D"))
				{
					daylightFlag = 1;
					daylight = 3600;
				}
			}
		}

		//the timezone is an HH:MM offset
		if (std::count_if(time.timezone().value().begin(), time.timezone().value().end(), [](auto& val) { return val == ':' || val == '-' || val == '+' || ::isdigit(val); })
				== time.timezone().value().size())
		{
			WTimeSpan span;
			span.ParseTime(time.timezone().value());
			location2.m_timezone = span;
			if (daylight)
			{
				location2.m_amtDST = WTimeSpan(daylight);
				location2.m_endDST = WTimeSpan(366, 0, 0, 0);
			}
		}
		//the timezone is an abbreviation or name
		else
		{
			const HSS_Time::TimeZoneInfo* timezone;
			if (daylightFlag == 1 || daylightFlag == 2)
				timezone = WorldLocation::TimeZoneFromName(time.timezone().value(), 1);
			else
			{
				timezone = WorldLocation::TimeZoneFromName(time.timezone().value(), 0);
				//if neither LST nor LDT were specified, try both
				if (!timezone && daylightFlag == -1)
					timezone = WorldLocation::TimeZoneFromName(time.timezone().value(), 1);
			}

			if (timezone)
			{
				location2.m_timezone = timezone->m_timezone;
				if (daylight)
				{
					location2.m_amtDST = WTimeSpan(daylight);
					location2.m_endDST = WTimeSpan(366, 0, 0, 0);
					//correct for differences between the specified DST offset and the one that
					//the timezone thinks it should have
					location2.m_timezone = location2.m_timezone + (timezone->m_dst - location2.m_amtDST);
				}
				else if (timezone->m_dst.GetTotalSeconds() != 0)
				{
					location2.m_amtDST = timezone->m_dst;
					location2.m_endDST = WTimeSpan(366, 0, 0, 0);
				}
			}
			else if (daylight)
			{
				location2.m_amtDST = WTimeSpan(daylight);
				location2.m_endDST = WTimeSpan(366, 0, 0, 0);
				//can't find a valid timezone so use the one from the ISO8601 string
				//offset that value by the known DST offset so that the total offset
				//is the same
				location2.m_timezone -= location2.m_amtDST;
			}
		}
	}

	retval.time = wtime.GetTime(0);
	retval.offset = location2.m_timezone.GetTotalSeconds();
	if (location2.m_startDST == location2.m_endDST)
		retval.dst = 0;
	else
		retval.dst = location2.m_amtDST.GetTotalSeconds();

	//the original time was in local time, reset it to UTC
	if ((location.m_timezone.GetTotalSeconds() == -1) && (retval.offset != 0 || retval.dst != 0))
		retval.time = retval.time - (retval.offset + retval.dst);

	return retval;
}

auto HSS_Time::Serialization::TimeSerializer::deserializeTime(const HSS::Times::WTime& time, const WTimeManager* manager) -> WTime*
{
	DeserializationData data = deserializeTime(time);
	return new WTime(data.time, manager);
}

HSS::Times::WTimeSpan* HSS_Time::Serialization::TimeSerializer::serializeTimeSpan(const WTimeSpan& span)
{
	auto ret = new HSS::Times::WTimeSpan();
	ret->set_time(span.ToString(WTIME_FORMAT_YEAR | WTIME_FORMAT_DAY | WTIME_FORMAT_INCLUDE_USECS));
	return ret;
}

auto HSS_Time::Serialization::TimeSerializer::deserializeTimeSpan(const HSS::Times::WTimeSpan& span) -> WTimeSpan*
{
	auto ret = new WTimeSpan();
	ret->ParseTime(span.time());

	return ret;
}