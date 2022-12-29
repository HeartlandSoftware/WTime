/**
 * Times.h
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

#include "times_internal.h"
#include "worldlocation.h"
#include "SunriseSunsetCalc.h"
#include "poly.gis.h"

#include <string>


#ifdef MSVC_COMPILER
#pragma managed(push, off)
#endif

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
#include <afx.h>
#include <ATLComTime.h>
#endif

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 4)
#endif

namespace HSS_Time {
	class WorldLocation;
	class WTimeSpan;
	class WTime;
};


#define WTIME_FORMAT_TIME				0x00100000	// stick on the time
#define WTIME_FORMAT_DAY				0x00200000	// stick on the day
#define WTIME_FORMAT_YEAR				0x00800000	// stick on the year, but (for WTime) you also need WTIME_FORMAT_MONTH or WTIME_FORMAT_DATE with it
#define WTIME_FORMAT_EXCLUDE_SECONDS	0x20000000	// forget about sticking the seconds onto the end of the string
#define WTIME_FORMAT_INCLUDE_USECS		0x00080000	// stick on the usecs
#define WTIME_FORMAT_CONDITIONAL_TIME	0x40000000	// if we have days but no hours, minutes, seconds, don't print the hours, minutes, secs

		// ***** indexes into things that we would stick onto drop-lists
#define ITERATION_1SEC					0
#define ITERATION_1MIN					1
#define ITERATION_5MIN					2
#define ITERATION_15MIN					3
#define ITERATION_30MIN					4
#define ITERATION_1HOUR					5
#define ITERATION_2HOUR					6
#define ITERATION_1DAY					7
#define ITERATION_1WEEK					8

		// use these #defines, or'd together, to
		// decide what AsString() produces
#define WTIME_FORMAT_MONTH				0x00400000	// stick on the month
#define WTIME_FORMAT_DATE				(WTIME_FORMAT_DAY | WTIME_FORMAT_MONTH)
#define WTIME_FORMAT_AS_LOCAL			0x01000000	// print it as local time
#define WTIME_FORMAT_AS_SOLAR			0x02000000	// print it as solar time
#define WTIME_FORMAT_DAY_OF_WEEK		0x00000100	// prepend with day of week
#define WTIME_FORMAT_ABBREV				0x00001000	// abbreviate the month, day of week
#define WTIME_FORMAT_WITHDST			0x04000000
#define WTIME_FORMAT_PARSE_USING_SYSTEM	0x80000000	// if this is used, then we use the system parser which will use the locale settings, otherwise
		// we use our own internal parser
		//		#define WTIME_FORMAT_EXCLUDE_SECONDS	0x0100			// forget about sticking the seconds onto the end of the string

#define WTIME_FORMAT_STRING_DD_MM_YYYY	0x00000001	// this is also the default for parsing
#define WTIME_FORMAT_STRING_YYYY_MM_DD	0x00000002
#define WTIME_FORMAT_STRING_MM_DD_YYYY	0x00000003
#define WTIME_FORMAT_STRING_DDhMMhYYYY	0x00000004	// same as above, but instead of '/', we are using '-' (h for hyphenated)
#define WTIME_FORMAT_STRING_YYYYhMMhDD	0x00000005
#define WTIME_FORMAT_STRING_MMhDDhYYYY	0x00000006
#define WTIME_FORMAT_STRING_YYYYMMDD	0x00000007
#define WTIME_FORMAT_STRING_YYYYMMDDHH	0x00000008
#define WTIME_FORMAT_STRING_YYYYMMDDT	0x00000010
#define WTIME_FORMAT_STRING_YYYYhMMhDDT	0x00000020
#define	WTIME_FORMAT_STRING_TIMEZONE	0x00000200
#define WTIME_FORMAT_STRING_ISO8601		(WTIME_FORMAT_STRING_TIMEZONE | WTIME_FORMAT_STRING_YYYYhMMhDDT | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)



namespace HSS_Time {

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	TIMES_API CArchive& AFXAPI operator<<(CArchive& ar, const WTimeSpan timeSpan);
	TIMES_API CArchive& AFXAPI operator>>(CArchive& ar, WTimeSpan& rtimeSpan);
	TIMES_API CArchive& AFXAPI operator<<(CArchive& ar, const WTime &time);
	TIMES_API CArchive& AFXAPI operator>>(CArchive& ar, WTime &rtime);
#endif


class TIMES_API WTimeSpan {
    friend class WTime;
    private:
	INTNM::int64_t m_timeSpan;

    public:
	WTimeSpan();
	explicit WTimeSpan(INTNM::int64_t /*time_t*/ time, bool units_are_seconds = true);
								// changing this constructor to avoid hick-ups between sec's and usec's
	WTimeSpan(INTNM::int32_t lDays, INTNM::int32_t nHours, INTNM::int32_t nMins, INTNM::int32_t nSecs);
	WTimeSpan(INTNM::int32_t lDays, INTNM::int32_t nHours, INTNM::int32_t nMins, const double &nSecs);
	WTimeSpan(INTNM::int32_t lDays, INTNM::int32_t nHours, INTNM::int32_t nMins, INTNM::int32_t nSecs, INTNM::int32_t uSecs);
	WTimeSpan(const WTimeSpan &timeSrc);
