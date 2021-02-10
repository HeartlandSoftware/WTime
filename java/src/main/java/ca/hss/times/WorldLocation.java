/**
 * WorldLocation.java
 *
 * Copyright 2016-2021 Heartland Software Solutions Inc.
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

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Optional;

import ca.hss.annotations.Source;

import ca.hss.general.OutVariable;

import static ca.hss.math.General.*;

/**
 * Stores information about a geographical area including its coordinates
 * and its timezone information.
 */
@Source(project="Times", sourceFile="worldlocation.cpp")
public class WorldLocation implements Serializable {
	public static final long serialVersionUID = 4;

	double		m_Latitude;			// stored as radians!!!
	double		m_Longitude;
	WTimeSpan	m_TimeZone;			// time zone
	WTimeSpan	m_StartDST;			// when daylight savings turns on - from start of year
	WTimeSpan	m_EndDST;			// when daylight savings turns off - from start of year
											// if you want to disable DST, then just set these to the same value
	WTimeSpan	m_AmtDST;			// amount to adjust for DST
	
	/**
	 * If a timezone is known it will be saved. Having
	 * timezone information in an object doesn't mean that
	 * the timezone will be known though.
	 */
	protected TimeZoneInfo m_timezone;
	
	/**
	 * Get the locations latitude in radians.
	 * @return
	 */
	public double getLatitude() { return m_Latitude; }
	/**
	 * Set the locations latitude in radians.
	 * @param latitude
	 */
	public void setLatitude(double latitude) { m_Latitude = latitude; }
	/**
	 * Get the locations longitude in radians.
	 * @return
	 */
	public double getLongitude() { return m_Longitude; }
	/**
	 * Set the locations longitude in radians.
	 */
	public void setLongitude(double longitude) { m_Longitude = longitude; }
	/**
	 * Get the locations timezone offset.
	 * @return
	 */
	public WTimeSpan getTimezoneOffset() { return m_TimeZone; }
	/**
	 * Set the locations timezone offset.
	 * @param offset
	 */
	public void setTimezoneOffset(WTimeSpan offset) { m_TimeZone = offset; m_timezone = null; }
	/**
	 * Get the locations starting DST offset from January 1st 0:00.
	 * @return
	 */
	public WTimeSpan getStartDST() { return m_StartDST; }
	/**
	 * Set the locations starting DST offset from January 1st 0:00.
	 * @param start
	 */
	public void setStartDST(WTimeSpan start) { m_StartDST = start; m_timezone = null; }
	/**
	 * Get the locations ending DST offset from January 1st 0:00.
	 * @return
	 */
	public WTimeSpan getEndDST() { return m_EndDST; }
	/**
	 * Set the locations ending DST offset from January 1st 0:00.
	 * @param end
	 */
	public void setEndDST(WTimeSpan end) { m_EndDST = end; m_timezone = null; }
	/**
	 * Get the amount of offset from DST.
	 * @return
	 */
	public WTimeSpan getDSTAmount() { return m_AmtDST; }
	/**
	 * Set the amount of offset from DST.
	 * @param amount
	 */
	public void setDSTAmount(WTimeSpan amount) { m_AmtDST = amount; m_timezone = null; }
	/**
	 * Get the pre-defined timezone that was used to initialize this time. 
	 * @return The timezone info if present, null otherwise.
	 */
	public TimeZoneInfo getTimeZoneInfo() { return m_timezone; }
	
	/**
	 * Set the timezone offset and the DST amount using a {@link TimeZoneInfo}.
	 * @param timezone The timezone to apply to this world location. If the
	 * timezone is has a DST offset the end DST time will be set to day 366.
	 */
	public void setTimezoneOffset(TimeZoneInfo timezone) {
        setDSTAmount(timezone.getDSTAmount());
        setTimezoneOffset(timezone.getTimezoneOffset());
        setStartDST(new WTimeSpan(0));
        if (timezone.getDSTAmount().getTotalSeconds() > 0)
            setEndDST(new WTimeSpan(366, 0, 0, 0));
        m_timezone = timezone;
	}
	
	private static final int STD_TIMEZONE_ID = 0x10000;
	private static final int DST_TIMEZONE_ID = 0x20000;
	private static final int MIL_TIMEZONE_ID = 0x40000;
	private static int MAKE_ID(int zone, int id) {
	    return zone | id;
	}
	private static boolean IS_STD(int id) { return (STD_TIMEZONE_ID & id) != 0; }
	private static boolean IS_DST(int id) { return (DST_TIMEZONE_ID & id) != 0; }
	@SuppressWarnings("unused")
    private static boolean IS_MIL(int id) { return (MIL_TIMEZONE_ID & id) != 0; }

