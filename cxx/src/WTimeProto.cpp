/**
 * WTimeProto.cpp
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

#include "WTimeProto.h"
#include "types.h"
#include "str_printf.h"
#include "worldlocation.h"

HSS::Times::WTime* HSS_Time::Serialization::TimeSerializer::serializeTime(const WTime& time, const std::uint32_t version)
{
	auto ret = new HSS::Times::WTime();
	ret->set_time(time.ToString(WTIME_FORMAT_STRING_ISO8601));
	auto timezone = new google::protobuf::StringValue();
	if (time.GetTimeManager()) {
		if (time.GetTimeManager()->m_worldLocation.m_startDST() == time.GetTimeManager()->m_worldLocation.m_endDST() &&
			time.GetTimeManager()->m_worldLocation.m_timezone() == WTimeSpan(0))
		{
			timezone->set_value("UTC");
		}
		else
		{
			auto zone = time.GetTimeManager()->m_worldLocation.CurrentTimeZone(0);
			if (zone) {
				if (version == 1) {
					zone = time.GetTimeManager()->m_worldLocation.DowngradeTimeZone();
					timezone->set_value(zone->m_code);
					ret->set_allocated_timezone(timezone);
					auto id = new google::protobuf::Int32Value();
					id->set_value(zone->m_id);
					ret->set_allocated_timezone_id(id);
				}
				else {
					timezone->set_value(zone->m_name);
					ret->set_allocated_timezone(timezone);
					auto daylight = new google::protobuf::StringValue();
					daylight->set_value(time.GetTimeManager()->m_worldLocation.dstExists() ? "true" : "false");
					ret->set_allocated_daylight(daylight);
				}
			}
			else
			{
				timezone->set_value(time.GetTimeManager()->m_worldLocation.m_timezone().ToString(WTIME_FORMAT_EXCLUDE_SECONDS));
				ret->set_allocated_timezone(timezone);
				if (time.GetTimeManager()->m_worldLocation.m_endDST() != time.GetTimeManager()->m_worldLocation.m_startDST() &&
					time.GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds() > 0)
				{
					auto daylight = new google::protobuf::StringValue();
					daylight->set_value(time.GetTimeManager()->m_worldLocation.m_amtDST().ToString(WTIME_FORMAT_EXCLUDE_SECONDS));
					ret->set_allocated_daylight(daylight);
				}
			}
		}
	}
	return ret;
}

auto HSS_Time::Serialization::TimeSerializer::deserializeTime(const HSS::Times::WTime& time, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> DeserializationData
{
	DeserializationData retval;

	//location is used as a return value from ParseDateTime to get the timezone that was deserialized
	WorldLocation location;
	location.m_timezone(WTimeSpan(0));
	location.m_startDST(WTimeSpan(0));
	location.m_endDST(WTimeSpan(0));
	location.m_amtDST(WTimeSpan(0));
	//location0 and manager are for initializing the WTime that will do the parsing
	WorldLocation location0(location);
	location0.m_timezone(WTimeSpan(0, 0, 0, 0));//this line is probably redundant, should remove
	WTimeManager manager(location0);
	WTime wtime(&manager);
	if (!wtime.ParseDateTime(time.time(), WTIME_FORMAT_STRING_ISO8601, &location)) {
		if (valid)
			valid->add_child_validation("HSS.Times.WTime", name, validation::error_level::WARNING, validation::id::parse_failed, time.time(), "Expected ISO-8601 (YYYY-mm-ddTHH:MM:SS");
		retval.valid = false;
		return retval;
	}
	retval.valid = true;
	//now we need another world location so that we can correct the time in the original WTime with the deserialized timezone
	WorldLocation location2(location);
	if (location2.m_timezone().GetTotalSeconds() == -1)
		location2.m_timezone(WTimeSpan(0));
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
				if (boost::iequals(time.daylight().value(), "LDT") || boost::iequals(time.daylight().value(), "D") || boost::iequals(time.daylight().value(), "true"))
				{
					daylightFlag = 1;
					daylight = 3600;
				}
				if (boost::iequals(time.daylight().value(), "LST") || boost::iequals(time.daylight().value(), "S") || boost::iequals(time.daylight().value(), "false"))
				{
					daylightFlag = 0;
					daylight = 0;
				}
			}
		}

		if (time.has_timezone_id()) {
			const HSS_Time::TimeZoneInfo* timezone;
			timezone = WorldLocation::TimeZoneFromId(time.timezone_id().value());
			location2.SetTimeZoneOffset(timezone);
		}
		//the timezone is an HH:MM offset
		else if (std::count_if(time.timezone().value().begin(), time.timezone().value().end(), [](auto& val) { return val == ':' || val == '-' || val == '+' || ::isdigit(val); })
				== time.timezone().value().size())
		{
			WTimeSpan span;
			span.ParseTime(time.timezone().value());
			location2.m_timezone(span);
			if (daylight)
			{
				location2.m_amtDST(WTimeSpan(daylight));
				location2.m_endDST(WTimeSpan(366, 0, 0, 0));
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
				location2.SetTimeZoneOffset(timezone);
				//daylight time was requested but the parsed timezone is not DST, adjust accordingly
				//will unset the stored timezone details
				if (daylight && !location2.dstExists())
				{
					location2.m_amtDST(WTimeSpan(daylight));
					location2.m_endDST(WTimeSpan(366, 0, 0, 0));
					//correct for differences between the specified DST offset and the one that
					//the timezone thinks it should have
					location2.m_timezone(location2.m_timezone() + (timezone->m_dst - location2.m_amtDST()));
				}
			}
			else if (daylight)
			{
				location2.m_amtDST(WTimeSpan(daylight));
				location2.m_endDST(WTimeSpan(366, 0, 0, 0));
				//can't find a valid timezone so use the one from the ISO8601 string
				//offset that value by the known DST offset so that the total offset
				//is the same
				location2.m_timezone(location2.m_timezone() - location2.m_amtDST());
			}
		}
	}

	//finally get the time that we were looking for
	if (location2.m_timezoneInfo())
		retval.timezone = location2.m_timezoneInfo()->m_id;
	else
		retval.timezone = 0;
	retval.time = wtime.GetTime(0);
	retval.offset = location2.m_timezone().GetTotalSeconds();
	if (location2.m_startDST() == location2.m_endDST())
		retval.dst = 0;
	else
		retval.dst = location2.m_amtDST().GetTotalSeconds();

	//the original time was in local time, reset it to UTC
	if ((location.m_timezone().GetTotalSeconds() == -1) && (retval.offset != 0 || retval.dst != 0))
		retval.time = retval.time - (retval.offset + retval.dst);

	return retval;
}

auto HSS_Time::Serialization::TimeSerializer::deserializeTime(const HSS::Times::WTime& time, const WTimeManager* manager, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> WTime*
{
	DeserializationData data = deserializeTime(time, valid, name);
	if (!data.valid)
		return nullptr;

	WTime *wt = new WTime(data.time, manager);
	return wt;
}

HSS::Times::WTimeSpan* HSS_Time::Serialization::TimeSerializer::serializeTimeSpan(const WTimeSpan& span)
{
	auto ret = new HSS::Times::WTimeSpan();
	ret->set_time(span.ToString(WTIME_FORMAT_YEAR | WTIME_FORMAT_DAY | WTIME_FORMAT_INCLUDE_USECS));
	return ret;
}

auto HSS_Time::Serialization::TimeSerializer::deserializeTimeSpan(const HSS::Times::WTimeSpan& span, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> WTimeSpan*
{
	auto ret = new WTimeSpan();
	if (!ret->ParseTime(span.time())) {
		if (valid)
			valid->add_child_validation("HSS.Times.WTimeSpan", name, validation::error_level::WARNING, validation::id::parse_failed, span.time(), "Recommended ISO-8601 ([-]THH:MM:SS");
		delete ret;
		return nullptr;
	}
	return ret;
}

HSS::Times::WTimeZone* HSS_Time::Serialization::TimeSerializer::serializeTimeZone(const HSS_Time::WorldLocation& worldLocation, const std::uint32_t version) {
	auto ret = new HSS::Times::WTimeZone();
	ret->set_version(version);
	const TimeZoneInfo* tzi;
	if (version == 1)
		tzi = worldLocation.DowngradeTimeZone();
	else
		tzi = worldLocation.m_timezoneInfo();

	if (tzi) {
		if (version == 1) {
			ret->set_timezoneindex(tzi->m_id);
		}
		else {
			auto msg = ret->mutable_tztimezone();
			msg->set_allocated_name(new std::string(tzi->m_name));
			msg->set_daylight(tzi->m_dst.GetTotalSeconds() ? true : false);
		}
	} else {
		auto msg = ret->mutable_timezonedetails();
		msg->set_allocated_amttimezone(serializeTimeSpan(worldLocation.m_timezone()));
		msg->set_allocated_startdst(serializeTimeSpan(worldLocation.m_startDST()));
		msg->set_allocated_enddst(serializeTimeSpan(worldLocation.m_endDST()));
		msg->set_allocated_amtdst(serializeTimeSpan(worldLocation.m_amtDST()));
	}
	return ret;
}

void HSS_Time::Serialization::TimeSerializer::deserializeTimeZone(const HSS::Times::WTimeZone& zone, HSS_Time::WorldLocation& worldLocation, std::shared_ptr<validation::validation_object> valid, const std::string& name) {
	if ((zone.version() != 1) && (zone.version() != 2)) {
		weak_assert(false);
		if (valid)
			valid->add_child_validation("HSS.Times.WTimeZone", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(zone.version()));
		throw std::invalid_argument("HSS.Times.WTimeZone: Version is invalid");
	}

#ifdef _DEBUG
	auto z = zone.msg_case();
#endif

	if (zone.msg_case() == HSS::Times::WTimeZone::kTimezoneDetails) {
		auto vt = validation::conditional_make_object(valid, "HSS.Times.WTimeZone", name);
		auto v = vt.lock();
		auto tz = deserializeTimeSpan(zone.timezonedetails().amttimezone(), v, "timezoneDetails.amtTimeZone");
		auto sdst = deserializeTimeSpan(zone.timezonedetails().startdst(), v, "timezoneDetails.startDST");
		auto edst = deserializeTimeSpan(zone.timezonedetails().enddst(), v, "timezoneDetails.endDst");
		auto adst = deserializeTimeSpan(zone.timezonedetails().amtdst(), v, "timezoneDetails.amtDST");
		worldLocation.m_timezone(*tz);
		worldLocation.m_startDST(*sdst);
		worldLocation.m_endDST(*edst);
		worldLocation.m_amtDST(*adst);
		delete tz;
		delete sdst;
		delete edst;
		delete adst;

		// try to guess what the timezone ID is
		const HSS_Time::TimeZoneInfo* tzz;
		if ((worldLocation._amtDST.GetTotalSeconds() == 0) || (!worldLocation.dstExists()))	tzz = worldLocation.m_std_timezones;
		else																				tzz = worldLocation.m_dst_timezones;
		while (tzz->m_name) {
			if ((tzz->m_timezone == worldLocation.__timezone) && (tzz->m_dst == worldLocation._amtDST)) {
				worldLocation._timezoneInfo = tzz;
				break;
			}
			tzz++;
		}
	} else if (zone.msg_case() == HSS::Times::WTimeZone::kTimezoneIndex) {
		if (!worldLocation.SetTimeZoneOffset(zone.timezoneindex()) && (valid)) {
			valid->add_child_validation("HSS.Times.WTimeZone", name, validation::error_level::WARNING, validation::id::index_invalid, std::to_string(zone.timezoneindex()));
		}
	} else if (zone.msg_case() == HSS::Times::WTimeZone::kTztimezone) {
		const TimeZoneInfo* tzi;
		if (!(tzi = worldLocation.TimeZoneFromName(zone.tztimezone().name(), zone.tztimezone().has_daylight() && zone.tztimezone().daylight() ? 1 : 0)))
			valid->add_child_validation("HSS.Times.WTimeZone", name, validation::error_level::WARNING, validation::id::index_invalid, zone.tztimezone().name());
		else
			worldLocation.SetTimeZoneOffset(tzi);
	} else if (valid)
		valid->add_child_validation("HSS.Times.WTimeZone", name, validation::error_level::WARNING, validation::id::object_invalid, "Time Zone");
}
