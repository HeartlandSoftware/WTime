/**
 * WTime.java
 *
 * Copyright 2016-2018 Heartland Software Solutions Inc.
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


package ca.hss.times;

import ca.hss.general.OutVariable;

import ca.hss.text.StringExtensions;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.*;

import ca.hss.annotations.Source;

/**
 * A class for storing time information. I can handle timezones
 * with or without DST as defined in WorldLocation.java. It can
 * parse dates from strings as well as convert the to strings
 * in a provided format.
 */
@Source(project = "Times", sourceFile = "Times.cpp")
public class WTime implements Serializable, Comparable<WTime> {
	public static final long serialVersionUID = 1;

	/**
	 * Format with the time.
	 */
	public static final long FORMAT_TIME				= 0x00100000;
	/**
	 * Format with the day of the month.
	 */
	public static final long FORMAT_DAY					= 0x00200000;
	/**
	 * Format with the year. FORMAT_MONTH and FORMAT_DATE are
	 * also required for this to take effect.
	 */
	public static final long FORMAT_YEAR				= 0x00800000;
	/**
	 * Exclude the seconds when formatting times.
	 */
	public static final long FORMAT_EXCLUDE_SECONDS		= 0x20000000;
	/**
	 * Include the microseconds when formatting times.
	 */
	public static final long FORMAT_INCLUDE_USECS		= 0x00080000;
	/**
	 * If there is a day but no hours, minutes, or seconds,
	 * then don't print the hours, minutes, and seconds.
	 */
	public static final long FORMAT_CONDITIONAL_TIME	= 0x40000000;
	/**
	 * Include the month when formatting dates.
	 */
	public static final long FORMAT_MONTH				= 0x00400000;
	/**
	 * Format both the day and the month.
	 */
	public static final long FORMAT_DATE				= (FORMAT_DAY | FORMAT_MONTH);
	/**
	 * Format the date as local time. If this is omitted the time
	 * will be formatted in UTC.
	 */
	public static final long FORMAT_AS_LOCAL			= 0x01000000;
	/**
	 * Format the date as solar time.
	 */
	public static final long FORMAT_AS_SOLAR			= 0x02000000;
	/**
	 * Format the date with the day of week.
	 */
	public static final long FORMAT_DAY_OF_WEEK			= 0x00000100;
	/**
	 * Abbreviate the month.
	 */
	public static final long FORMAT_ABBREV				= 0x00001000;
	/**
	 * Format the time with the DST offset.
	 */
	public static final long FORMAT_WITHDST				= 0x04000000;
	/**
	 * Not currently implemented.
	 */
	public static final long FORMAT_PARSE_USING_SYSTEM	= 0x80000000;
	/**
	 * Format the date as DD MM YYYY.
	 */
	public static final long FORMAT_STRING_DD_MM_YYYY	= 0x00000001;
	/**
	 * Format the date as YYYY MM DD.
	 */
	public static final long FORMAT_STRING_YYYY_MM_DD	= 0x00000002;
	/**
	 * Format the date as MM DD YYYY.
	 */
	public static final long FORMAT_STRING_MM_DD_YYYY	= 0x00000003;
	/**
	 * Format the date with delimiters between the values in the order DD MM YYYY.
	 */
	public static final long FORMAT_STRING_DDhMMhYYYY	= 0x00000004;
	/**
	 * Format the date with delimiters between the values in the order YYYY MM DD.
	 */
	public static final long FORMAT_STRING_YYYYhMMhDD	= 0x00000005;
	/**
	 * Format the date with delimiters between the values in the order MM DD YYYY.
	 */
	public static final long FORMAT_STRING_MMhDDhYYYY	= 0x00000006;
	/**
	 * Format the date as YYYYMMDD.
	 */
	public static final long FORMAT_STRING_YYYYMMDD		= 0x00000007;
	/**
	 * Format the date as YYYYMMDDHH.
	 */
	public static final long FORMAT_STRING_YYYYMMDDHH	= 0x00000008;
	/**
	 * Format the date with a T at the end in the order YYYY MM DD.
	 */
	public static final long FORMAT_STRING_YYYYMMDDT	= 0x00000010;
	/**
	 * Format the date with a T at the end and delimiters between the values
	 * in the order YYYY MM DD.
	 */
	public static final long FORMAT_STRING_YYYYhMMhDDT	= 0x00000020;
	/**
	 * Add the timezone to the end of the time string.
	 */
	public static final long FORMAT_STRING_TIMEZONE		= 0x00000200;
	/**
	 * Format the time so it is compatible with ISO8601.
	 */
	public static final long FORMAT_STRING_ISO8601		= (FORMAT_STRING_TIMEZONE | FORMAT_STRING_YYYYhMMhDDT | FORMAT_DATE | FORMAT_TIME | FORMAT_AS_LOCAL | FORMAT_WITHDST);

	/**
	 * Abbreviations for the months.
	 */
	public static final String[] months_abbrev = {" Jan", " Feb", " Mar", " Apr", " May", "Jun", "Jul", " Aug", "Sep", " Oct", " Nov", " Dec"};
	/**
	 * The names of the months.
	 */
	public static final String[] months = {"January", "February", "March", "April", "May", "June", "July", "August", "September",	"October", "November", "December"};
	/**
	 * Abbreviations for the days of the week.
	 */
	public static final String[] days_abbrev = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	/**
	 * The names of the days of the week.
	 */
	public static final String[] days = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