#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	explicit WTimeSpan(const COleDateTimeSpan &timeSrc);
	WTimeSpan(const CString &timeSrc, INTNM::int16_t *cnt = NULL);
#endif

	WTimeSpan(const std::string &timeSrc, INTNM::int16_t *cnt = NULL);			// set the time from a char *, assumes hr:mn:sc, where hr can
																				// be signed.  We don't expect a day prepending this format!

	std::string ToString(INTNM::uint32_t flags) const;

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	bool ParseTime(const CString &time);
#endif
	bool ParseTime(const TCHAR *lpszTime);
	bool ParseTime(const std::string &time);

	void SetTotalSeconds(INTNM::int64_t secs);
	
// Operations
	INTNM::int64_t GetYears() const;
		// can't do GetMonths() 'cause we need to know if it's a leap year or not
	INTNM::int64_t GetWeeks() const;
	INTNM::int64_t GetDays() const;
	INTNM::int64_t GetTotalHours() const;
	INTNM::int32_t GetHours() const;
	INTNM::int64_t GetTotalMinutes() const;
	INTNM::int32_t GetMinutes() const;
	INTNM::int64_t GetTotalSeconds() const;
	INTNM::int32_t GetSeconds() const;
	INTNM::int32_t GetMilliSeconds() const;
	INTNM::int32_t GetTotalMilliSeconds() const;
	INTNM::int32_t GetMicroSeconds() const;
	INTNM::int64_t GetTotalMicroSeconds() const;

	double GetDaysFraction() const;
	double GetSecondsFraction() const;
	double GetFractionOfSecond() const;
	double GetFractionOfMinute() const;
	double GetFractionOfHour() const;
	double GetFractionOfDay() const;
	INTNM::int32_t GetSecondsOfDay() const;

	void PurgeToSecond();
	void PurgeToMinute();
	void PurgeToHour();
	void PurgeToDay();

	// time math
	const WTimeSpan& operator=(const WTimeSpan &timeSrc);
	WTimeSpan operator-(const WTimeSpan &timeSpan) const;
	WTimeSpan operator+(const WTimeSpan &timeSpan) const;
	const WTimeSpan& operator-=(const WTimeSpan &timeSpan);
	const WTimeSpan& operator+=(const WTimeSpan &timeSpan);
	WTimeSpan operator*(INTNM::int32_t factor) const;
	WTimeSpan operator/(INTNM::int32_t factor) const;
	double operator/(const WTimeSpan &timeSpan) const;
	WTimeSpan operator*(double factor) const;
	WTimeSpan operator/(double factor) const;
	const WTimeSpan operator*=(INTNM::int32_t factor);
	const WTimeSpan operator/=(INTNM::int32_t factor);
	const WTimeSpan operator*=(double f);
	const WTimeSpan operator/=(double f);

#ifdef __cpp_impl_three_way_comparison
	auto operator<=>(const WTimeSpan& timeSpan) const;
#endif
	bool operator==(const WTimeSpan& timeSpan) const;
	bool operator!=(const WTimeSpan& timeSpan) const;
	bool operator<(const WTimeSpan& timeSpan) const;
	bool operator>(const WTimeSpan& timeSpan) const;
	bool operator<=(const WTimeSpan& timeSpan) const;
	bool operator>=(const WTimeSpan& timeSpan) const;

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	friend TIMES_API CArchive& AFXAPI HSS_Time::operator<<(CArchive& ar, const WTimeSpan timeSpan);
	friend TIMES_API CArchive& AFXAPI HSS_Time::operator>>(CArchive& ar, WTimeSpan& rtimeSpan);

	COleDateTimeSpan AsCOleDateTimeSpan() const;
#endif
};


class TIMES_API WTimeManager {
    public:
	const WorldLocation	&m_worldLocation;
					// where timezone, lat, and long are kept
    public:
	explicit WTimeManager(const WorldLocation &worldLocation);

	static WTimeSpan TimeForIndex(INTNM::int32_t index);
	static INTNM::int32_t IterationIndex(const WTimeSpan &time);  
	
