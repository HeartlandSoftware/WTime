/**
 * TimeZoneInfo.java
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


package ca.hss.times;

import java.text.DecimalFormat;

import ca.hss.text.TranslationCallback;

/**
 * An immutable class that stores information about timezones. 
 */
public class TimeZoneInfo implements Comparable<TimeZoneInfo> {
	private WTimeSpan m_TimeZone;
	private WTimeSpan m_Dst;
	private String m_Code;
	private String m_Name;
	private String m_Id;
	
	/**
	 * Get the timezone offset from UTC.
	 * @return the timezone offset
	 */
	public WTimeSpan getTimezoneOffset() { return m_TimeZone; }
	/**
	 * Get the amount of offset from UTC caused by DST.
	 * @return the DST offset
	 */
	public WTimeSpan getDSTAmount() { return m_Dst; }
	/**
	 * Get the timzones string code.
	 * @return the timezone code
	 */
	public String getCode() { return m_Code; }
	/**
	 * Get the name of the timezone.
	 * @return the timezones name
	 */
	public String getName() { return m_Name; }
	/**
	 * Get the unique ID of the timezone. Useful for translation.
	 * @return the timezones unique id
	 */
	public String getID() { return m_Id; }

	TimeZoneInfo(WTimeSpan _timezone, WTimeSpan _dst, String _code, String _name, String _id) {
		m_TimeZone = _timezone;
		m_Dst = _dst;
		m_Code = _code;
		m_Name = _name;
		m_Id = _id;
	}

	/**
	 * Converts the time zone info into a string. If a translation callback instance has
	 * been set in ca.hss.text.TranslationCallback it will be used to translate the
	 * timezone name.
	 */
	@Override
	public String toString() {
		WTimeSpan WTimeSpan;
		WTimeSpan = m_TimeZone;
		String timeZoneCode = m_Code;
		String timeZoneName = m_Name;
		if (TranslationCallback.instance != null && m_Id.length() > 0)
			timeZoneName = TranslationCallback.instance.translate(m_Id);

		DecimalFormat myFormatter = new DecimalFormat("00");
		double minutes = (double) (Math.abs(WTimeSpan.getMinutes()));
		String timeZoneTimeDiff = String.format("%d:%s", WTimeSpan.getHours() + m_Dst.getHours(), myFormatter.format(minutes));
		return (String.format("%s: %s (%s)", timeZoneCode, timeZoneName, timeZoneTimeDiff));
	}

	/**
	 * Compare this timezone info to another.
	 */
	@Override
	public int compareTo(TimeZoneInfo o) {
		if (o == null)
			return -1;

		int compareTimeZone = this.m_TimeZone.asDateTimeSpan().compareTo(o.m_TimeZone.asDateTimeSpan());
		int comareDST = this.m_Dst.asDateTimeSpan().compareTo(o.m_Dst.asDateTimeSpan());

		if (compareTimeZone == 0) {
			return comareDST;
		} else {
			return compareTimeZone;
		}
	}
	
	@Override
	public boolean equals(Object other) {
		if (other instanceof TimeZoneInfo) {
			return m_Code.equals(((TimeZoneInfo)other).m_Code);
		}
		return false;
	}
}