	private long m_time;
	private WTimeManager	m_tm;	// used so we can get the timezone offset

	private void construct_time_t(long nYear, long nMonth, long nDay, long nHour, long nMin, long nSec) {
		if (nMonth <= 2) {
			nYear -= 1;
			nMonth += 12;
		}
		long A = nYear / 100L;
		long B = 2L - A + A / 4;
		m_time = (long)(Math.floor(365.25 * (nYear + 4716)) + Math.floor(30.6001 * (nMonth + 1)) + (nDay + 1) + B - 1524.5);
		m_time -= 2305448;
		m_time *= 24;
		m_time += nHour;
		m_time *= 60;
		m_time += nMin;
		m_time *= 60;
		m_time += nSec;                                                 // m_time is # secs since Jan 1, 1600
		m_time *= 1000000;
	}

	private long adjusted_tm(long flags) {
		if (m_time == 0 || m_time == -1L)
			return m_time;
		if (flags == 0)
			return m_time;

		return adjusted_tm_math(flags);
	}

	private long adjusted_tm_math(long flags) {
		if ((flags & FORMAT_AS_SOLAR) != 0)
			if ((flags & (FORMAT_WITHDST | FORMAT_AS_LOCAL)) != 0)
				throw new IllegalArgumentException("One of the arguments is illegal.");
		long time;
		if (m_tm != null) {
			if((flags & FORMAT_AS_LOCAL) != 0)
				time = m_time + m_tm.getWorldLocation().m_TimeZone.getTotalMicroSeconds();
			else if ((flags & FORMAT_AS_SOLAR) != 0)
				time = m_time + m_tm.getWorldLocation().m_solar_timezone(this).getTotalMicroSeconds();
			else	time = m_time;

			if (((flags & FORMAT_WITHDST)) != 0 && (WTimeSpan.notEqual(m_tm.getWorldLocation().m_StartDST, m_tm.getWorldLocation().m_EndDST))) {
				WTime t = new WTime(time, null, false);
				long secs = t.getSecondsIntoYear((short)0);
				if (m_tm.getWorldLocation().m_EndDST.compareTo(m_tm.getWorldLocation().m_StartDST) == 1) {
					if (((long)m_tm.getWorldLocation().m_StartDST.getTotalSeconds() <= secs) &&
					    (secs < (long)m_tm.getWorldLocation().m_EndDST.getTotalSeconds()))
						time += m_tm.getWorldLocation().m_AmtDST.getTotalMicroSeconds();
				} else {
					if (((long)m_tm.getWorldLocation().m_StartDST.getTotalSeconds() < secs) ||
					    (secs <= (long)m_tm.getWorldLocation().m_EndDST.getTotalSeconds()))
						time += m_tm.getWorldLocation().m_AmtDST.getTotalMicroSeconds();
				}
			}
		} else	time = m_time;
		return time;
	}

	/**
	 * Construct a new WTime.
	 * @param tm the time manager that describes how the time is to be interpreted
	 */
	public WTime(WTimeManager tm) {
		m_tm = tm;
	}

	/**
	 * Construct a WTime with a given UTC offset in seconds.
	 * @param time the UTC offset in seconds
	 * @param tm the time manager that describes how the time is to be interpreted
	 */
	public WTime(long time, WTimeManager tm) {
		this(time, tm, true);
	}

	/**
	 * Construct a WTime with either a second or microsecond offset from UTC.
	 * @param time the offset from UTC
	 * @param tm the time manager that describes how the time is to be interpreted
	 * @param units_are_seconds whether the offset is in seconds or microseconds
	 */
	public WTime(long time, WTimeManager tm, boolean units_are_seconds) {
		if (units_are_seconds && time != -1L)
			m_time = time * 1000000L;
		else
			m_time = time;
		m_tm = tm;
	}

	/**
	 * Construct a WTime with a given set of values all in UTC
	 * @param nYear the year
	 * @param nMonth the month
	 * @param nDay the day of the month
	 * @param nHour the hour of the day
	 * @param nMin the minutes
	 * @param nSec the seconds
	 * @param tm the time manager that describes how the time is to be interpreted
	 */
	public WTime(long nYear, long nMonth, long nDay, long nHour, long nMin, long nSec, WTimeManager tm) {
		construct_time_t(nYear, nMonth, nDay, nHour, nMin, nSec);
		m_tm = tm;
	}

	/**
	 * Construct a WTime with a given set of values all in UTC
	 * @param nYear the year
	 * @param nMonth the month
	 * @param nDay the day of the month
	 * @param nHour the hour of the day
	 * @param nMin the minutes
	 * @param nSec the seconds
	 * @param uSec the microseconds
	 * @param tm the time manager that describes how the time is to be interpreted
	 */
	public WTime(long nYear, long nMonth, long nDay, long nHour, long nMin, long nSec, long uSec, WTimeManager tm) {
		construct_time_t(nYear, nMonth, nDay, nHour, nMin, nSec);
		m_time += uSec;
		m_tm = tm;
	}

	/**
	 * Copy constructor.
	 * @param timeSrc The time to copy
	 */
	public WTime(WTime timeSrc) {
		m_time = timeSrc.m_time;
		m_tm = timeSrc.m_tm;
	}

