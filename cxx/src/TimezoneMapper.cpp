/**
 * TimezoneMapper.cpp
 *
 * Copyright 2016-2023 Heartland Software Solutions Inc.
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

#include "TimeZoneMapper.h"
#include "times_internal.h"
#include "types.h"

#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#ifdef HAVE_CSTDLIB
#include <cstdlib>
#elif defined(HAVE_STDLIB_H)
#include <stdlib.h>
#else
#error No StdLib definition.
#endif

#ifdef __GNUC__
#include <strings.h>
#endif

// from ZoneDetect
#include "open/out_v1/timezone21.h"
#include "open/tzdb-2021e-src/africa.h"
#include "open/tzdb-2021e-src/antarctica.h"
#include "open/tzdb-2021e-src/asia.h"
#include "open/tzdb-2021e-src/australasia.h"
#include "open/tzdb-2021e-src/backward.h"
#include "open/tzdb-2021e-src/etcetera.h"
#include "open/tzdb-2021e-src/europe.h"
#include "open/tzdb-2021e-src/leapseconds.h"
#include "open/tzdb-2021e-src/northamerica.h"
#include "open/tzdb-2021e-src/southamerica.h"
#include "open/tzdb-2021e-src/version.h"
#include "open/tzdb-2021e-src/windowsZones.h"

using namespace HSS_Time;


ZoneDetect* TimezoneMapper::cd = nullptr;
std::vector<::TimeZoneInfo*> TimezoneMapper::timezones;
CThreadSemaphore TimezoneMapper::lock;

static double RADIAN_TO_DEGREE(const double X) {
	return (X * 180.0) * 0.318309886183790671537768;
}


const HSS_Time::TimeZoneInfo* TimezoneMapper::fromName(const char *name, INTNM::int16_t set) {
	using namespace std;
	using namespace std::chrono;
	using namespace date;

	initTz();

	CThreadSemaphoreEngage engage(&lock, true);

	for (auto tzi0 : timezones) {
		if (!stricmp(name, tzi0->m_name)) {
			if (set == (tzi0->m_dst.GetTotalSeconds() != 0))
				return tzi0;
		}
	}

	date::sys_info infos, infod, tmp;

	std::string tz_name;
#ifdef _WIN32
	if (tzdb::native_to_standard_timezone_name(name, tz_name))
		name = tz_name.c_str();
#endif
	auto tz = date::locate_zone(name);
	if (tz) {
		infos = tz->get_info(date::sys_days{ 2022_y / jan / 2 });
		infod = tz->get_info(date::sys_days{ 2022_y / jul / 2 });

		if (infos.save.count()) {	// this can occur in the southern hemisphere
			tmp = infos;
			infos = infod;
			infod = tmp;
		}

		const HSS_Time::TimeZoneInfo* tzi;
		if (set == -1)
			set = 0;
		if (set)
			tzi = addTz(infod, name);
		else
			tzi = addTz(infos, name);
		return tzi;
	}
	weak_assert(false);
	return nullptr;
}


const HSS_Time::TimeZoneInfo* TimezoneMapper::fromId(std::uint32_t id, INTNM::int16_t set) {
	using namespace std;
	using namespace std::chrono;
	using namespace date;

	initTz();

	CThreadSemaphoreEngage engage(&lock, true);

	for (auto tzi0 : timezones) {
		if (id == tzi0->m_id) {
			int dst = tzi0->m_dst.GetTotalSeconds();
			if (set == -1)
				set = 0;
			if ((set == -2) || (set == (dst != 0)))
				return tzi0;
		}
	}
	weak_assert(false);
	return nullptr;
}


constexpr int OPEN_TIMEZONE_ID = 0x80000;									// matching the constexpr def'n's in worldlocation.cpp
constexpr bool IS_OPEN(int id) { return (OPEN_TIMEZONE_ID & id) != 0; }


const HSS_Time::TimeZoneInfo* TimezoneMapper::addTz(const date::sys_info& si, const std::string& name) {
	for (auto tzi0 : timezones) {
		if ((si.offset - si.save).count() == tzi0->m_timezone.GetTotalSeconds())
			if (si.offset.count() == (tzi0->m_timezone + tzi0->m_dst).GetTotalSeconds())
				if (!strcmp(si.abbrev.c_str(), tzi0->m_code))
					if (!strcmp(name.c_str(), tzi0->m_name))
						return tzi0;
	}
	TimeZoneInfo *tzi = new TimeZoneInfo();
	tzi->m_code = strdup(si.abbrev.c_str());
	tzi->m_dst = WTimeSpan(std::chrono::seconds(si.save).count());
	tzi->m_timezone = WTimeSpan((si.offset - si.save).count());
	tzi->m_name = strdup(name.c_str());
	tzi->m_id = OPEN_TIMEZONE_ID + timezones.size();	// different offset from STD_TIMEZONE_ID, DST_TIMEZONE_ID, MIL_TIMEZONE_ID
	if (timezones.capacity() == timezones.size())
		timezones.reserve(timezones.capacity() + 128);
	timezones.push_back(tzi);
	return tzi;
}


static bool same_sys_info(const date::sys_info& s0, const date::sys_info& s1) {
	if (s0.begin != s1.begin)
		return false;
	if (s0.end != s1.end)
		return false;
	if (s0.offset != s1.offset)
		return false;
	if (s0.save != s1.save)
		return false;
	if (stricmp(s0.abbrev.c_str(), s1.abbrev.c_str()))
		return false;
	return true;
}


bool TimezoneMapper::initTz() {
	CThreadSemaphoreEngage engage(&lock, true);
	if (!cd) {
		cd = ZDOpenDatabaseFromMemory((void*)timezone21_bin, timezone21_bin_size);
		if (!cd)
			return false;

		date::add_inmemory_file(africa, africa_size);
		date::add_inmemory_file(antarctica, antarctica_size);
		date::add_inmemory_file(asia, asia_size);
		date::add_inmemory_file(australasia, australasia_size);
		date::add_inmemory_file(backward, backward_size);
		date::add_inmemory_file(etcetera, etcetera_size);
		date::add_inmemory_file(europe, europe_size);
		date::add_inmemory_file(leapseconds, leapseconds_size);
		date::add_inmemory_file(northamerica, northamerica_size);
		date::add_inmemory_file(southamerica, southamerica_size);
		std::string _version((char *)version);
		date::version_inmemory_file(_version);
		date::xml_inmemory_file(windowsZones_xml, windowsZones_xml_size);

		date::get_tzdb();
	}
	return true;
}


const ::TimeZoneInfo* TimezoneMapper::getTz(const double lat, const double lng, INTNM::int16_t set, bool* valid)
{
	using namespace std;
	using namespace std::chrono;
	using namespace date;

	initTz();

	const HSS_Time::TimeZoneInfo* tzi = nullptr;
	float safezone;
	date::sys_info infos, infod, tmp;
	ZoneDetectResult* results = ZDLookup(cd, lat, lng, &safezone);
	INTNM::int32_t index = 0, i;
	std::string zonestr, zoneset;
	while (results[index].lookupResult != ZD_LOOKUP_END) {
		for (i = 0; i < results[index].numFields; i++) {
			if (!strcmp(results[index].fieldNames[i], "TimezoneIdPrefix")) {
				zonestr += results[index].data[i];
				break;
			}
		}
		for (i = 0; i < results[index].numFields; i++) {
			if (!strcmp(results[index].fieldNames[i], "TimezoneId")) {
				zonestr += results[index].data[i];
				break;
			}
		}

		zonestr.erase(std::remove(zonestr.begin(), zonestr.end(), '['), zonestr.end());
		zonestr.erase(std::remove(zonestr.begin(), zonestr.end(), ']'), zonestr.end());

		date::zoned_time<date::days> zt1{ zonestr.c_str() };
		auto tz = zt1.get_time_zone();

		infos = tz->get_info(date::sys_days{ 2022_y / jan / 2 });
		infod = tz->get_info(date::sys_days{ 2022_y / jul / 2 });

		if (infos.save.count()) {
			tmp = infos;
			infos = infod;
			infod = tmp;
		}

		if (set == -1)
			set = 0;
		if (set)
			tzi = addTz(infod, tz->name());
		else
			tzi = addTz(infos, tz->name());

		index++;
		if (!zoneset.empty())
			zoneset += ',';
		zoneset += zonestr;
		zonestr.clear();
	}

	weak_assert(tzi != nullptr);
	*valid = (tzi != nullptr);
	return tzi;
}
