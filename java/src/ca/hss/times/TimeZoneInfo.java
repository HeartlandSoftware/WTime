/**
 * TimeZoneInfo.java
 *
 * Copyright 2016 Heartland Software Solutions Inc.
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

import java.text.DecimalFormat;

import ca.hss.text.TranslationCallback;

/**
 * An immutable class that stores information about timezones. 
 */
public class TimeZoneInfo implements Comparable<TimeZoneInfo> {
	private WTimeSpan m_timezone;
	private WTimeSpan m_dst;
	private String m_code;
	private String m_name;
	private String m_id;
	
	/**
	 * Get the timezone offset from UTC.
	 * @return the timezone offset
	 */
	public WTimeSpan getTimezoneOffset() { return m_timezone; }
	/**
	 * Get the amount of offset from UTC caused by DST.
	 * @return the DST offset
	 */
	public WTimeSpan getDSTAmount() { return m_dst; }
	/**
	 * Get the timzones string code.
	 * @return the timezone code
	 */
	public String getCode() { return m_code; }
	/**
	 * Get the name of the timezone.
	 * @return the timezones name
	 */
	public String getName() { return m_name; }
	/**
	 * Get the unique ID of the timezone. Useful for translation.
	 * @return the timezones unique id
	 */
	public String getID() { return m_id; }

	TimeZoneInfo(WTimeSpan _timezone, WTimeSpan _dst, String _code, String _name, String _id) {
		m_timezone = _timezone;
		m_dst = _dst;
		m_code = _code;
		m_name = _name;
		m_id = _id;
	}

	/**
	 * Converts the time zone info into a string. If a translation callback instance has
	 * been set in ca.hss.text.TranslationCallback it will be used to translate the
	 * timezone name.
	 */
	@Override
	public String toString() {
		WTimeSpan WTimeSpan;
		WTimeSpan = m_timezone;
		String timeZoneCode = m_code;
		String timeZoneName = m_name;
		if (TranslationCallback.instance != null && m_id.length() > 0)
			timeZoneName = TranslationCallback.instance.translate(m_id);

		DecimalFormat myFormatter = new DecimalFormat("00");
		double minutes = (double) (Math.abs(WTimeSpan.GetMinutes()));
		String timeZoneTimeDiff = String.format("%d:%s", WTimeSpan.GetHours() + m_dst.GetHours(), myFormatter.format(minutes));
		return (String.format("%s: %s (%s)", timeZoneCode, timeZoneName, timeZoneTimeDiff));
	}

	/**
	 * Compare this timezone info to another.
	 */
	@Override
	public int compareTo(TimeZoneInfo o) {
		if (o == null)
			return -1;

		int compareTimeZone = this.m_timezone.AsDateTimeSpan().compareTo(o.m_timezone.AsDateTimeSpan());
		int comareDST = this.m_dst.AsDateTimeSpan().compareTo(o.m_dst.AsDateTimeSpan());

		if (compareTimeZone == 0) {
			return comareDST;
		} else {
			return compareTimeZone;
		}
	}
}