	/**
	 * Copy a time either to or from UTC.
	 * @param timeSrc the time to copy
	 * @param flags
	 * @param direction positive means from UTC, negative means to UTC
	 */
	public WTime(WTime timeSrc, long flags, short direction) {
		m_time = timeSrc.m_time;
		m_tm = timeSrc.m_tm;
		if (m_tm != null) {
			long atm = adjusted_tm(flags);
			if (direction < 0)		m_time = m_time - (atm - m_time);
			else if (direction > 0)		m_time = atm;
		}
	}

	/**
	 * Construct a WTime from local time (the values are in the same timezone as described in the manager).
	 * @param nYear the year
	 * @param nMonth the month
	 * @param nDay the day of the month
	 * @param nHour the hour of the day
	 * @param nMin the minutes
	 * @param nSec the seconds
	 * @param tm the time manager that describes how the time is to be interpreted
	 * @return a new WTime instance for the given date and time
	 */
	public static WTime fromLocal(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, WTimeManager tm) {
		nHour -= tm.getWorldLocation().m_TimeZone.getHours();
		if (!WTimeSpan.equal(tm.getWorldLocation().m_StartDST, tm.getWorldLocation().m_EndDST))
			nHour -= tm.getWorldLocation().m_AmtDST.getHours();
		if (nHour < 0) {
			nHour += 24;
			nDay -= 1;
			if (nDay < 1) {
				nMonth -= 1;
				if (nMonth < 1) {
					nMonth = 11;
					nYear -= 1;
				}
				nDay = WTimeManager.daysInMonth((short)nMonth, (short)nYear);
			}
		}
		else if (nHour > 23) {
			nHour -= 24;
			nDay += 1;
			int tempday = WTimeManager.daysInMonth((short)nMonth, (short)nYear);
			if (nDay > tempday) {
				nDay = 1;
				nMonth++;
				if (nMonth > 12) {
					nMonth = 0;
					nYear++;
				}
			}
		}
		return new WTime(nYear, nMonth, nDay, nHour, nMin, nSec, tm);
	}
	
	public WTime Now(WTimeManager tm, long flags) {
		Calendar t = Calendar.getInstance();
		int seconds = t.get(Calendar.SECOND);
	    if (flags == FORMAT_EXCLUDE_SECONDS)
	        seconds = 0;
	    WTime temp = fromLocal(t.get(Calendar.YEAR), t.get(Calendar.MONTH), t.get(Calendar.DAY_OF_MONTH), t.get(Calendar.HOUR), t.get(Calendar.MINUTE), seconds, tm);
	    return new WTime(temp, flags, (short)-1);
	}

	/**
	 * Get the second offset with the given formatting parameters.
	 * @param mode
	 * @return
	 */
	public long getTime(long mode) {
		if(m_time == -1L)
			return -1L;
		
		return adjusted_tm(mode) / 1000000L;
	}

	/**
	 * Get the total number of seconds stored.
	 * @return
	 */
	public long getTotalSeconds() {
		if(m_time == -1L)
			return -1L;
		
		return m_time / 1000000L;
	}

	/**
	 * Get the total number of microseconds stored.
	 * @return
	 */
	public long getTotalMicroSeconds() {
		if(m_time == -1L)
			return -1L;
		
		return m_time;
	}

	/**
	 * Get the associated time manager.
	 * @return
	 */
	public WTimeManager getTimeManager() {
		return m_tm;
	}

	/**
	 * Set the time manager, not recommended.
	 * @param tm the new time manager
	 * @return
	 */
	public WTimeManager setTimeManager(WTimeManager tm) {
		m_tm = tm;
		return m_tm;
	}

	/**
	 * Get the hour with the given formatting flags.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getHour(long flags) {
		if(m_time == -1L)
			return -1L;
		
		return (long)((adjusted_tm(flags) / (60L * 60L * 1000000L)) % 24);
	}

	/**
	 * Get the minutes with the given formatting flags.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getMinute(long flags) {
		if(m_time == -1L)
			return -1L;
		
		return (long)((adjusted_tm(flags) / (60L * 1000000L)) % 60);
	}

	/**
	 * Get the seconds with the given formatting flags.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getSecond(long flags) {
		if(m_time == -1L)
			return -1L;
		
		return (long)((adjusted_tm(flags) / 1000000L) % 60);
	}

	/**
	 * Get the microseconds with the given formatting flags.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getMicroSeconds(long flags) {
		if(m_time == -1L)
			return -1L;
		
		return (long)((adjusted_tm(flags) % 1000000L));
	}

	/**
	 * Get the time offset into the day.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public WTimeSpan getTimeOfDay(long flags) {
		if(m_time == -1L)
			return new WTimeSpan(-1L, false);
		
		long time = adjusted_tm(flags) % (60L * 60L * 24L * 1000000L);
		return new WTimeSpan(time, false);
	}

	/**
	 * Get the day of the week.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getDayOfWeek(long flags) {
		if(m_time == -1L)
			return -1L;
		
		long days = adjusted_tm(flags) / (24L * 60L * 60L * 1000000L) + 0;
		long ret = (long)(days % 7);
		return ret == 0 ? 7 : ret;
	}

	/**
	 * Get the day of the year.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getDayOfYear(long flags) {
		if(m_time == -1L)
			return -1L;
		
		long atm = adjusted_tm(flags) / 24 / 60 / 60 / 1000000;
		WTime year = new WTime(getYear(flags), 1, 1, 0, 0, 0, m_tm);
		long year_atm = year.adjusted_tm(0) / 24 / 60 / 60 / 1000000;
		return (atm - year_atm) + 1;
	}

	/**
	 * Get the number of seconds into the year.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getSecondsIntoYear(long flags) {
		if(m_time == -1L)
			return -1L;
		
		long atm = adjusted_tm(flags) / 1000000;
		WTime year = new WTime(getYear(flags), 1, 1, 0, 0, 0, m_tm);
		return (atm - year.getTotalSeconds());
	}

	/**
	 * Get the fraction of days into the year.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public double getDayFractionOfYear(long flags) {
		if(m_time == -1L)
			return -1L;
		
		long atm = adjusted_tm(flags) / 1000000;
		WTime year = new WTime(getYear(flags), 1, 1, 0, 0, 0, m_tm);
		long total_secs = atm - year.getTotalSeconds();
		return ((double)total_secs) / (24.0 * 60.0 * 60.0) + 1.0;
	}

	/**
	 * Is the stored year a leap year.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public boolean isLeapYear(long flags) {
		if(m_time == -1L)
			return false;
		
		return WTimeManager.isLeapYear((short)getYear(flags));
	}

	/**
	 * Purge all time steps below seconds.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 */
	public void purgeToSecond(long flags) {
		if(m_time != -1L)
			m_time = m_time - (adjusted_tm(flags) % (1000000L));
	}