    			// ***** general functions which are useful to the outside world
	static bool isLeapYear(INTNM::int16_t year);
					// is the given year a leap year?
    static INTNM::int16_t daysInMonth(INTNM::int16_t month, INTNM::int16_t year);
					// given a month [1..12] and a year, how many days in that month?
	static INTNM::int16_t GetJulianCount(INTNM::int16_t year);
					// returns number of days in the year
	static INTNM::int16_t ToJulian(INTNM::int16_t year, INTNM::int16_t month, INTNM::int16_t day);
					// converts month+day to julian (origin 0) for the provided year
	static void FromJulian(INTNM::int16_t julian, INTNM::int16_t *year, INTNM::int16_t *month, INTNM::int16_t *day);
					// years since 1900, month is 1..12,
					// day is 1..[], returns 0..365 (origin 0)   

	static const char *months_abbrev[12];
	static const char *months[12];
	static const char *days_abbrev[7];
	static const char *days[7];

	static const WTimeManager GetSystemTimeManager(WorldLocation& location);
};
                                                                                   

class TIMES_API WTime {				// this value is always stored in GMT time!!! - unless you play with constructors or do it manually
private:
	INTNM::uint64_t		m_time;	// this is a count of microseconds since January 1, 1600.  This may seem like an arbritrary point in time (and it is), but there is some
					// logic to this: the Gregorian calendar started on October 4, 1582:
					//	Gregorian Calendar - in 1582, Pope Gregory XIII reformed the Julian calendar to improve the accuracy. The current scheme for leap
					//	years was introduced, and 04 October 1582 (Julian calendar) was immediately followed by 15 October 1582 (Gregorian calendar) to
					//	correct for past errors due to the Julian calendar's implementation of leap years. For consistency, and to avoid having a 10-day
					//	discontinuity, astronomers use Julian Day when dealing with historical dates. The NOAA solar calculators simply extrapolate the
					//	Gregorian calendar back through time, so dates before 15 October 1582 should be corrected by the user. For more information, read
					//	the Gregorian Calendar page from Rice University.
					// I saw no need to go to specifically that date, so we've started at the start of the next century instead.  These notes are from NOAA,
					// which provided our code for sun rise and set.  They state that for these calculations to be valid before 1582, the user would have to
					// correct for leap years (and then the calculations would be reportedly good to year -2000).  But I don't foresee a need for this (right
					// now) so we'll leave this for future work if/as/when needed.
	const WTimeManager	*m_tm;	// used so we can get the timezone offset
	void construct_time_t(INTNM::int32_t nYear, INTNM::int32_t nMonth, INTNM::int32_t nDay, INTNM::int32_t nHour, INTNM::int32_t nMin, INTNM::int32_t nSec);
	INTNM::uint64_t adjusted_tm(INTNM::uint32_t flags) const;
	INTNM::uint64_t adjusted_tm_math(INTNM::uint32_t flags) const;
#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	bool systemParseDateTime(const TCHAR *lpszDate, INTNM::uint32_t flags);
#endif

public:
	explicit WTime(const WTimeManager *tm);
	WTime(INTNM::uint64_t /*time_t*/ time, const WTimeManager *tm, bool units_are_seconds = true);
	WTime(const GDALTime& time, const WTimeManager *tm);
	WTime(INTNM::int32_t nYear, INTNM::int32_t nMonth, INTNM::int32_t nDay, INTNM::int32_t nHour, INTNM::int32_t nMin, INTNM::int32_t nSec, const WTimeManager *tm);
	WTime(INTNM::int32_t nYear, INTNM::int32_t nMonth, INTNM::int32_t nDay, INTNM::int32_t nHour, INTNM::int32_t nMin, double nSec, const WTimeManager *tm);
	WTime(INTNM::int32_t nYear, INTNM::int32_t nMonth, INTNM::int32_t nDay, INTNM::int32_t nHour, INTNM::int32_t nMin, INTNM::int32_t nSec, INTNM::int32_t uSec, const WTimeManager *tm);
					// REMEMBER: years, hours, months, etc. are specified in GMT, even if
					// you attach a timezone to it!!!
#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	WTime(const COleDateTime& timeSrc, const WTimeManager *tm, INTNM::uint32_t flags);
#endif
	WTime(const WTime& timeSrc);
	WTime(const WTime& timeSrc, const WTimeManager *tm);
	WTime(const WTime& timeSrc, INTNM::uint32_t flags, INTNM::int16_t direction);		// positive means from GMT, negative means to GMT

public:
    static WTime Now(const WTimeManager *tm, INTNM::uint32_t flags);
	static WTime& GlobalMin();
	static WTime& GlobalMax();
	static WTime GlobalMin(const WTimeManager* tm);
	static WTime GlobalMax(const WTimeManager* tm);

#if TIMES_STATIC==1
private:
	static WTime _gmin, _gmax;
#endif

public:
	INTNM::uint64_t /*time_t*/ GetTime(INTNM::uint32_t mode) const;
	GDALTime AsGDALTime(INTNM::uint32_t mode) const;
	INTNM::uint64_t /*long*/ GetTotalSeconds() const;
	INTNM::uint64_t GetTotalMilliSeconds() const;
	INTNM::uint64_t GetTotalMicroSeconds() const;
	const WTimeManager *GetTimeManager() const;
	const WTimeManager *SetTimeManager(const WTimeManager *tm);		// VERY dangerous

