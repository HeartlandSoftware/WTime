/**
 * worldlocation.h
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


namespace HSS_Time {
	class WorldLocation;
	class WTimeSpan;
	class WTime;
};

#include "times_internal.h"
#include "Times.h"
#include <string>
#include "validation_object.h"
#include "hssconfig/config.h"
#ifdef HSS_USE_CACHING
#include "valuecache_mt.h"
#include "objectcache_mt.h"
#endif

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
#include <afx.h>
#endif


class Canada;


namespace HSS_Time {


#ifdef MSVC_COMPILER
#pragma managed(push, off)
#endif

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 4)
#endif


struct TIMES_API TimeZoneInfo {
	WTimeSpan	m_timezone;
	WTimeSpan	m_dst;
	const char	*m_code;
	const char	*m_name;
	std::uint32_t m_id;
};


class TIMES_API WorldLocation {		// this is what we have to define what a plot's location in the world is
    public:
	double		_latitude;			// stored as radians!!!
	double		_longitude;
	WTimeSpan	__timezone;			// time zone
	WTimeSpan	_startDST,			// when daylight savings turns on - from start of year
				_endDST,			// when daylight savings turns off - from start of year
							// if you want to disable DST, then just set these to the same value
				_amtDST;			// amount to adjust for DST
	const TimeZoneInfo* _timezoneInfo;

public:
	 ///<summary>
	 ///Retrieve the latitude of the stored location (radians).
	 ///</summary>
	inline double m_latitude() const noexcept { return _latitude; }

	///<summary>
	///Set the latitude of the stored location (radians).
	///</summary>
	inline void m_latitude(const double value) noexcept { _latitude = value; }

	///<summary>
	///Retrieve the longitude of the stored location (radians).
	///</summary>
	inline double m_longitude() const noexcept { return _longitude; }

	///<summary>
	///Set the longitude of the stored location (radians).
	///</summary>
	inline void m_longitude(const double value) noexcept { _longitude = value; }

	///<summary>
	///Retrieve the timezone offset for the world location.
	///</summary>
	inline const WTimeSpan& m_timezone() const noexcept { return __timezone; }

	///<summary>
	///Set the timezone offset for the world location. Clears the timezone if there is one.
	///</summary>
	inline void m_timezone(const WTimeSpan& value) { _timezoneInfo = nullptr; __timezone = value; }

	///<summary>
	///Set the timezone offset for the world location. Clears the timezone if there is one.
	///</summary>
	inline void m_timezone(WTimeSpan&& value) { _timezoneInfo = nullptr; __timezone = std::move(value); }

	///<summary>
	///Retrieve the time that DST begins for the current timezone.
	///</summary>
	inline const WTimeSpan& m_startDST() const noexcept { return _startDST; }

	///<summary>
	///Set the time that DST begins for the current timezone. Clears the timezone if there is one.
	///</summary>
	inline void m_startDST(const WTimeSpan& value) { _timezoneInfo = nullptr; _startDST = value; }

	///<summary>
	///Set the time that DST begins for the current timezone. Clears the timezone if there is one.
	///</summary>
	inline void m_startDST(WTimeSpan&& value) { _timezoneInfo = nullptr; _startDST = std::move(value); }

	///<summary>
	///Retrieve the time that DST ends for the current timezone.
	///</summary>
	inline const WTimeSpan& m_endDST() const noexcept { return _endDST; }

	///<summary>
	///Set the time that DST ends for the current timezone. Clears the timezone if there is one.
	///</summary>
	inline void m_endDST(const WTimeSpan& value) { _timezoneInfo = nullptr; _endDST = value; }

	///<summary>
	///Set the time that DST ends for the current timezone. Clears the timezone if there is one.
	///</summary>
	inline void m_endDST(WTimeSpan&& value) { _timezoneInfo = nullptr; _endDST = std::move(value); }

	///<summary>
	///Retrieve the offset that is applied to the time during DST.
	///</summary>
	inline const WTimeSpan& m_amtDST() const noexcept { return _amtDST; }

	///<summary>
	///Set the offset that is applied to the time during DST. Clears the timezone if there is one.
	///</summary>
	inline void m_amtDST(const WTimeSpan& value) { _timezoneInfo = nullptr; _amtDST = value; }

	///<summary>
	///Set the offset that is applied to the time during DST. Clears the timezone if there is one.
	///</summary>
	inline void m_amtDST(WTimeSpan&& value) { _timezoneInfo = nullptr; _amtDST = std::move(value); }

	///<summary>
	///Retrieve the timezone that was used to create this world location. May be null
	///if the world location was constructed manually.
	///</summary>
	inline const TimeZoneInfo* m_timezoneInfo() const noexcept { return _timezoneInfo; }

	///<summary>
	///Set the timezone that this world location represents. This will also update the timezone offset
	///and DST values.
	///</summary>
	inline void m_timezoneInfo(const TimeZoneInfo* value) { SetTimeZoneOffset(value); }

	///<summary>
	///Is there a period of time that DST will be enabled for the current timezone (start DST != end DST).
	///</summary>
	inline bool dstExists() const { return _startDST != _endDST; }

public:
	WTimeSpan	m_solar_timezone(const WTime &solar_time) const;
							// used for GMT->solar time math based on longitude only, but don't assume that solar noon means the sun is right over you!
	INTNM::int16_t m_sun_rise_set(const WTime &local_day, WTime *Rise, WTime *Set, WTime *Noon) const;
							// any time during the local "solar" day will glean the right times - suggestion is to use local noon time
	Canada *canada;

    public:
	WorldLocation();
	WorldLocation(const WorldLocation &wl);
	///<summary>
	/// Construct a new timezone at the given latitude and longitude. Optionally guess the timezone from those coordinates.
	///</summary>
	///<param name="latitude">The latitude in degrees</param>
	///<param name="longitude">The longitude in degrees</param>
	///<param name="guessTimezone">True if the timezone should be guessed from the latitude and longitude</param>
	WorldLocation(double latitude, double longitude, bool guessTimezone);
	~WorldLocation();


				// ***** input/output...
#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	friend TIMES_API CArchive& operator>>(CArchive& is, WorldLocation &wl);
	friend TIMES_API CArchive& operator<<(CArchive& os, const WorldLocation &wl);
#endif

	WorldLocation &operator=(const WorldLocation &wl);
	bool operator==(const WorldLocation &wl) const;
	bool operator!=(const WorldLocation &wl) const;

	const struct TimeZoneInfo *GuessTimeZone(INTNM::int16_t set) const;		// 0 for std time zones, 1 for dst time zones, -1 for military time zones
	const struct TimeZoneInfo *CurrentTimeZone(INTNM::int16_t set, bool* hidden = nullptr) const;		// 0 for std time zones, 1 for dst time zones, -1 for military time zones

	bool InsideCanada(const double latitude, const double longitude);
	bool InsideCanada() const;
	bool InsideNewZealand() const;
	bool InsideTasmania() const;
	bool InsideAustraliaMainland() const;

	///<summary>
	///Set the timezone offset and the DST amount using a <see href="TimeZoneInfo"/>.
	///<param name="timezone">The timezone to apply to this world location. If the
	///timezone is has a DST offset the end DST time will be set to day 366.</param>
	///</summary>
	void SetTimeZoneOffset(const TimeZoneInfo* timezone);
	bool SetTimeZoneOffset(std::uint32_t id);

	static const struct TimeZoneInfo m_std_timezones[];
	static const struct TimeZoneInfo m_dst_timezones[];
	static const struct TimeZoneInfo m_mil_timezones[];
	static const struct TimeZoneInfo m_std_extra_timezones[];
	static const struct TimeZoneInfo m_dst_extra_timezones[];

public:
	///<summary>
	///Get a WorldLocation object with the timezone data set correctly for the given latitude/longitude. If the area uses
	///daylight savings time at any point in the year it will be enabled.
	///</summary>
	///<param name="latitude">The locations latitude (in radians).</param>
	///<param name="longitude">The locations longitude (in radians).</param>
	///<param name="set">Whether we want back a STD or DST timezone</param>
	///<param name="valid">If supplied, will be set to 1 if a timezone was found for the corresponding location, and 0 otherwise</param>
	static WorldLocation FromLatLon(const double latitude, const double longitude, INTNM::int16_t set, bool* valid = NULL);
	///<summary>
	///Get a TimeZoneInfo object with the timezone data set correctly for the given latitude/longitude. If the area uses
	///daylight savings time at any point in the year it will be enabled.
	///</summary>
	///<param name="latitude">The locations latitude.</param>
	///<param name="longitude">The locations longitude.</param>
	///<param name="set">Whether we want back a STD or DST timezone</param>
	///<param name="valid">If supplied, will be set to 1 if a timezone was found for the corresponding location, and 0 otherwise</param>
	static const TimeZoneInfo* TimeZoneFromLatLon(const double latitude, const double longitude, INTNM::int16_t set, bool* valid = NULL);
	///<summary>
	///Get a TimeZoneInfo object with the timezone data set correctly for the given location name. If the area uses
	///daylight savings time at any point in the year it will be enabled.
	///</summary>
	///<param name="name">The locations name (eg. America/Winnipeg).</param>
	///<param name="set">Whether we want back a STD or DST timezone</param>
	///<param name="valid">If supplied, will be set to 1 if a timezone was found for the corresponding location, and 0 otherwise</param>
	static const TimeZoneInfo* TimeZoneFromLocation(const std::string& name, INTNM::int16_t set, bool* valid = NULL);

	static const TimeZoneInfo* TimeZoneFromName(const std::string& name, INTNM::int16_t set, bool* hidden = nullptr);

	static const TimeZoneInfo* TimeZoneFromId(std::uint32_t id, bool* hidden = nullptr);

	/// <summary>
	/// Get a timezone from a Windows zone name.
	/// </summary>
	/// <param name="name">The name of the Windows zone.</param>
	/// <returns>A TimeZone that represents the Windows zone, or nullptr if one couldn't be found.</returns>
	static const TimeZoneInfo* TimeZoneFromWindowsName(const std::string& name);

	/// <summary>
	/// Get a timezone from a region name (ex. America/Winnipeg).
	/// </summary>
	/// <param name="region">The name of the region to try to find the timezone for.</param>
	/// <returns>A TimeZone that represents the region, or nullptr if one couldn't be found.</returns>
	static const TimeZoneInfo* TimeZoneFromRegionName(const std::string& region);

	/// <summary>
	/// Get the daylight savings version of a standard timezone.
	/// </summary>
	/// <param name="info">The timezone to get the daylight savings version of.</param>
	/// <returns>A daylight savings timezone, or <paramref name="info"/> if one doesn't exist.</returns>
	static const TimeZoneInfo* GetDaylightSavingsTimeZone(const TimeZoneInfo* info);
	/// <summary>
	/// Get the standard version of a daylight savings timezone.
	/// </summary>
	/// <param name="info">The timezone to get the standard version of.</param>
	/// <returns>A standard timezone, or <paramref name="info"/> if one doesn't exist.</returns>
	static const TimeZoneInfo* GetStandardTimeZone(const TimeZoneInfo* info);

    private:
	struct sun_key {
		INTNM::uint64_t	m_sun_cache_tm;
		double		m_sun_cache_lat,
				m_sun_cache_long;
	};
	struct sun_val {
		INTNM::uint64_t	m_sun_cache_rise,
				m_sun_cache_set,
				m_sun_cache_noon;
		INTNM::int32_t		m_success;
	};
#ifdef HSS_USE_CACHING
	mutable ValueCacheTempl_MT<sun_key, sun_val>	m_sunCache;
	mutable ValueCacheTempl_MT<sun_key, WTimeSpan>m_solarCache;
#endif
};


#ifdef MSVC_COMPILER
#pragma managed(pop)
#endif

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)			// undo our packing rules to what they were before
#endif

};