	/**
	 * Purge all time steps below minutes.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 */
	public void purgeToMinute(long flags) {
		if(m_time != -1L)
			m_time = m_time - (adjusted_tm(flags) % (60L * 1000000L));
	}

	/**
	 * Purge all time steps below hours.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 */
	public void purgeToHour(long flags) {
		if(m_time != -1L)
			m_time = m_time - (adjusted_tm(flags) % (60L * 60L * 1000000L));
	}

	/**
	 * Purge all time steps below days.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 */
	public void purgeToDay(long flags) {
		if(m_time != -1L)
			m_time = m_time - (adjusted_tm(flags) % (60L * 60L * 24L * 1000000L));
	}	

	/**
	 * Get the year.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getYear(long flags) {
		long z = adjusted_tm(flags) / 24 / 60 / 60 / 1000000;
		z += 2305448;

		long a = z + 32044;
		long b = (4*a+3)/146097;
		long c = a - (b*146097)/4;
		long d = (4*c+3)/1461;
		long e = c - (1461*d)/4;
		long m = (5*e+2)/153;
		long nyear = b*100 + d - 4800 + m/10;
		return nyear;
	}

	/**
	 * Get the month.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getMonth(long flags) {       // month of year (1 = Jan)
		long z = adjusted_tm(flags) / 24 / 60 / 60 / 1000000;
		z += 2305448;
		long a = z + 32044;
		long b = (4*a+3)/146097;
		long c = a - (b*146097)/4;
		long d = (4*c+3)/1461;
		long e = c - (1461*d)/4;
		long m = (5*e+2)/153;
		long nmonth = m + 3 - 12*(m/10);
		return nmonth;
	}

	/**
	 * Get the day of the month.
	 * @param flags potential flags are FORMAT_AS_LOCAL or FORMAT_WITHDST
	 * @return
	 */
	public long getDay(long flags) {
		long z = adjusted_tm(flags) / 24 / 60 / 60 / 1000000;
		z += 2305448;

		long a = z + 32044;
		long b = (4*a+3)/146097;
		long c = a - (b*146097)/4;
		long d = (4*c+3)/1461;
		long e = c - (1461*d)/4;
		long m = (5*e+2)/153;
		long nday = e - (153*m+2)/5 + 1;
		return nday;
	}