	bool IsValid() const;
	
	INTNM::int32_t GetYear(INTNM::uint32_t flags) const;
	INTNM::int32_t GetMonth(INTNM::uint32_t flags) const;       // month of year (1 = Jan)
	INTNM::int32_t GetDay(INTNM::uint32_t flags) const;         // day of month
 	INTNM::int32_t GetHour(INTNM::uint32_t flags) const;
	INTNM::int32_t GetMinute(INTNM::uint32_t flags) const;
	INTNM::int32_t GetSecond(INTNM::uint32_t flags) const;
	double GetSecondsFraction(INTNM::uint32_t flags) const;
	INTNM::int32_t GetMilliSeconds(INTNM::uint32_t flags) const;
	INTNM::int32_t GetMicroSeconds(INTNM::uint32_t flags) const;
	WTimeSpan GetTimeOfDay(INTNM::uint32_t flags) const;
	double GetFractionOfSecond(INTNM::uint32_t flags) const;
	double GetFractionOfMinute(INTNM::uint32_t flags) const;
	double GetFractionOfHour(INTNM::uint32_t flags) const;
	double GetFractionOfDay(INTNM::uint32_t flags) const;
	INTNM::int32_t GetDayOfWeek(INTNM::uint32_t flags) const;

					// 1=Sun, 2=Mon, 3=Tues, 4=Wed, 5=Thurs, 5=Fri, 7=Sat
	INTNM::int32_t GetDayOfYear(INTNM::uint32_t flags) const;	// Julian date - so Jan 1 = 1
	double GetDayFractionOfYear(INTNM::uint32_t flags) const; // same as GetDayOfyear() but with the fractional portion of the year too
	INTNM::uint64_t GetSecondsIntoYear(INTNM::uint32_t flags) const;
	WTimeSpan GetWTimeSpanIntoYear(INTNM::uint32_t flags) const;
	bool IsLeapYear(INTNM::uint32_t flags) const;

 	void PurgeToSecond(INTNM::uint32_t flags);
	void PurgeToMinute(INTNM::uint32_t flags);
	void PurgeToHour(INTNM::uint32_t flags);
	void PurgeToDay(INTNM::uint32_t flags);
	void PurgeToYear(INTNM::uint32_t flags);

// Operations
	// time math
	const WTime& operator=(const WTime &timeSrc);
	const WTime& SetTime(const WTime &timeSrc);	// like the assignment operator but just sets the time, not the time manager
	WTime operator-(const WTimeSpan &timeSpan) const;
	WTime operator+(const WTimeSpan &timeSpan) const;
	const WTime& operator-=(const WTimeSpan &timeSpan);
	const WTime& operator+=(const WTimeSpan &timeSpan);
	const WTime& operator--();					// decrement by a year
	const WTime& operator++();					// increment by a year
	const WTime& operator-=(INTNM::int32_t years);
	const WTime& operator+=(INTNM::int32_t years);
	WTimeSpan operator-(const WTime& time) const;
	bool operator==(const WTime &time) const;
	bool operator!=(const WTime &time) const;
	bool operator<(const WTime &time) const;
	bool operator>(const WTime &time) const;
	bool operator<=(const WTime &time) const;
	bool operator>=(const WTime &time) const;

	std::string ToString(INTNM::uint32_t flags) const;
#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	bool ParseDateTime(const CString &lpszDate, INTNM::uint32_t flags/* for timezone, DST */);
#endif
	bool ParseDateTime(const std::string &lpszDate, INTNM::uint32_t flags/* for timezone, DST */, WorldLocation* location = nullptr);
	bool ParseDateTime(const std::wstring &lpszDate, INTNM::uint32_t flags/* for timezone, DST */);
									// match_str
#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	friend TIMES_API CArchive& AFXAPI operator<<(CArchive& ar, const WTime &time);
	friend TIMES_API CArchive& AFXAPI operator>>(CArchive& ar, WTime &rtime);

	COleDateTime AsCOleDateTime(INTNM::uint32_t flags) const;
#endif

	static INTNM::uint64_t updateSerializedULONGLONG(INTNM::uint64_t toUpdate);
};

};


#ifdef MSVC_COMPILER
#pragma managed(pop)
#endif

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)			// undo our packing rules to what they were before
#endif