	protected static final TimeZoneInfo[] m_dst_timezones = {
		new TimeZoneInfo(new WTimeSpan(0, +9, 30, 0), new WTimeSpan(0, 1, 0, 0), "ACDT", "Australian Central Daylight Time", "ui.label.zone.acdt", MAKE_ID(DST_TIMEZONE_ID, 0)),//australia//0
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0, 1, 0, 0), "ADT", "Arabia Daylight Time", "ui.label.zone.ardt", MAKE_ID(DST_TIMEZONE_ID, 18)),//asia//1
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "ADT", "Atlantic Daylight Time", "ui.label.zone.adt", MAKE_ID(DST_TIMEZONE_ID, 1)),//north america//2
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0, 1, 0, 0), "AEDT", "Australian Eastern Daylight Time", "ui.label.zone.aedt", MAKE_ID(DST_TIMEZONE_ID, 2)),//australia//3
		new TimeZoneInfo(new WTimeSpan(0, -9, 00, 0), new WTimeSpan(0, 1, 0, 0), "AKDT", "Alaska Daylight Time", "ui.label.zone.akdt", MAKE_ID(DST_TIMEZONE_ID, 3)),//north america//4
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "AMST", "Amazon Summer Time", "ui.label.zone.amst", MAKE_ID(DST_TIMEZONE_ID, 19)),//south america//5
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0, 1, 0, 0), "AWDT", "Australian Western Daylight Time", "ui.label.zone.awdt", MAKE_ID(DST_TIMEZONE_ID, 4)),//australia//6
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0, 1, 0, 0), "AZOST", "Azores Summer Time", "ui.label.zone.azost", MAKE_ID(DST_TIMEZONE_ID, 20)),//atlantic//7
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0, 1, 0, 0), "AZST", "Azerbaijan Summer Time", "ui.label.zone.azst", MAKE_ID(DST_TIMEZONE_ID, 21)),//asia//8
		new TimeZoneInfo(new WTimeSpan(0, +0, 00, 0), new WTimeSpan(0, 1, 0, 0), "BST", "British Summer Time", "ui.label.zone.bst", MAKE_ID(DST_TIMEZONE_ID, 5)),//europe//9
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0, 1, 0, 0), "BRST", "Bras\u00EDlia Summer Time ", "ui.label.zone.brst", MAKE_ID(DST_TIMEZONE_ID, 22)),//south america//10
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0, 1, 0, 0), "CDT", "Central Daylight Time", "ui.label.zone.cdt", MAKE_ID(DST_TIMEZONE_ID, 6)),//north america//11
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0, 1, 0, 0), "CEST", "Central European Summer Time", "ui.label.zone.cest", MAKE_ID(DST_TIMEZONE_ID, 7)),//europe//12
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "CDT", "Cuba Daylight Time", "ui.label.zone.cudt", MAKE_ID(DST_TIMEZONE_ID, 23)),//caribbean//13
		new TimeZoneInfo(new WTimeSpan(0, +12, 45, 0), new WTimeSpan(0, 1, 0, 0), "CHADT", "Chatham Island Daylight Time", "ui.label.zone.chadt", MAKE_ID(DST_TIMEZONE_ID, 24)),//pacific//14
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "CLST", "Chile Summer Time", "ui.label.zone.clst", MAKE_ID(DST_TIMEZONE_ID, 25)),//south america//15
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0, 1, 0, 0), "EASST", "Easter Island Summer Time", "ui.label.zone.easst", MAKE_ID(DST_TIMEZONE_ID, 26)),//pacific//16
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0, 1, 0, 0), "EDT", "Eastern Daylight Time", "ui.label.zone.edt", MAKE_ID(DST_TIMEZONE_ID, 8)),//north america//17
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0, 1, 0, 0), "EEST", "Eastern European Summer Time", "ui.label.zone.eest", MAKE_ID(DST_TIMEZONE_ID, 9)),//europe//18
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0, 1, 0, 0), "EGST", "Eastern Greenland Summer Time", "ui.label.zone.egst", MAKE_ID(DST_TIMEZONE_ID, 27)),//north america//19
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "FKST", "Falkland Islands Summer Time", "ui.label.zone.fkst", MAKE_ID(DST_TIMEZONE_ID, 28)),//south america//20
		new TimeZoneInfo(new WTimeSpan(0, -10, 00, 0), new WTimeSpan(0, 1, 0, 0), "HADT", "Hawaii-Aleutien Daylight Time", "ui.label.zone.hadt", MAKE_ID(DST_TIMEZONE_ID, 10)),//north america//21
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0, 1, 0, 0), "IDT", "Israel Daylight Time", "ui.label.zone.idt", MAKE_ID(DST_TIMEZONE_ID, 29)),//asia//22
		new TimeZoneInfo(new WTimeSpan(0, +3, 30, 0), new WTimeSpan(0, 1, 0, 0), "IRDT", "Iran Daylight Time", "ui.label.zone.irdt", MAKE_ID(DST_TIMEZONE_ID, 30)),//asia//23
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0, 1, 0, 0), "IRKST", "Irkutsk Summer Time", "ui.label.zone.irkst", MAKE_ID(DST_TIMEZONE_ID, 31)),//asia//24
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0, 1, 0, 0), "KRAST", "Krasnoyarsk Summer Time", "ui.label.zone.krast", MAKE_ID(DST_TIMEZONE_ID, 32)),//asia//25
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0, 1, 0, 0), "LHDT", "Lord Howe Daylight Time", "ui.label.zone.lhdt", MAKE_ID(DST_TIMEZONE_ID, 33)),//australia//26
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0, 1, 0, 0), "MAGST", "Magadan Summer Time", "ui.label.zone.magst", MAKE_ID(DST_TIMEZONE_ID, 34)),//asia//27
		new TimeZoneInfo(new WTimeSpan(0, -7, 00, 0), new WTimeSpan(0, 1, 0, 0), "MDT", "Mountain Daylight Time", "ui.label.zone.mdt", MAKE_ID(DST_TIMEZONE_ID, 12)),//north america//28
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0, 1, 0, 0), "MSD", "Moscow Daylight Time", "ui.label.zone.msd", MAKE_ID(DST_TIMEZONE_ID, 13)),//europe//29
		new TimeZoneInfo(new WTimeSpan(0, -3, -30, 0), new WTimeSpan(0, 1, 0, 0), "NDT", "Newfoundland Daylight Time", "ui.label.zone.ndt", MAKE_ID(DST_TIMEZONE_ID, 14)),//north america//30
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0, 1, 0, 0), "NOVST", "Novosibirsk Summer Time", "ui.label.zone.novst", MAKE_ID(DST_TIMEZONE_ID, 35)),//asia//31
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0, 1, 0, 0), "NZDT", "New Zealand Daylight Time", "ui.label.zone.nzdt", MAKE_ID(DST_TIMEZONE_ID, 15)),//pacific//32
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0, 1, 0, 0), "OMSST", "Omsk Summer Time", "ui.label.zone.omsst", MAKE_ID(DST_TIMEZONE_ID, 36)),//asia//33
		new TimeZoneInfo(new WTimeSpan(0, -8, 00, 0), new WTimeSpan(0, 1, 0, 0), "PDT", "Pacific Daylight Time", "ui.label.zone.pdt", MAKE_ID(DST_TIMEZONE_ID, 16)),//north america//34
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0, 1, 0, 0), "PETST", "Kamchatka Summer Time", "ui.label.zone.petst", MAKE_ID(DST_TIMEZONE_ID, 37)),//asia//35
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0, 1, 0, 0), "PMDT", "Pierre & Miquelon Daylight Time", "ui.label.zone.pmdt", MAKE_ID(DST_TIMEZONE_ID, 38)),//north america//36
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0, 1, 0, 0), "UYST", "Uruguay Summer Time", "ui.label.zone.uyst", MAKE_ID(DST_TIMEZONE_ID, 39)),//south america//37
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0, 1, 0, 0), "VLAST", "Vladivostok Summer Time", "ui.label.zone.vlast", MAKE_ID(DST_TIMEZONE_ID, 40)),//asia//38
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "WARST", "Western Argentine Summer Time", "ui.label.zone.warst", MAKE_ID(DST_TIMEZONE_ID, 41)),//south america//39
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0, 1, 0, 0), "WAST", "West Africa Summer Time", "ui.label.zone.wast", MAKE_ID(DST_TIMEZONE_ID, 42)),//africa//40
		new TimeZoneInfo(new WTimeSpan(0, 0, 00, 0), new WTimeSpan(0, 1, 0, 0), "WEST", "Western European Summer Time", "ui.label.zone.west", MAKE_ID(DST_TIMEZONE_ID, 17)),//europe//41
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0, 1, 0, 0), "WGST", "Western Greenland Summer Time", "ui.label.zone.wgst", MAKE_ID(DST_TIMEZONE_ID, 43)),//north america//42
		new TimeZoneInfo(new WTimeSpan(0, 0, 00, 0), new WTimeSpan(0, 1, 0, 0), "WST", "Western Sahara Summer Time", "ui.label.zone.wst", MAKE_ID(DST_TIMEZONE_ID, 44)),//africa//43
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0, 1, 0, 0), "YAKST", "Yakutsk Summer Time", "ui.label.zone.yakst", MAKE_ID(DST_TIMEZONE_ID, 45)),//asia//44
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0, 1, 0, 0), "YEKST", "Yekaterinburg Summer Time", "ui.label.zone.yekst", MAKE_ID(DST_TIMEZONE_ID, 46)),//asia//45
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0, 1, 0, 0), "FJST", "Fiji Summer Time", "ui.label.zone.fjst", MAKE_ID(DST_TIMEZONE_ID, 47)),//pacific//46
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "PYST", "Paraguay Summer Time", "ui.label.zone.pyst", MAKE_ID(DST_TIMEZONE_ID, 48)),//south america//47
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0, 1, 0, 0), "AMST", "Armenia Summer Time", "ui.label.zone.armst", MAKE_ID(DST_TIMEZONE_ID, 49)),//asia//48
	};

	protected static final TimeZoneInfo[] m_std_timezones = {
		//a
		new TimeZoneInfo(new WTimeSpan(0, +9, 30, 0), new WTimeSpan(0), "ACST", "Australian Central Standard Time", "ui.label.zone.acst", MAKE_ID(STD_TIMEZONE_ID, 0)),//australia//0
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "ACT", "Acre Time", "ui.label.zone.act", MAKE_ID(STD_TIMEZONE_ID, 28)),//south america//1
		new TimeZoneInfo(new WTimeSpan(0, +8, 45, 0), new WTimeSpan(0), "ACWST", "Australian Central Western Standard Time", "ui.label.zone.acwst", MAKE_ID(STD_TIMEZONE_ID, 29)),//australia//2
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "AEST", "Australian Eastern Standard Time", "ui.label.zone.aest", MAKE_ID(STD_TIMEZONE_ID, 1)),//australia//3
		new TimeZoneInfo(new WTimeSpan(0, +4, 30, 0), new WTimeSpan(0), "AFT", "Afghanistan Time", "ui.label.zone.aft", MAKE_ID(STD_TIMEZONE_ID, 30)),//asia//4
		new TimeZoneInfo(new WTimeSpan(0, -9, 00, 0), new WTimeSpan(0), "AKST", "Alaska Standard Time", "ui.label.zone.akst", MAKE_ID(STD_TIMEZONE_ID, 2)),//north america//5
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "ALMT", "Alma-Ata Time", "ui.label.zone.almt", MAKE_ID(STD_TIMEZONE_ID, 31)),//asia//6
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "AMT", "Amazon Time", "ui.label.zone.amt", MAKE_ID(STD_TIMEZONE_ID, 32)),//south america//7
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "AMT", "Armenia Time", "ui.label.zone.armt", MAKE_ID(STD_TIMEZONE_ID, 33)),//asia//8
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "ANAT", "Anadyr Time", "ui.label.zone.anat", MAKE_ID(STD_TIMEZONE_ID, 34)),//asia//9
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "AQTT", "Aqtobe Time", "ui.label.zone.aqtt", MAKE_ID(STD_TIMEZONE_ID, 35)),//asia//10
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "ART", "Argentina Time", "ui.label.zone.art", MAKE_ID(STD_TIMEZONE_ID, 36)),//south america//11
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "AST", "Arabia Standard Time", "ui.label.zone.arst", MAKE_ID(STD_TIMEZONE_ID, 37)),//asia//12
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "AST", "Atlantic Standard Time", "ui.label.zone.ast", MAKE_ID(STD_TIMEZONE_ID, 3)),//north america//13
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "AWST", "Australian Western Standard Time", "ui.label.zone.awst", MAKE_ID(STD_TIMEZONE_ID, 4)),//australia//14
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0), "AZOT", "Azores Time", "ui.label.zone.azot", MAKE_ID(STD_TIMEZONE_ID, 22)),//atlantic//15
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "AZT", "Azerbaijan Time", "ui.label.zone.azt", MAKE_ID(STD_TIMEZONE_ID, 38)),//asia//16
		new TimeZoneInfo(new WTimeSpan(0, -12, 00, 0), new WTimeSpan(0), "AoE", "Anywhere on Earth", "ui.label.zone.aoe", MAKE_ID(STD_TIMEZONE_ID, 39)),//pacific//17
		//b
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "BNT", "Brunei Darussalam Time", "ui.label.zone.bnt", MAKE_ID(STD_TIMEZONE_ID, 40)),//asia//18
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "BOT", "Bolivia Time", "ui.label.zone.bot", MAKE_ID(STD_TIMEZONE_ID, 41)),//south america//19
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "BRT", "Bras\u00EDlia Time", "ui.label.zone.brt", MAKE_ID(STD_TIMEZONE_ID, 42)),//south america//20
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "BST", "Bangladesh Standard Time", "ui.label.zone.bast", MAKE_ID(STD_TIMEZONE_ID, 43)),//asia//21
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "BTT", "Bhutan Time", "ui.label.zone.btt", MAKE_ID(STD_TIMEZONE_ID, 44)),//asia//22
		//c
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "CAST", "Casey Time", "ui.label.zone.cast", MAKE_ID(STD_TIMEZONE_ID, 45)),//antarctica//23
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0), "CAT", "Central Africa Time", "ui.label.zone.cat", MAKE_ID(STD_TIMEZONE_ID, 46)),//africa//24
		new TimeZoneInfo(new WTimeSpan(0, +6, 30, 0), new WTimeSpan(0), "CCT", "Cocos Islands Time", "ui.label.zone.cct", MAKE_ID(STD_TIMEZONE_ID, 47)),//indian ocean//25
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0), "CET", "Central European Time", "ui.label.zone.cet", MAKE_ID(STD_TIMEZONE_ID, 5)),//europe//26
		new TimeZoneInfo(new WTimeSpan(0, +12, 45, 0), new WTimeSpan(0), "CHAST", "Chatham Island Standard Time", "ui.label.zone.chast", MAKE_ID(STD_TIMEZONE_ID, 48)),//pacific//27
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "CHOT", "Choibalsan Time", "ui.label.zone.chot", MAKE_ID(STD_TIMEZONE_ID, 49)),//asia//28
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "CHUT", "Chuuk Time", "ui.label.zone.chut", MAKE_ID(STD_TIMEZONE_ID, 50)),//pacific//29
		new TimeZoneInfo(new WTimeSpan(0, -10, 00, 0), new WTimeSpan(0), "CKT", "Cook Island Time", "ui.label.zone.ckt", MAKE_ID(STD_TIMEZONE_ID, 51)),//pacific//30
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "CLT", "Chile Standard Time", "ui.label.zone.clt", MAKE_ID(STD_TIMEZONE_ID, 52)),//south america//31
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "COT", "Colombia Time", "ui.label.zone.cot", MAKE_ID(STD_TIMEZONE_ID, 53)),//south america//32
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0), "CST", "Central Standard Time", "ui.label.zone.cst", MAKE_ID(STD_TIMEZONE_ID, 6)),//north america//33
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "CST", "China Standard Time", "ui.label.zone.chst", MAKE_ID(STD_TIMEZONE_ID, 54)),//asia//34
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "CST", "Cuba Standard Time", "ui.label.zone.cust", MAKE_ID(STD_TIMEZONE_ID, 55)),//caribbean//35
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0), "CVT", "Cape Verde Time", "ui.label.zone.cvt", MAKE_ID(STD_TIMEZONE_ID, 56)),//africa//36
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "CXT", "Christmas Island Time", "ui.label.zone.cxt", MAKE_ID(STD_TIMEZONE_ID, 7)),//australia//37
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "ChST", "Chamorro Standard Time", "ui.label.zone.chst", MAKE_ID(STD_TIMEZONE_ID, 57)),//pacific//38
		//d
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "DAVT", "Davis Time", "ui.label.zone.davt", MAKE_ID(STD_TIMEZONE_ID, 58)),//antarctica//39
		//e
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0), "EAST", "Easter Island Standard Time", "ui.label.zone.east", MAKE_ID(STD_TIMEZONE_ID, 59)),//pacific//40
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "EAT", "Eastern Africa Time", "ui.label.zone.eat", MAKE_ID(STD_TIMEZONE_ID, 60)),//africa//41
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "ECT", "Ecuador Time", "ui.label.zone.ect", MAKE_ID(STD_TIMEZONE_ID, 61)),//south america//42
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0), "EET", "Eastern European Time", "ui.label.zone.eet", MAKE_ID(STD_TIMEZONE_ID, 8)),//europe//43
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0), "EGT", "East Greenland Time", "ui.label.zone.egt", MAKE_ID(STD_TIMEZONE_ID, 62)),//north america//44
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "EST", "Eastern Standard Time", "ui.label.zone.est", MAKE_ID(STD_TIMEZONE_ID, 9)),//north america//45
		//f
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "FET", "Further-Eastern European Time", "ui.label.zone.fet", MAKE_ID(STD_TIMEZONE_ID, 63)),//europe//46
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "FJT", "Fiji Time", "ui.label.zone.fjt", MAKE_ID(STD_TIMEZONE_ID, 64)),//pacific//47
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "FKT", "Falkland Island Time", "ui.label.zone.fkt", MAKE_ID(STD_TIMEZONE_ID, 65)),//south america//48
		new TimeZoneInfo(new WTimeSpan(0, -2, 00, 0), new WTimeSpan(0), "FNT", "Fernando de Noronha Time", "ui.label.zone.fnt", MAKE_ID(STD_TIMEZONE_ID, 66)),//south america//49
		//g
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0), "GALT", "Galapagos Time", "ui.label.zone.galt", MAKE_ID(STD_TIMEZONE_ID, 67)),//pacific//50
		new TimeZoneInfo(new WTimeSpan(0, -9, 00, 0), new WTimeSpan(0), "GAMT", "Gambier Time", "ui.label.zone.gamt", MAKE_ID(STD_TIMEZONE_ID, 68)),//pacific//51
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "GET", "Georgia Standard Time", "ui.label.zone.get", MAKE_ID(STD_TIMEZONE_ID, 69)),//asia//52
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "GFT", "French Guiana Time", "ui.label.zone.gft", MAKE_ID(STD_TIMEZONE_ID, 70)),//south america//53
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "GILT", "Gilbert Island Time", "ui.label.zone.gilt", MAKE_ID(STD_TIMEZONE_ID, 71)),//pacific//54
		new TimeZoneInfo(new WTimeSpan(0, +0, 00, 0), new WTimeSpan(0), "GMT", "Greenwich Mean Time", "ui.label.zone.gmt", MAKE_ID(STD_TIMEZONE_ID, 72)),//europe//55
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "GST", "Gulf Standard Time", "ui.label.zone.gust", MAKE_ID(STD_TIMEZONE_ID, 73)),//asia//56
		new TimeZoneInfo(new WTimeSpan(0, -2, 00, 0), new WTimeSpan(0), "GST", "South Georgia Time", "ui.label.zone.gst", MAKE_ID(STD_TIMEZONE_ID, 74)),//south america//57
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "GYT", "Guyana Time", "ui.label.zone.gyt", MAKE_ID(STD_TIMEZONE_ID, 75)),//south america//58
		//h
		new TimeZoneInfo(new WTimeSpan(0, -10, 00, 0), new WTimeSpan(0), "HAST", "Hawaii-Aleutien Standard Time", "ui.label.zone.hast", MAKE_ID(STD_TIMEZONE_ID, 10)),//north america//59
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "HKT", "Hong Kong Time", "ui.label.zone.hkt", MAKE_ID(STD_TIMEZONE_ID, 76)),//asia//60
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "HOVT", "Hovd Time", "ui.label.zone.hovt", MAKE_ID(STD_TIMEZONE_ID, 77)),//asia//61
		//i
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "ICT", "Indochina Time", "ui.label.zone.ict", MAKE_ID(STD_TIMEZONE_ID, 78)),//asia//62
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "IOT", "Indian Chagos Time", "ui.label.zone.iot", MAKE_ID(STD_TIMEZONE_ID, 79)),//indian ocean//63
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "IRKT", "Irkutsk Time", "ui.label.zone.irkt", MAKE_ID(STD_TIMEZONE_ID, 80)),//asia//64
		new TimeZoneInfo(new WTimeSpan(0, +3, 30, 0), new WTimeSpan(0), "IRST", "Iran Standard Time", "ui.label.zone.irst", MAKE_ID(STD_TIMEZONE_ID, 81)),//asia//65
		new TimeZoneInfo(new WTimeSpan(0, +5, 30, 0), new WTimeSpan(0), "IST", "India Standard Time", "ui.label.zone.inst", MAKE_ID(STD_TIMEZONE_ID, 24)),//asia//66
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0), "IST", "Irish Standard Time", "ui.label.zone.ist", MAKE_ID(STD_TIMEZONE_ID, 82)),//europe//67
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0), "IST", "Israel Standard Time", "ui.label.zone.isst", MAKE_ID(STD_TIMEZONE_ID, 83)),//asia//68
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "JST", "Japan Standard Time", "ui.label.zone.jst", MAKE_ID(STD_TIMEZONE_ID, 26)),//asia//69
		//k
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "KGT", "Kyrgyzstan Time", "ui.label.zone.kgt", MAKE_ID(STD_TIMEZONE_ID, 84)),//asia//70
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "KOST", "Kosrae Time", "ui.label.zone.kost", MAKE_ID(STD_TIMEZONE_ID, 85)),//pacific//71
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "KRAT", "Krasnoyarsk Time", "ui.label.zone.krat", MAKE_ID(STD_TIMEZONE_ID, 86)),//asia//72
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "KST", "Korea Standard Time", "ui.label.zone.kst", MAKE_ID(STD_TIMEZONE_ID, 87)),//asia//73
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "KUYT", "Kuybyshev Time", "ui.label.zone.kuyt", MAKE_ID(STD_TIMEZONE_ID, 88)),//europe//74
		//l
		new TimeZoneInfo(new WTimeSpan(0, +10, 30, 0), new WTimeSpan(0), "LHST", "Lord Howe Standard Time", "ui.label.zone.lhst", MAKE_ID(STD_TIMEZONE_ID, 89)),//australia//75
		new TimeZoneInfo(new WTimeSpan(0, +14, 00, 0), new WTimeSpan(0), "LINT", "Line Islands Time", "ui.label.zone.lint", MAKE_ID(STD_TIMEZONE_ID, 90)),//pacific//76
		//m
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "MAGT", "Magadan Time", "ui.label.zone.magt", MAKE_ID(STD_TIMEZONE_ID, 91)),//asia//77
		new TimeZoneInfo(new WTimeSpan(0, -9, -30, 0), new WTimeSpan(0), "MART", "Marquesas Time", "ui.label.zone.mart", MAKE_ID(STD_TIMEZONE_ID, 92)),//pacific//78
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "MAWT", "Mawson Time", "ui.label.zone.mawt", MAKE_ID(STD_TIMEZONE_ID, 93)),//antarctica//79
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "MHT", "Marshall Islands Time", "ui.label.zone.mht", MAKE_ID(STD_TIMEZONE_ID, 94)),//pacific//80
		new TimeZoneInfo(new WTimeSpan(0, +6, 30, 0), new WTimeSpan(0), "MMT", "Myanmar Time", "ui.label.zone.mmt", MAKE_ID(STD_TIMEZONE_ID, 95)),//asia//81
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "MSK", "Moscow Standard Time", "ui.label.zone.msk", MAKE_ID(STD_TIMEZONE_ID, 11)),//europe//82
		new TimeZoneInfo(new WTimeSpan(0, -7, 00, 0), new WTimeSpan(0), "MST", "Mountain Standard Time", "ui.label.zone.mst", MAKE_ID(STD_TIMEZONE_ID, 12)),//north america//83
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "MUT", "Mauritius Time", "ui.label.zone.mut", MAKE_ID(STD_TIMEZONE_ID, 96)),//africa//84
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "MVT", "Maldives Time", "ui.label.zone.mvt", MAKE_ID(STD_TIMEZONE_ID, 97)),//asia//85
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "MYT", "Malaysia Time", "ui.label.zone.myt", MAKE_ID(STD_TIMEZONE_ID, 98)),//asia//86
		//n
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "NCT", "New Caledonia Time", "ui.label.zone.nct", MAKE_ID(STD_TIMEZONE_ID, 99)),//pacific//87
		new TimeZoneInfo(new WTimeSpan(0, +11, 30, 0), new WTimeSpan(0), "NFT", "Norfolk Time", "ui.label.zone.nft", MAKE_ID(STD_TIMEZONE_ID, 13)),//australia//88
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "NOVT", "Novosibirsk Time", "ui.label.zone.novt", MAKE_ID(STD_TIMEZONE_ID, 100)),//asia//89
		new TimeZoneInfo(new WTimeSpan(0, +5, 45, 0), new WTimeSpan(0), "NPT", "Nepal Time", "ui.label.zone.npt", MAKE_ID(STD_TIMEZONE_ID, 101)),//asia//90
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "NRT", "Nauru Time", "ui.label.zone.nrt", MAKE_ID(STD_TIMEZONE_ID, 102)),//pacific//91
		new TimeZoneInfo(new WTimeSpan(0, -3, -30, 0), new WTimeSpan(0), "NST", "Newfoundland Standard Time", "ui.label.zone.nst", MAKE_ID(STD_TIMEZONE_ID, 14)),//north america//92
		new TimeZoneInfo(new WTimeSpan(0, -11, 00, 0), new WTimeSpan(0), "NUT", "Niue Time", "ui.label.zone.nut", MAKE_ID(STD_TIMEZONE_ID, 103)),//pacific//93
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "NZST", "New Zealand Standard Time", "ui.label.zone.nzst", MAKE_ID(STD_TIMEZONE_ID, 15)),//pacific//94
		//o
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "OMST", "Omsk Standard Time", "ui.label.zone.omst", MAKE_ID(STD_TIMEZONE_ID, 104)),//asia//95
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "ORAT", "Oral Time", "ui.label.zone.orat", MAKE_ID(STD_TIMEZONE_ID, 105)),//asia//96
		//p
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "PET", "Peru Time", "ui.label.zone.pet", MAKE_ID(STD_TIMEZONE_ID, 106)),//south america//97
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "PETT", "Kamchatka Time", "ui.label.zone.pett", MAKE_ID(STD_TIMEZONE_ID, 107)),//asia//98
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "PGT", "Papua New Guinea Time", "ui.label.zone.pgt", MAKE_ID(STD_TIMEZONE_ID, 108)),//pacific//99
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0), "PHOT", "Phoenix Island Time", "ui.label.zone.phot", MAKE_ID(STD_TIMEZONE_ID, 109)),//pacific//100
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "PHT", "Philippine Time", "ui.label.zone.pht", MAKE_ID(STD_TIMEZONE_ID, 110)),//asia//101
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "PKT", "Pakistan Standard Time", "ui.label.zone.pkt", MAKE_ID(STD_TIMEZONE_ID, 111)),//asia//102
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "PMST", "Pierre & Miquelon Standard Time", "ui.label.zone.pmst", MAKE_ID(STD_TIMEZONE_ID, 112)),//north america//103
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "PONT", "Pohnpei Standard Time", "ui.label.zone.pont", MAKE_ID(STD_TIMEZONE_ID, 113)),//pacific//104
		new TimeZoneInfo(new WTimeSpan(0, -8, 00, 0), new WTimeSpan(0), "PST", "Pacific Standard Time", "ui.label.zone.pst", MAKE_ID(STD_TIMEZONE_ID, 16)),//north america//105
		new TimeZoneInfo(new WTimeSpan(0, -8, 00, 0), new WTimeSpan(0), "PST", "Pitcairn Standard Time", "ui.label.zone.pcst", MAKE_ID(STD_TIMEZONE_ID, 114)),//pacific//106
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "PWT", "Palau Time", "ui.label.zone.pwt", MAKE_ID(STD_TIMEZONE_ID, 115)),//pacific//107
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "PYT", "Paraguay Time", "ui.label.zone.pyt", MAKE_ID(STD_TIMEZONE_ID, 116)),//south america//108
		//q
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "QYZT", "Qyzylorda Time", "ui.label.zone.qyzt", MAKE_ID(STD_TIMEZONE_ID, 117)),//asia//109
		//r
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "RET", "Reunion Time", "ui.label.zone.ret", MAKE_ID(STD_TIMEZONE_ID, 118)),//africa//110
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "ROTT", "Rothera Time", "ui.label.zone.rott", MAKE_ID(STD_TIMEZONE_ID, 119)),//antarctica//111
		//s
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "SAKT", "Sakhalin Time", "ui.label.zone.sakt", MAKE_ID(STD_TIMEZONE_ID, 120)),//asia//112
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "SAMT", "Samara Time", "ui.label.zone.samt", MAKE_ID(STD_TIMEZONE_ID, 121)),//europe//113
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0), "SAST", "South Africa Standard Time", "ui.label.zone.sast", MAKE_ID(STD_TIMEZONE_ID, 122)),//africa//114
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "SBT", "Solomon Islands Time", "ui.label.zone.sbt", MAKE_ID(STD_TIMEZONE_ID, 123)),//pacific//115
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "SCT", "Seychelles Time", "ui.label.zone.sct", MAKE_ID(STD_TIMEZONE_ID, 124)),//africa//116
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "SGT", "Singapore Time", "ui.label.zone.sgt", MAKE_ID(STD_TIMEZONE_ID, 125)),//asia//117
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "SRET", "Srednekolymsk Time", "ui.label.zone.sret", MAKE_ID(STD_TIMEZONE_ID, 126)),//asia//118
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "SRT", "Suriname Time", "ui.label.zone.srt", MAKE_ID(STD_TIMEZONE_ID, 127)),//south america//119
		new TimeZoneInfo(new WTimeSpan(0, -11, 00, 0), new WTimeSpan(0), "SST", "Samoa Standard Time", "ui.label.zone.sst", MAKE_ID(STD_TIMEZONE_ID, 128)),//pacific//120
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "SYOT", "Syowa Time", "ui.label.zone.syot", MAKE_ID(STD_TIMEZONE_ID, 129)),//antarctica//121
		//t
		new TimeZoneInfo(new WTimeSpan(0, -10, 00, 0), new WTimeSpan(0), "TAHT", "Tahiti Time", "ui.label.zone.taht", MAKE_ID(STD_TIMEZONE_ID, 130)),//pacific//122
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "TFT", "French Southern and Antarctic Time", "ui.label.zone.tft", MAKE_ID(STD_TIMEZONE_ID, 131)),//indian ocean//123
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "TJT", "Tajikistan Time", "ui.label.zone.tjt", MAKE_ID(STD_TIMEZONE_ID, 132)),//asia//124
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0), "TKT", "Tokelau Time", "ui.label.zone.tkt", MAKE_ID(STD_TIMEZONE_ID, 133)),//pacific//125
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "TLT", "East Timor Time", "ui.label.zone.tlt", MAKE_ID(STD_TIMEZONE_ID, 134)),//asia//126
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "TMT", "Turkmenistan Time", "ui.label.zone.tmt", MAKE_ID(STD_TIMEZONE_ID, 135)),//asia//127
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0), "TOT", "Tonga Time", "ui.label.zone.tot", MAKE_ID(STD_TIMEZONE_ID, 136)),//pacific//128
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "TVT", "Tuvalu Time", "ui.label.zone.tvt", MAKE_ID(STD_TIMEZONE_ID, 137)),//pacific//129
		//u
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "ULAT", "Ulaanbaatar Time", "ui.label.zone.ulat", MAKE_ID(STD_TIMEZONE_ID, 138)),//asia//130
		new TimeZoneInfo(new WTimeSpan(0, 0, 00, 0), new WTimeSpan(0), "UTC", "Coordinated Universal Time", "ui.label.zone.utc", MAKE_ID(STD_TIMEZONE_ID, 17)),//worldwide//131
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "UYT", "Uruguay Time", "ui.label.zone.uyt", MAKE_ID(STD_TIMEZONE_ID, 139)),//south america//132
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "UZT", "Uzbekistan Time", "ui.label.zone.uzt", MAKE_ID(STD_TIMEZONE_ID, 140)),//asia//133
		//v
		new TimeZoneInfo(new WTimeSpan(0, -4, -30, 0), new WTimeSpan(0), "VET", "Venezuelan Standard Time", "ui.label.zone.vet", MAKE_ID(STD_TIMEZONE_ID, 141)),//south america//134
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "VLAT", "Vladivostok Time", "ui.label.zone.vlat", MAKE_ID(STD_TIMEZONE_ID, 142)),//asia//135
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "VOST", "Vostok Time", "ui.label.zone.vost", MAKE_ID(STD_TIMEZONE_ID, 143)),//antarctica//136
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "VUT", "Vanuatu Time", "ui.label.zone.vut", MAKE_ID(STD_TIMEZONE_ID, 144)),//pacific//137
		//w
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "WAKT", "Wake Time", "ui.label.zone.wakt", MAKE_ID(STD_TIMEZONE_ID, 145)),//pacific//138
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0), "WAT", "West Africa Time", "ui.label.zone.wat", MAKE_ID(STD_TIMEZONE_ID, 21)),//africa//139
		new TimeZoneInfo(new WTimeSpan(0, +0, 00, 0), new WTimeSpan(0), "WET", "Western European Time", "ui.label.zone.wet", MAKE_ID(STD_TIMEZONE_ID, 146)),//europe//140
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "WFT", "Wallis and Futuna Time", "ui.label.zone.wft", MAKE_ID(STD_TIMEZONE_ID, 147)),//pacific//141
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "WGT", "West Greenland Time", "ui.label.zone.wgt", MAKE_ID(STD_TIMEZONE_ID, 148)),//north america//142
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "WIB", "Western Indonesian Time", "ui.label.zone.wib", MAKE_ID(STD_TIMEZONE_ID, 149)),//asia//143
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "WIT", "Eastern Indonesian Time", "ui.label.zone.wit", MAKE_ID(STD_TIMEZONE_ID, 150)),//asia//144
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "WITA", "Central Indonesian Time", "ui.label.zone.wita", MAKE_ID(STD_TIMEZONE_ID, 151)),//asia//145
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0), "WST", "West Samoa Time", "ui.label.zone.wsst", MAKE_ID(STD_TIMEZONE_ID, 152)),//pacific//146
		new TimeZoneInfo(new WTimeSpan(0, +0, 00, 0), new WTimeSpan(0), "WT", "Western Sahara Standard Time", "ui.label.zone.wt", MAKE_ID(STD_TIMEZONE_ID, 153)),//africa//147
		//x
		//y
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "YAKT", "Yakutsk Time", "ui.label.zone.yakt", MAKE_ID(STD_TIMEZONE_ID, 154)),//asia//148
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "YAPT", "Yap Time", "ui.label.zone.yapt", MAKE_ID(STD_TIMEZONE_ID, 155)),//pacific//149
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "YEKT", "Yekaterinburg Time", "ui.label.zone.yekt", MAKE_ID(STD_TIMEZONE_ID, 156)),//asia//150
		//z
	};

	protected static final TimeZoneInfo[] m_mil_timezones = {
			new TimeZoneInfo(new WTimeSpan(0, 0, 0, 0), new WTimeSpan(0), "Z", "Zulu Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 0)),
			new TimeZoneInfo(new WTimeSpan(0, 1, 0, 0), new WTimeSpan(0), "A", "Alpha Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 1)),
			new TimeZoneInfo(new WTimeSpan(0, 2, 0, 0), new WTimeSpan(0), "B", "Bravo Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 2)),
			new TimeZoneInfo(new WTimeSpan(0, 3, 0, 0), new WTimeSpan(0), "C", "Charlie Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 3)),
			new TimeZoneInfo(new WTimeSpan(0, 4, 0, 0), new WTimeSpan(0), "D", "Delta Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 4)),
			new TimeZoneInfo(new WTimeSpan(0, 5, 0, 0), new WTimeSpan(0), "E", "Echo Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 5)),
			new TimeZoneInfo(new WTimeSpan(0, 6, 0, 0), new WTimeSpan(0), "F", "Foxtrot Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 6)),
			new TimeZoneInfo(new WTimeSpan(0, 7, 0, 0), new WTimeSpan(0), "G", "Golf Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 7)),
			new TimeZoneInfo(new WTimeSpan(0, 8, 0, 0), new WTimeSpan(0), "H", "Hotel Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 8)),
			new TimeZoneInfo(new WTimeSpan(0, 9, 0, 0), new WTimeSpan(0), "I", "India Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 9)),
			new TimeZoneInfo(new WTimeSpan(0, 10, 0, 0), new WTimeSpan(0), "K",	"Kilo Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 10)),
			new TimeZoneInfo(new WTimeSpan(0, 11, 0, 0), new WTimeSpan(0), "L",	"Lima Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 11)),
			new TimeZoneInfo(new WTimeSpan(0, 12, 0, 0), new WTimeSpan(0), "M",	"Mike Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 12)),
			new TimeZoneInfo(new WTimeSpan(0, -1, 0, 0), new WTimeSpan(0), "N",	"November Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 13)),
			new TimeZoneInfo(new WTimeSpan(0, -2, 0, 0), new WTimeSpan(0), "O",	"Oscar Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 14)),
			new TimeZoneInfo(new WTimeSpan(0, -3, 0, 0), new WTimeSpan(0), "P",	"Papa Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 15)),
			new TimeZoneInfo(new WTimeSpan(0, -4, 0, 0), new WTimeSpan(0), "Q",	"Quebec Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 16)),
			new TimeZoneInfo(new WTimeSpan(0, -5, 0, 0), new WTimeSpan(0), "R",	"Romeo Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 17)),
			new TimeZoneInfo(new WTimeSpan(0, -6, 0, 0), new WTimeSpan(0), "S",	"Sierra Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 18)),
			new TimeZoneInfo(new WTimeSpan(0, -7, 0, 0), new WTimeSpan(0), "T",	"Tango Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 19)),
			new TimeZoneInfo(new WTimeSpan(0, -8, 0, 0), new WTimeSpan(0), "U",	"Uniform Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 20)),
			new TimeZoneInfo(new WTimeSpan(0, -9, 0, 0), new WTimeSpan(0), "V",	"Vector Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 21)),
			new TimeZoneInfo(new WTimeSpan(0, -10, 0, 0), new WTimeSpan(0), "W", "Whiskey Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 22)),
			new TimeZoneInfo(new WTimeSpan(0, -11, 0, 0), new WTimeSpan(0), "X", "X-ray Time Zone" , "", MAKE_ID(MIL_TIMEZONE_ID, 23)),
			new TimeZoneInfo(new WTimeSpan(0, -12, 0, 0), new WTimeSpan(0), "Y", "Yankee Time Zone", "", MAKE_ID(MIL_TIMEZONE_ID, 24))
	};

	/**
	 * Different groups that the timezones can be requested in.
	 *
	 */
	public enum TimeZoneGroup {
		/**
		 * Fetch all timezones.
		 */
		All(0),
		/**
		 * Only fetch Canadian timezones.
		 */
		Canada(1),
		/**
		 * Only fetch African timezones.
		 */
		Africa(2),
		/**
		 * Only fetch Asian timezones.
		 */
		Asia(3),
		/**
		 * Only fetch European timezones.
		 */
		Europe(4),
		/**
		 * Only fetch North American timezones.
		 */
		North_America(5),
		/**
		 * Only fetch Oceania timezones.
		 */
		Oceania(6),
		/**
		 * Only fetch South American timezones.
		 */
		South_America(7);

		int id;

		TimeZoneGroup(int id) {
			this.id = id;
		}

		public int getId() { return id; }

		public static TimeZoneGroup fromId(int id) {
			for (TimeZoneGroup g : TimeZoneGroup.values()) {
				if (g.id == id)
					return g;
			}
			return All;
		}
	}

	/**
	 * Get all timezones in a given group.
	 * @param group the group to fetch timezones for
	 * @return an array of timezones
	 */
	public static TimeZoneInfo[] getTimezones(TimeZoneGroup group) {
		switch (group) {
		case All:
			return  getList();
		case Africa:
			return getAfricaList();
		case Asia:
			return getAsiaList();
		case Europe:
			return getEuropeList();
		case North_America:
			return getNorthAmericaList();
		case Oceania:
			return getOceaniaList();
		case South_America:
			return getSouthAmericaList();
		default:
			return getCanadaList();
		}
	}

	/**
	 * Get an arbitrarily defined default timezone.
	 * @return
	 */
	public static TimeZoneInfo defaultTimeZone() {
		return m_std_timezones[84];
	}

	/**
	 * Get the list of Canadian timezones sorted by UTC offset.
	 * @return
	 */
	public static TimeZoneInfo[] getCanadaList() {
		TimeZoneInfo[] list = {
				m_std_timezones[131],
				m_std_timezones[105],
				m_dst_timezones[34],
				m_std_timezones[83],
				m_dst_timezones[28],
				m_std_timezones[33],
				m_dst_timezones[11],
				m_std_timezones[45],
				m_dst_timezones[17],
				m_std_timezones[13],
				m_dst_timezones[2],
				m_std_timezones[92],
				m_dst_timezones[30]
		};
		return list;
	}

	/**
	 * Get the African timezones sorted by UTC offset.
	 * @return
	 */
	public static TimeZoneInfo[] getAfricaList() {
		TimeZoneInfo[] list = {
				m_std_timezones[131],
				m_std_timezones[36],
				m_std_timezones[55],
				m_std_timezones[140],
				m_std_timezones[147],
				m_std_timezones[26],
				m_std_timezones[139],
				m_dst_timezones[41],
				m_dst_timezones[43],
				m_std_timezones[43],
				m_std_timezones[24],
				m_std_timezones[114],
				m_dst_timezones[40],
				m_std_timezones[41],
				m_std_timezones[84],
				m_std_timezones[110],
				m_std_timezones[116]
		};
		return list;
	}

	/**
	 * Get the Asian timezones sorted by UTC offset.
	 * @return
	 */
	public static TimeZoneInfo[] getAsiaList() {
		TimeZoneInfo[] list = {
				m_std_timezones[131],
				m_std_timezones[68],
				m_std_timezones[43],
				m_dst_timezones[18],
				m_dst_timezones[22],
				m_std_timezones[12],
				m_std_timezones[82],
				m_std_timezones[65],
				m_dst_timezones[1],
				m_std_timezones[8],
				m_std_timezones[52],
				m_std_timezones[16],
				m_std_timezones[56],
				m_std_timezones[4],
				m_dst_timezones[23],
				m_dst_timezones[48],
				m_dst_timezones[8],
				m_std_timezones[10],
				m_std_timezones[85],
				m_std_timezones[96],
				m_std_timezones[102],
				m_std_timezones[124],
				m_std_timezones[127],
				m_std_timezones[133],
				m_std_timezones[150],
				m_std_timezones[66],
				m_std_timezones[90],
				m_std_timezones[6],
				m_std_timezones[22],
				m_std_timezones[21],
				m_std_timezones[70],
				m_std_timezones[89],
				m_std_timezones[95],
				m_std_timezones[110],
				m_dst_timezones[45],
				m_std_timezones[81],
				m_dst_timezones[33],
				m_std_timezones[61],
				m_std_timezones[62],
				m_dst_timezones[31],
				m_std_timezones[143],
				m_std_timezones[72],
				m_dst_timezones[25],
				m_std_timezones[18],
				m_std_timezones[28],
				m_std_timezones[60],
				m_std_timezones[34],
				m_std_timezones[86],
				m_std_timezones[101],
				m_std_timezones[117],
				m_std_timezones[130],
				m_std_timezones[64],
				m_std_timezones[145],
				m_dst_timezones[24],
				m_std_timezones[126],
				m_std_timezones[69],
				m_std_timezones[73],
				m_std_timezones[144],
				m_std_timezones[148],
				m_dst_timezones[44],
				m_std_timezones[112],
				m_std_timezones[77],
				m_dst_timezones[38],
				m_std_timezones[135],
				m_std_timezones[118],
				m_dst_timezones[27],
				m_std_timezones[98],
				m_std_timezones[9]
		};
		return list;
	}

	/**
	 * Get the European timezones sorted by UTC offset.
	 * @return
	 */
	public static TimeZoneInfo[] getEuropeList() {
		TimeZoneInfo[] list = {
				m_std_timezones[131],
				m_std_timezones[55],
				m_std_timezones[140],
				m_dst_timezones[41],
				m_dst_timezones[9],
				m_std_timezones[67],
				m_std_timezones[26],
				m_dst_timezones[12],
				m_std_timezones[43],
				m_dst_timezones[18],
				m_std_timezones[46],
				m_std_timezones[82],
				m_dst_timezones[29],
				m_std_timezones[74],
				m_std_timezones[113]
		};
		return list;
	}

	/**
	 * Get the North American timezones sorted by UTC offset.
	 * @return
	 */
	public static TimeZoneInfo[] getNorthAmericaList() {
		TimeZoneInfo[] list = {
				m_std_timezones[131],
				m_std_timezones[59],
				m_dst_timezones[21],
				m_std_timezones[5],
				m_dst_timezones[4],
				m_std_timezones[105],
				m_dst_timezones[34],
				m_std_timezones[83],
				m_dst_timezones[28],
				m_std_timezones[33],
				m_dst_timezones[11],
				m_std_timezones[45],
				m_dst_timezones[17],
				m_std_timezones[13],
				m_std_timezones[92],
				m_dst_timezones[2],
				m_std_timezones[103],
				m_std_timezones[142],
				m_dst_timezones[30],
				m_dst_timezones[36],
				m_dst_timezones[42],
				m_std_timezones[44],
				m_dst_timezones[19]
		};
		return list;
	}

	/**
	 * Get the Oceania timezones sorted by UTC offset.
	 * @return
	 */
	public static TimeZoneInfo[] getOceaniaList() {
		TimeZoneInfo[] list = {
				m_std_timezones[131],
				m_std_timezones[37],
				m_std_timezones[14],
				m_std_timezones[2],
				m_dst_timezones[6],
				m_std_timezones[0],
				m_std_timezones[3],
				m_dst_timezones[0],
				m_std_timezones[75],
				m_dst_timezones[3],
				m_dst_timezones[26],
				m_std_timezones[88],
				m_std_timezones[94],
				m_dst_timezones[32]
		};
		return list;
	}

	/**
	 * Get the South American timezones sorted by UTC offset.
	 * @return
	 */
	public static TimeZoneInfo[] getSouthAmericaList() {
		TimeZoneInfo[] list = {
				m_std_timezones[131],
				m_std_timezones[1],
				m_std_timezones[32],
				m_std_timezones[42],
				m_std_timezones[97],
				m_std_timezones[134],
				m_std_timezones[19],
				m_std_timezones[31],
				m_std_timezones[48],
				m_std_timezones[58],
				m_std_timezones[108],
				m_std_timezones[7],
				m_dst_timezones[5],
				m_std_timezones[11],
				m_dst_timezones[15],
				m_dst_timezones[20],
				m_std_timezones[53],
				m_dst_timezones[47],
				m_std_timezones[119],
				m_dst_timezones[39],
				m_std_timezones[132],
				m_dst_timezones[37],
				m_dst_timezones[10],
				m_std_timezones[49],
				m_std_timezones[57]
		};
		return list;
	}

	/**
	 * Geth all timezones sorted by UTC offset.
	 * @return
	 */
	public static TimeZoneInfo[] getList() {
		TimeZoneInfo[] list = {
				m_std_timezones[17],
				m_std_timezones[93],
				m_std_timezones[120],
				m_std_timezones[30],
				m_std_timezones[59],
				m_std_timezones[122],
				m_std_timezones[78],
				m_std_timezones[5],
				m_std_timezones[51],
				m_dst_timezones[21],
				m_std_timezones[105],
				m_std_timezones[106],
				m_dst_timezones[4],
				m_std_timezones[83],
				m_dst_timezones[34],
				m_std_timezones[33],
				m_std_timezones[40],
				m_std_timezones[50],
				m_dst_timezones[28],
				m_std_timezones[1],
				m_std_timezones[32],
				m_std_timezones[35],
				m_std_timezones[42],
				m_std_timezones[45],
				m_std_timezones[97],
				m_dst_timezones[11],
				m_dst_timezones[16],
				m_std_timezones[134],
				m_std_timezones[7],
				m_std_timezones[13],
				m_std_timezones[19],
				m_std_timezones[31],
				m_std_timezones[48],
				m_std_timezones[58],
				m_std_timezones[108],
				m_dst_timezones[13],
				m_dst_timezones[17],
				m_std_timezones[92],
				m_std_timezones[11],
				m_std_timezones[20],
				m_std_timezones[53],
				m_std_timezones[103],
				m_dst_timezones[47],
				m_std_timezones[111],
				m_std_timezones[119],
				m_std_timezones[132],
				m_std_timezones[142],
				m_dst_timezones[2],
				m_dst_timezones[5],
				m_dst_timezones[15],
				m_dst_timezones[20],
				m_dst_timezones[39],
				m_dst_timezones[30],
				m_std_timezones[49],
				m_std_timezones[57],
				m_dst_timezones[10],
				m_dst_timezones[36],
				m_dst_timezones[37],
				m_dst_timezones[42],
				m_std_timezones[15],
				m_std_timezones[36],
				m_std_timezones[44],
				m_std_timezones[131],
				m_dst_timezones[7],
				m_std_timezones[55],
				m_std_timezones[140],
				m_std_timezones[147],
				m_dst_timezones[19],
				m_std_timezones[26],
				m_std_timezones[67],
				m_std_timezones[139],
				m_dst_timezones[9],
				m_dst_timezones[41],
				m_dst_timezones[43],
				m_std_timezones[24],
				m_std_timezones[43],
				m_std_timezones[68],
				m_std_timezones[114],
				m_dst_timezones[12],
				m_dst_timezones[40],
				m_std_timezones[12],
				m_std_timezones[41],
				m_std_timezones[46],
				m_std_timezones[82],
				m_std_timezones[121],
				m_dst_timezones[1],
				m_dst_timezones[18],
				m_dst_timezones[22],
				m_std_timezones[65],
				m_std_timezones[8],
				m_std_timezones[16],
				m_std_timezones[52],
				m_std_timezones[56],
				m_std_timezones[74],
				m_std_timezones[84],
				m_std_timezones[110],
				m_std_timezones[113],
				m_std_timezones[116],
				m_dst_timezones[29],
				m_std_timezones[4],
				m_dst_timezones[23],
				m_std_timezones[10],
				m_std_timezones[79],
				m_std_timezones[85],
				m_std_timezones[96],
				m_std_timezones[102],
				m_std_timezones[123],
				m_std_timezones[124],
				m_std_timezones[127],
				m_std_timezones[133],
				m_std_timezones[150],
				m_dst_timezones[8],
				m_dst_timezones[48],
				m_std_timezones[66],
				m_std_timezones[90],
				m_std_timezones[6],
				m_std_timezones[21],
				m_std_timezones[22],
				m_std_timezones[63],
				m_std_timezones[70],
				m_std_timezones[89],
				m_std_timezones[95],
				m_std_timezones[110],
				m_std_timezones[137],
				m_dst_timezones[45],
				m_std_timezones[25],
				m_std_timezones[81],
				m_std_timezones[37],
				m_std_timezones[39],
				m_std_timezones[61],
				m_std_timezones[62],
				m_std_timezones[72],
				m_std_timezones[143],
				m_dst_timezones[31],
				m_dst_timezones[33],
				m_std_timezones[14],
				m_std_timezones[18],
				m_std_timezones[23],
				m_std_timezones[28],
				m_std_timezones[34],
				m_std_timezones[64],
				m_std_timezones[86],
				m_std_timezones[101],
				m_std_timezones[117],
				m_std_timezones[130],
				m_std_timezones[145],
				m_dst_timezones[25],
				m_std_timezones[2],
				m_std_timezones[60],
				m_std_timezones[69],
				m_std_timezones[73],
				m_std_timezones[107],
				m_std_timezones[126],
				m_std_timezones[144],
				m_std_timezones[148],
				m_dst_timezones[6],
				m_dst_timezones[24],
				m_std_timezones[0],
				m_std_timezones[3],
				m_std_timezones[29],
				m_std_timezones[38],
				m_std_timezones[77],
				m_std_timezones[99],
				m_std_timezones[112],
				m_std_timezones[135],
				m_std_timezones[149],
				m_dst_timezones[44],
				m_std_timezones[75],
				m_dst_timezones[0],
				m_std_timezones[71],
				m_std_timezones[87],
				m_std_timezones[104],
				m_std_timezones[115],
				m_std_timezones[118],
				m_std_timezones[137],
				m_dst_timezones[3],
				m_dst_timezones[26],
				m_dst_timezones[38],
				m_std_timezones[88],
				m_std_timezones[9],
				m_std_timezones[47],
				m_std_timezones[54],
				m_std_timezones[80],
				m_std_timezones[91],
				m_std_timezones[94],
				m_std_timezones[98],
				m_std_timezones[129],
				m_std_timezones[138],
				m_std_timezones[141],
				m_dst_timezones[27],
				m_dst_timezones[35],
				m_std_timezones[27],
				m_dst_timezones[46],
				m_std_timezones[100],
				m_std_timezones[125],
				m_std_timezones[128],
				m_std_timezones[146],
				m_dst_timezones[32],
				m_dst_timezones[14],
				m_std_timezones[76]
		};
		return list;
	}
	
	/**
	 * Get a timezone from its code.
	 * @param code the timezones code
	 * @return the timezone with the given code, null if one is not found
	 */
	public static final TimeZoneInfo getTimeZoneFromCode(String code) {
		for(TimeZoneInfo tzi : getList())
			if(tzi.getCode().equals(code))
				return tzi;
		return null;
	}
	
	public static final TimeZoneInfo getTimeZoneFromName(String name, int set) {
		TimeZoneInfo[] tz;
		if (set == -1)
			tz = m_mil_timezones;
		else if (set == 0)
			tz = m_std_timezones;
		else
			tz = m_dst_timezones;
		
		for (TimeZoneInfo info : tz) {
			if (info.getCode().equalsIgnoreCase(name) || info.getName().equalsIgnoreCase(name))
				return info;
		}
		return null;
	}
	
	public static final TimeZoneInfo getTimeZoneFromId(int id) {
	    TimeZoneInfo[] arr;
	    if (IS_STD(id))
	        arr = m_std_timezones;
	    else if (IS_DST(id))
	        arr = m_dst_timezones;
	    else
	        arr = m_mil_timezones;
        Optional<TimeZoneInfo> info = Arrays.stream(arr).filter(x -> x.getUUID() == id).findFirst();
        if (info.isPresent())
            return info.get();
        else
            return null;
	}

	/**
	 * Get a timezone from its offset from UTC.
	 * @param offset the timezones offset from UTC in hours
	 * @return the timezone with the given offset, null if one is not found
	 */
	public static final TimeZoneInfo getTimeZoneFromOffset(int offset) {
		for (TimeZoneInfo tzi : m_std_timezones)
			if (tzi.getTimezoneOffset().getHours() == offset)
				return tzi;
		return null;
	}

	/**
	 * Get all LST timezones in no order.
	 * @return
	 */
	public static final ArrayList<String> getStandardTimeZones() {
		ArrayList<String> list = new ArrayList<String>(m_std_timezones.length);
		for (int i=0; i < m_std_timezones.length; i++) list.add(i, m_std_timezones[i].toString());
		return list;
	}

	/**
	 * Get all DST timezones in no order.
	 * @return
	 */
	public static final ArrayList<String> getDaylightSavingsTimeZones() {
		ArrayList<String> list = new ArrayList<String>(m_dst_timezones.length);
		for (int i=0; i < m_dst_timezones.length; i++) list.add(i, m_dst_timezones[i].toString());
		return list;
	}

	WTimeSpan m_solar_timezone(WTime solar_time) {
							// used for GMT->solar time math based on longitude only, but don't assume that solar noon means the sun is right over you!

		int	day = (int)solar_time.getDay(WTime.FORMAT_AS_LOCAL),
			year = (int)solar_time.getYear(WTime.FORMAT_AS_LOCAL),
			month = (int)solar_time.getMonth(WTime.FORMAT_AS_LOCAL);

		RiseSetInput sInput = new RiseSetInput();
		sInput.latitude = RADIAN_TO_DEGREE((float)m_Latitude);
		sInput.longitude = RADIAN_TO_DEGREE((float)m_Longitude) * -1;
		sInput.timezone = 0;
		sInput.daytimeSaving = false;
		sInput.year = year;
		sInput.month = month;
		sInput.day = day;
		RiseSetOut sOut = new RiseSetOut();
		SunriseSunsetCalc.calcSun(sInput,sOut);

		WTimeSpan solarTime = new WTimeSpan(0, sOut.solarNoonHour - 12, sOut.solarNoonMin, sOut.solarNoonSec);
		WTimeSpan result = new WTimeSpan((long)0 - solarTime.getTotalSeconds());

		return result;
	}

	/**
	 * Get the sunrise, sunset and solar noon values at the current location for a given date.
	 * @param daytime the date to find the solar values for
	 * @param Rise the sunrise time
	 * @param Set the sunset time
	 * @param Noon the solar noon (when the sun is at its highest above the horizon)
	 * @return 0 if all times exist, SunriseSunsetCalc.NO_SUNRISE if there is no sunrise on the given date,
	 * SunriseSunsetCalc.NO_SUNSET if there is no sunset on the given date
	 */
	public int getSunRiseSetNoon(WTime daytime, OutVariable<WTime> Rise, OutVariable<WTime> Set, OutVariable<WTime> Noon) {

		int day = (int)daytime.getDay(WTime.FORMAT_AS_LOCAL);
		int year = (int)daytime.getYear(WTime.FORMAT_AS_LOCAL);
		int month = (int)daytime.getMonth(WTime.FORMAT_AS_LOCAL);

		RiseSetInput sInput = new RiseSetInput();
		sInput.latitude = RADIAN_TO_DEGREE(m_Latitude);
		sInput.longitude = -1 * RADIAN_TO_DEGREE(m_Longitude);
		sInput.timezone = 0;
		sInput.daytimeSaving = false;
		sInput.year = year;//solar_time.GetYear();
		sInput.month = month;//solar_time.GetMonth();
		sInput.day = day;//solar_time.GetDay();
		RiseSetOut sOut = new RiseSetOut();
		int success = SunriseSunsetCalc.calcSun(sInput,sOut);

		WTime riseTime = new WTime(sOut.yearRise,sOut.monthRise,sOut.dayRise,sOut.hourRise,sOut.minRise,sOut.secRise,Rise.value.getTimeManager());
		Rise.value = riseTime;
		WTime setTime = new WTime(sOut.yearSet,sOut.monthSet,sOut.daySet,sOut.hourSet,sOut.minSet,sOut.secSet,Set.value.getTimeManager());
		Set.value = setTime;
		WTime noonTime = new WTime(sInput.year,sInput.month,sInput.day, sOut.solarNoonHour, sOut.solarNoonMin, sOut.solarNoonSec,Noon.value.getTimeManager());
		Noon.value = noonTime;

		return success;
	}

	/**
	 * Default constructor.
	 */
	public WorldLocation() {
		m_Latitude = 1000.0;
		m_Longitude = 1000.0;
		m_TimeZone = new WTimeSpan(0);
		m_StartDST = new WTimeSpan(0);
		m_EndDST = new WTimeSpan(0);
		m_AmtDST = new WTimeSpan(0, 1, 0, 0);
	}

	/**
	 * Copy constructor
	 * @param wl the world location to copy
	 */
	public WorldLocation(WorldLocation wl) {
		m_Latitude = wl.m_Latitude;
		m_Longitude = wl.m_Longitude;
		m_TimeZone = wl.m_TimeZone;
		m_StartDST = wl.m_StartDST;
		m_EndDST = wl.m_EndDST;
		m_AmtDST = wl.m_AmtDST;
	}

	/**
	 * Make a rough estimate of the locations timezone based on its latitude and longitude.
	 * @param set 0 if LST timezones should be used, 1 if DST timezones should be used
	 * @return
	 */
	public TimeZoneInfo guessTimeZone(short set) {
		if (insideNewZealand()) {
			if (set == 0)
				return m_std_timezones[15];
			else if (set == 1)
				return m_dst_timezones[15];
		}
		else if (insideTasmania()) {
			if (set == 0)
				return m_std_timezones[3];
			else
				return m_dst_timezones[0];
		}

		double longitude = m_Longitude;
		while (longitude < (-Math.PI))
			longitude += Math.PI*2;
		while (longitude > Math.PI)
			longitude -= Math.PI*2;

		TimeZoneInfo[] tz;
		double variation = Math.PI*2;
		if (set == 1)		tz = m_dst_timezones;
		else if (set == -1)	tz = m_mil_timezones;
		else if (set == 0)	tz = m_std_timezones;
		else			return null;

		int i=0;
		TimeZoneInfo tzi = tz[0];
		do
		{
			double ideal_longitude = ((double)tz[i].getTimezoneOffset().getTotalSeconds()) / (double)(12.0 * 60.0 * 60.0) * Math.PI;
			double offset_longitude = Math.abs(longitude - ideal_longitude);
			if (variation > offset_longitude) {
				variation = offset_longitude;
				tzi= tz[i++];
			}
			else i++;
		}
		while (i < tz.length);

		return tzi;
	}

	/**
	 * Return the current timezone.
	 * @param set
	 * @return
	 */
	public TimeZoneInfo currentTimeZone(short set) {
	    if (m_timezone != null) {
	        return m_timezone;
	    }
	    else {
    		TimeZoneInfo[] tz;
    		if (set == -1)	tz = m_mil_timezones;
    		else if (WTimeSpan.equal(m_StartDST, m_EndDST))	tz = m_std_timezones;
    		else					tz = m_dst_timezones;
    
    		int i=0;
    		do
    		{
    			if (WTimeSpan.equal(tz[i].getTimezoneOffset(), m_TimeZone))
    				break;
    		}
    		while (tz[++i].getCode() != null);
    
    		if (tz[i].getCode() != null)
    			return tz[i];
	    }

		return null;
	}

	boolean insideCanada(double latitude, double longitude) {
		if (latitude < DEGREE_TO_RADIAN((float)42.0))		return false;
		if (latitude > DEGREE_TO_RADIAN((float)83.0))		return false;
		if (longitude < DEGREE_TO_RADIAN((float)-141.0))	return false;
		if (longitude > DEGREE_TO_RADIAN((float)-52.0))		return false;
		return true;
	}

	boolean insideCanada() {
		return insideCanada(m_Latitude, m_Longitude);
	}

	boolean insideNewZealand() {
		if ((m_Longitude > DEGREE_TO_RADIAN((float)172.5)) && (m_Longitude < DEGREE_TO_RADIAN((float)178.6))) {
			if ((m_Latitude > DEGREE_TO_RADIAN((float)-41.75)) && (m_Latitude < DEGREE_TO_RADIAN((float)-34.3))) {	// general extents of New Zealand's north island
				return true;
			}
		}

		if ((m_Longitude > DEGREE_TO_RADIAN((float)166.3)) && (m_Longitude < DEGREE_TO_RADIAN((float)174.5))) {
			if ((m_Latitude > DEGREE_TO_RADIAN((float)-47.35)) && (m_Latitude < DEGREE_TO_RADIAN((float)40.4))) {	// general extents of New Zealand's south island
				return true;
			}
		}
		return false;
	}

	boolean insideTasmania() {
		if ((m_Longitude > DEGREE_TO_RADIAN(143.5)) && (m_Longitude < DEGREE_TO_RADIAN(149.0))) {
			if ((m_Latitude > DEGREE_TO_RADIAN(-44.0)) && (m_Latitude < DEGREE_TO_RADIAN(-39.5))) {
				return true;
			}
		}
		return false;
	}

	boolean insideAustraliaMainland() {
		if ((m_Longitude > DEGREE_TO_RADIAN(113.15)) && (m_Longitude < DEGREE_TO_RADIAN(153.6333333))) {
			if ((m_Latitude > DEGREE_TO_RADIAN(-39.133333)) && (m_Latitude < DEGREE_TO_RADIAN(-10.683333333))) {
				return true;
			}
		}
		return false;
	}
}