	/**
	 * Get a string representation of the time.
	 * @param flags any of the FORMAT_ flags
	 * @return
	 */
	public String toString(long flags) {
		String str = "";
		String month_str, day_str;
		int year, month, day, hour, minute, second, usecs, day_of_week;
		boolean need_leading_space;

		if ((m_time == 0) || (m_time == -1L)) {
			str = "[Time Not Set]";
			return str;
		}

		year = (int)getYear(flags);
		month = (int)getMonth(flags);
		day = (int)getDay(flags);
		hour = (int)getHour(flags);
		minute = (int)getMinute(flags);
		second = (int)getSecond(flags);
		usecs = (int)getMicroSeconds(flags);

		assert second >= 0 && second < 60;
		assert minute >= 0 && minute < 60;
		assert hour >= 0 && hour < 24;

		day_of_week = ((int)getDayOfWeek(flags));

		// decide if we should be abbreviating things or not
		if ((flags & FORMAT_ABBREV) == FORMAT_ABBREV) {
			month_str = months_abbrev[month - 1];
			day_str = days_abbrev[--day_of_week];
		}
		else {
			month_str = months[month - 1];
			day_str = days[--day_of_week];
		}

		// first, try to prepend the day-of-week to the string
		if ((flags & FORMAT_DAY_OF_WEEK) == FORMAT_DAY_OF_WEEK) {
			str = day_str;
			need_leading_space = true;
		}
		else
			need_leading_space = false;

		if ((flags & FORMAT_DATE) == FORMAT_DATE) {
			if (need_leading_space)
				str += " ";
			else
				need_leading_space = true;

			if ((flags & 0xf) == FORMAT_STRING_DD_MM_YYYY) {
				str += String.format("%02d/%02d/%04d", day, month, year);
			}
			else if ((flags & FORMAT_STRING_YYYY_MM_DD) == FORMAT_STRING_YYYY_MM_DD) {
				str += String.format("%04d/%02d/%02d", year, month, day);
			}
			else if ((flags & FORMAT_STRING_MM_DD_YYYY) ==  FORMAT_STRING_MM_DD_YYYY) {
				str += String.format("%02d/%02d/%04d", month, day, year);
			}
			else if ((flags & FORMAT_STRING_DDhMMhYYYY) == FORMAT_STRING_DDhMMhYYYY) {
				str += String.format("%02d-%02d-%04d", day, month,year);
			}
			else if (((flags & FORMAT_STRING_YYYYhMMhDD) == FORMAT_STRING_YYYYhMMhDD) || ((flags & 0xff) == FORMAT_STRING_YYYYhMMhDDT)) {
				str += String.format("%04d-%02d-%02d", year, month, day);
			}
			else if ((flags & FORMAT_STRING_MMhDDhYYYY) == FORMAT_STRING_MMhDDhYYYY) {
				str += String.format("%02d-%02d-%04d", month, day, year);
			}
			else if (((flags & FORMAT_STRING_YYYYMMDD) == FORMAT_STRING_YYYYMMDD) || ((flags & 0xff) == FORMAT_STRING_YYYYMMDDT)) {
				str += String.format("%04d%02d%02d", year, month, day);
			}
			else if ((flags & FORMAT_STRING_YYYYMMDDHH) == FORMAT_STRING_YYYYMMDDHH) {
				str += String.format("%04d%02d%02d%02d", year, month, day, hour);
			}
			else if ((flags & FORMAT_STRING_YYYYMMDDHH) == FORMAT_STRING_YYYYMMDDHH) {
				str += String.format("%04d%02d%02d%02d", year, month, day, hour);
			}
			else if ((flags & FORMAT_MONTH) == FORMAT_MONTH) {	// next, try to append date as month, day
				String tmp;
				if ((flags & FORMAT_DAY) == FORMAT_DAY)
				{
					tmp = String.format("%s %2d", month_str, day);
				}
				else
					tmp = String.format("%s", month_str);
				str += tmp;

				if ((flags & FORMAT_YEAR) == FORMAT_YEAR) {	// ...and conditionally add in the year
					str += String.format(", %d", year);
				}
			} else if ((flags & FORMAT_DAY) == FORMAT_DAY) {
				str += String.format("%2d", day);
			}
		}

		if (((flags & FORMAT_TIME) == FORMAT_TIME) || (((flags & FORMAT_CONDITIONAL_TIME) == FORMAT_CONDITIONAL_TIME) &&
				((usecs > 0) || (second > 0) || (minute > 0) || (hour > 0)))) {			// finally, try to append the time
			if ((((flags & 0x000000ff) == FORMAT_STRING_YYYYhMMhDDT) || ((flags & 0x000000ff) == FORMAT_STRING_YYYYMMDDT)) &&
					((flags & FORMAT_DATE) == FORMAT_DATE))
				str += "T";
			else if (need_leading_space)
				str += " ";
			String tmp;
			if ((flags & FORMAT_EXCLUDE_SECONDS) == FORMAT_EXCLUDE_SECONDS) {
				if (second >= 30)		// perform any rounding
					minute++;
				tmp = String.format("%02d:%02d", hour, minute);
			}
			else {
				if ((flags & FORMAT_INCLUDE_USECS) == FORMAT_INCLUDE_USECS)
					tmp = String.format("%02d:%02d:%02d.%06d", hour, minute, second, usecs);
				else
					tmp = String.format("%02d:%02d:%02d", hour, minute, second);
			}
			str += tmp;
		}
		
		if ((flags & FORMAT_STRING_TIMEZONE) != 0) {
			long offset = m_tm.getWorldLocation().m_TimeZone.getTotalMinutes();
			long intoYear = getSecondsIntoYear(0);
			if (WTimeSpan.notEqual(m_tm.getWorldLocation().m_EndDST, m_tm.getWorldLocation().m_StartDST)) {
				if (WTimeSpan.lessThan(m_tm.getWorldLocation().m_StartDST, m_tm.getWorldLocation().m_EndDST)) {
					if ((m_tm.getWorldLocation().m_StartDST.getTotalSeconds() <= intoYear) &&
						(intoYear < m_tm.getWorldLocation().m_EndDST.getTotalSeconds()))
						offset += m_tm.getWorldLocation().m_AmtDST.getTotalMinutes();
				}
				else if ((m_tm.getWorldLocation().m_StartDST.getTotalSeconds() < intoYear) ||
						 (intoYear <= m_tm.getWorldLocation().m_EndDST.getTotalSeconds()))
					offset += m_tm.getWorldLocation().m_AmtDST.getTotalMinutes();
			}
			
			if (offset == 0)
				str += "Z";
			else {
				if (offset < 0) {
					str += "-";
					offset = -offset;
				}
				else
					str += "+";
				int hourOffset = 0;
				while (offset >= 60) {
					hourOffset++;
					offset -= 60;
				}
				String tmp = String.format("%02d:%02d", hourOffset, offset);
				str += tmp;
			}
		}
		return str;
	}

