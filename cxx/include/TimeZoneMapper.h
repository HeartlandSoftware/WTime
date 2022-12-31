#pragma once
#include "open/date/include/date/tz.h"
#include "semaphore.h"
#include "worldlocation.h"
#include "library/zonedetect.h"
#include <string>

class TimezoneMapper {
public:
	static const HSS_Time::TimeZoneInfo* getTz(const double lat, const double lng, INTNM::int16_t set, bool* valid);
	static const HSS_Time::TimeZoneInfo* fromName(const char *name, INTNM::int16_t set);
	static const HSS_Time::TimeZoneInfo* fromId(std::uint32_t id, INTNM::int16_t set);
	static std::vector<HSS_Time::TimeZoneInfo*> timezones;

private:
	static CThreadSemaphore lock;
	static ZoneDetect* cd;

	static bool initTz();
	static const HSS_Time::TimeZoneInfo* addTz(const date::sys_info& si, const std::string& name);
};