	/**
	 * Convert to a string with a set of standard formatting flags. Those
	 * flags are FORMAT_TIME, FORMAT_AS_LOCAL, FORMAT_DATE, FORMAT_STRING_DD_MM_YYYY, and
	 * FORMAT_WITHDST.
	 */
	@Override
	public String toString() {
		return toString(FORMAT_TIME | FORMAT_AS_LOCAL | FORMAT_DATE | FORMAT_STRING_DD_MM_YYYY | FORMAT_WITHDST);
	}
	
	/**
	 * Convert a WTime value to a Calendar.
	 * @param flags The FORMAT flags to use in the conversion.
	 * @return A new Calendar instance set to the same date and time as the WTime object.
	 */
	public Calendar toCalendar(long flags) {
		Calendar cal = Calendar.getInstance();
		cal.set(Calendar.YEAR, (int)getYear(flags));
		cal.set(Calendar.MONTH, (int)getMonth(flags));
		cal.set(Calendar.DAY_OF_MONTH, (int)getDay(flags));
		cal.set(Calendar.HOUR_OF_DAY, (int)getHour(flags));
		cal.set(Calendar.MINUTE, (int)getMinute(flags));
		cal.set(Calendar.SECOND, (int)getSecond(flags));
		return cal;
	}

	/**
	 * Parse a date from a string.
	 * @param lpszDate the string to parse
	 * @param flags FORMAT_ flags that describe the layout of the string
	 * @return
	 */
	public boolean parseDateTime(String lpszDate, long flags) {
		String d = lpszDate;
		d = d.trim();
		if ((flags & FORMAT_PARSE_USING_SYSTEM) != 0)
			return false;

		final String delimit = "./\\:;-, \t";
		final String delimit2 = "./\\:;-, \tT";
		String dateBuf = lpszDate;
		long year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
		long v1 = 0, v2 = 0, v3 = 0;
		short time_scan = 0;
		long secondOffset = 0;

		boolean delimit_found = false;
		while (delimit.charAt(time_scan) != '\0') {
			if (dateBuf.contains(String.valueOf(delimit.charAt(time_scan)))) {
				delimit_found = true;
				break;
			}
			time_scan++;
		}
		// TIME_FORMAT_DAY is implied

		if (!delimit_found) {
				// some groups are providing a screwy "YYYYMMDDHH" format
			if (((flags & 0x00ff) != FORMAT_STRING_YYYYMMDD) &&
				((flags & 0x00ff) != FORMAT_STRING_YYYYMMDDT) &&
				((flags & 0x00ff) != FORMAT_STRING_YYYYMMDDHH))
				return false;
			if ((flags & 0x000f) == FORMAT_STRING_YYYYMMDD) {
				if (dateBuf.length() != 8)
					return false;
			}
			else if ((flags & 0x000f) == FORMAT_STRING_YYYYMMDDHH) {
				if (dateBuf.length() != 10)
					return false;
			}
			String yr, mn, dy, hr;
			//char yr[8], mn[4], dy[4], hr[4];
			yr = dateBuf.substring(0, 4);
			mn = dateBuf.substring(4, 6);
			dy = dateBuf.substring(6, 8);
			hr = "";
			if ((flags & 0x00ff) == FORMAT_STRING_YYYYMMDDHH)
				hr = dateBuf.substring(8, 10);
			List<OutVariable<Object>> l = new ArrayList<OutVariable<Object>>();
			OutVariable<Object> v = new OutVariable<Object>();
			l.add(v);
			int rd = StringExtensions.sscanf(yr, "%ld", l);
			if (rd != 1)
				return false;
			year = (Long)v.value;
			rd = StringExtensions.sscanf(mn, "%ld", l);
			if (rd != 1)
				return false;
			month = (Long)v.value;
			rd = StringExtensions.sscanf(dy, "%ld", l);
			if (rd != 1)
				return false;
			day = (Long)v.value;
			if ((flags & 0x00ff) == FORMAT_STRING_YYYYMMDDHH) {
				rd = StringExtensions.sscanf(hr, "%ld", l);
				if (rd != 1)
					return false;
				hour = (Long)v.value;
			}
			min = sec = 0;
		} else {
			OutVariable<StringTokenizer> context = new OutVariable<>();
			String tok;

			tok = StringExtensions.strtok_s(dateBuf, delimit, context);
			if (tok == null)
				return false;
			short guess_month = 0;
			if (!Character.isDigit(tok.charAt(0))) {
				if (((flags & 0x00ff) != FORMAT_STRING_MM_DD_YYYY) &&
					((flags & 0x00ff) != FORMAT_STRING_MMhDDhYYYY) &&
					((flags & 0x00ff) != 0))
					return false;
				for (v1 = 0; v1 < 12; v1++)
					if (months[(int)v1].compareToIgnoreCase(tok) == 0 || months_abbrev[(int)v1].compareToIgnoreCase(tok) == 0)
						break;
				if (v1 == 12)
					return false;
				v1++;
				guess_month = 1;
			}
			else {
				List<OutVariable<Object>> l = new ArrayList<OutVariable<Object>>();
				OutVariable<Object> v = new OutVariable<Object>();
				l.add(v);
				int rd = StringExtensions.sscanf(tok, "%ld", l);
				if (rd != 1)
					return false;
				v1 = (Long)v.value;
			}

			tok = StringExtensions.strtok_s(null, delimit, context);
			if (tok == null)
				return false;
			if (!Character.isDigit(tok.charAt(0))) {
				if (((flags & 0x00ff) != FORMAT_STRING_DD_MM_YYYY) &&
					((flags & 0x00ff) != FORMAT_STRING_YYYY_MM_DD) &&
					((flags & 0x00ff) != FORMAT_STRING_DDhMMhYYYY) &&
					((flags & 0x00ff) != FORMAT_STRING_YYYYhMMhDD) &&
					((flags & 0x00ff) != FORMAT_STRING_YYYYhMMhDDT) &&
					((flags & 0x00ff) != 0))
					return false;
				for (v2 = 0; v2 < 12; v2++)
					if (months[(int)v2].compareToIgnoreCase(tok) == 0 || months_abbrev[(int)v2].compareToIgnoreCase(tok) == 0)
						break;
				if (v2 == 12)
					return false;
				v2++;
				guess_month = 2;
			}
			else {
				List<OutVariable<Object>> l = new ArrayList<OutVariable<Object>>();
				OutVariable<Object> v = new OutVariable<Object>();
				l.add(v);
				int rd = StringExtensions.sscanf(tok, "%ld", l);
				if (rd != 1)
					return false;
				v2 = (Long)v.value;
			}

			if ((flags & 0x00ff) == FORMAT_STRING_YYYYhMMhDDT)
				tok = StringExtensions.strtok_s(null, delimit2, context);
			else
				tok = StringExtensions.strtok_s(null, delimit, context);

			if (tok == null)
				return false;
			if (!Character.isDigit(tok.charAt(0)))
				return false;
			else {
				List<OutVariable<Object>> l = new ArrayList<OutVariable<Object>>();
				OutVariable<Object> v = new OutVariable<Object>();
				l.add(v);
				int rd = StringExtensions.sscanf(tok, "%ld", l);
				if (rd != 1)
					return false;
				v3 = (Long)v.value;
			}
			
			if ((flags & 0x00ff) == 0) {
				if (guess_month == 1) {
					if (v3 >= 32)
						flags |= FORMAT_STRING_MM_DD_YYYY;
				}
				else if (guess_month == 2) {
					if (v3 >= 32)
						flags |= FORMAT_STRING_DD_MM_YYYY;
					else if (v1 >= 32)
						flags |= FORMAT_STRING_YYYY_MM_DD;
				}
				else {
					if (v1 >= 32)
						flags |= FORMAT_STRING_YYYY_MM_DD;
					else if (v3 >= 32 && v2 > 12) {
						flags |= FORMAT_STRING_MM_DD_YYYY;
					}
				}
			}

			if (((flags & 0x00ff) == FORMAT_STRING_DD_MM_YYYY) ||
				((flags & 0x00ff) == FORMAT_STRING_DDhMMhYYYY) ||
				((flags & 0x00ff) == 0)) {
				day = v1;
				month = v2;
				year = v3;
			}
			else if (((flags & 0x00ff) == FORMAT_STRING_YYYY_MM_DD) ||
					 ((flags & 0x00ff) == FORMAT_STRING_YYYYhMMhDD) ||
					 ((flags & 0x00ff) == FORMAT_STRING_YYYYhMMhDDT)) {
				day = v3;
				month = v2;
				year = v1;
			}
			else if (((flags & 0x00ff) == FORMAT_STRING_MM_DD_YYYY) ||
					 ((flags & 0x00ff) == FORMAT_STRING_MMhDDhYYYY)) {
				day = v2;
				month = v1;
				year = v3;
			}
			else {
				assert false;
			}

			if (day >= 32)
				return false;
			if ((month < 1) || (month > 12))
				return false;
			if (year < 39)
				year += 2000;
			else if ((year < 100) && (year > 60))
				year += 1200;

			if ((flags & FORMAT_TIME) == FORMAT_TIME) {
				tok = StringExtensions.strtok_s(null, "", context);
				//no time is specified
				if (tok != null) {
					if (tok.startsWith("T"))
						tok = tok.substring(1);
					String timezone = null;
					boolean negative = false;
					boolean timezoneExists = tok.indexOf('+') >= 0 || tok.indexOf('-') >= 0;
					if (timezoneExists) {
						negative = tok.indexOf('-') >= 0;
						String[] split = tok.split("[-\\+Z]");
						if (split.length == 2) {
							tok = split[0];
							timezone = split[1];
						}
					}
					OutVariable<Short> s = new OutVariable<Short>();
					WTimeSpan ts = new WTimeSpan(tok, s);
					time_scan = s.value;
					if (time_scan == 0) {
						hour = min = sec = 0;
					}
					else {
						hour = ts.getHours();
						min = ts.getMinutes();
						sec = ts.getSeconds();
					}
					
					if (timezone != null) {
						ts = new WTimeSpan(timezone, s);
						time_scan = s.value;
						if (time_scan != 0) {
							secondOffset = ts.getTotalSeconds();
							if (negative)
								secondOffset = -secondOffset;
						}
					}
				}
			}
			else {
				hour = min = sec = 0;
			}
		}

		if ((year >= 1600) && (year < 2900)) {
			WTime t = new WTime(year, month, day, hour, min, sec, m_tm);
			
			if (secondOffset != 0) {
				long offset = m_tm.getWorldLocation().m_TimeZone.getTotalSeconds();
				long intoYear = getSecondsIntoYear(0);
				if (WTimeSpan.notEqual(m_tm.getWorldLocation().m_EndDST, m_tm.getWorldLocation().m_StartDST)) {
					if (WTimeSpan.lessThan(m_tm.getWorldLocation().m_StartDST, m_tm.getWorldLocation().m_EndDST)) {
						if ((m_tm.getWorldLocation().m_StartDST.getTotalSeconds() <= intoYear) &&
							(intoYear < m_tm.getWorldLocation().m_EndDST.getTotalSeconds()))
							offset += m_tm.getWorldLocation().m_AmtDST.getTotalMinutes();
					}
					else if ((m_tm.getWorldLocation().m_StartDST.getTotalSeconds() < intoYear) ||
							 (intoYear <= m_tm.getWorldLocation().m_EndDST.getTotalSeconds()))
						offset += m_tm.getWorldLocation().m_AmtDST.getTotalMinutes();
				}
				offset -= secondOffset;
				if (offset != 0)
					t.add(new WTimeSpan(offset));
			}
			
			m_time = t.m_time;
			long atm = adjusted_tm(flags);
			m_time = m_time - (atm - m_time);
		}

		return true;
	}

	private void readObject(ObjectInputStream inStream) throws IOException {
		m_time = inStream.readLong();
	}

	private void writeObject(ObjectOutputStream outStream) throws IOException {
		outStream.writeLong(m_time);
	}

	/**
	 * Compare this time to another object.
	 * Unless the other object is also a
	 * WTime this is just a test for object
	 * equality.
	 */
	@Override
	public boolean equals(Object other) {
		if (other == null)
			return false;
		if (other instanceof WTime) {
			WTime othertime = (WTime)other;
			return equal(this, othertime);
		}
		return this == other;
	}

	/**
	 * Get a hash code to represent this time for
	 * doing things like storing it in a HashMap.
	 */
	@Override
	public int hashCode() {
		return (int)m_time;
	}

	/**
	 * +=
	 * @param s
	 */
	public WTime add(WTimeSpan s) {
		m_time += s.getTotalMicroSeconds();
		return this;
	}

	/**
	 * +
	 * @param t
	 * @param s
	 * @return
	 */
	public static WTime add(WTime t, WTimeSpan s) {
		return new WTime(t.m_time + s.getTotalMicroSeconds(), t.m_tm, false);
	}

	/**
	 * -=
	 * @param s
	 */
	public WTime subtract(WTimeSpan s) {
		m_time -= s.getTotalMicroSeconds();
		return this;
	}

	/**
	 * -
	 * @param t
	 * @param s
	 * @return
	 */
	public static WTime subtract(WTime t, WTimeSpan s) {
		return new WTime(t.m_time - s.getTotalMicroSeconds(), t.m_tm, false);
	}


	/**
	 * -
	 * @param t
	 * @param s
	 * @return
	 */
	public static WTimeSpan subtract(WTime t1, WTime t2) {
		return new WTimeSpan(t1.m_time - t2.m_time, false);
	}

	/**
	 * ==
	 * @param t1
	 * @param t2
	 * @return
	 */
	public static boolean equal(WTime t1, WTime t2) {
		return t1.m_time == t2.m_time;
	}

	/**
	 * !=
	 * @param t1
	 * @param t2
	 * @return
	 */
	public static boolean notEqual(WTime t1, WTime t2) {
		return t1.m_time != t2.m_time;
	}

	/**
	 * t1 < t2
	 * @param t1
	 * @param t2
	 * @return
	 */
	public static boolean lessThan(WTime t1, WTime t2) {
		return t1.m_time < t2.m_time;
	}

	/**
	 * t1 <= t2
	 * @param t1
	 * @param t2
	 * @return
	 */
	public static boolean lessThanEqualTo(WTime t1, WTime t2) {
		return t1.m_time <= t2.m_time;
	}

	/**
	 * t1 > t2
	 * @param t1
	 * @param t2
	 * @return
	 */
	public static boolean greaterThan(WTime t1, WTime t2) {
		return t1.m_time > t2.m_time;
	}

	/**
	 * t1 >= t2
	 * @param t1
	 * @param t2
	 * @return
	 */
	public static boolean greaterThanEqualTo(WTime t1, WTime t2) {
		return t1.m_time >= t2.m_time;
	}

	/**
	 * Compare this WTime to another.
	 */
	@Override
	public int compareTo(WTime o) {
		return (int)(getTotalSeconds() - o.getTotalSeconds());
	}

	/**
	 * A comparator that can be used for sorting times.
	 *
	 */
	public static class WTimeComparator implements Comparator<WTime> {
		@Override
		public int compare(WTime o1, WTime o2) {
			return o1.compareTo(o2);
		}
	}

	/*CString AsString(USHORT flags) const;
	BOOL ParseDateTime(const char *lpszDate, USHORT flags);
									// match_str

	friend CArchive& AFXAPI operator<<(CArchive& ar, WTime time)	{ return ar << time.m_time; };
	friend CArchive& AFXAPI operator>>(CArchive& ar, WTime& rtime){ return ar >> rtime.m_time; };*/
}
