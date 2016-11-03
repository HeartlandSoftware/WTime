/**
 * WorldLocation.java
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

import java.io.Serializable;
import java.util.ArrayList;

import ca.hss.annotations.Source;

import ca.hss.general.OutVariable;

import static ca.hss.math.general.*;

/**
 * Stores information about a geographical area including its coordinates
 * and its timezone information.
 */
@Source(project="Times", sourceFile="worldlocation.cpp")
public class WorldLocation implements Serializable {
	public static final long serialVersionUID = 4;

	double		m_latitude;			// stored as radians!!!
	double		m_longitude;
	WTimeSpan	m_timezone;			// time zone
	WTimeSpan	m_startDST;			// when daylight savings turns on - from start of year
	WTimeSpan	m_endDST;			// when daylight savings turns off - from start of year
											// if you want to disable DST, then just set these to the same value
	WTimeSpan	m_amtDST;			// amount to adjust for DST
	
	/**
	 * Get the locations latitude in radians.
	 * @return
	 */
	public double getLatitude() { return m_latitude; }
	/**
	 * Set the locations latitude in radians.
	 * @param latitude
	 */
	public void setLatitude(double latitude) { m_latitude = latitude; }
	/**
	 * Get the locations longitude in radians.
	 * @return
	 */
	public double getLongitude() { return m_longitude; }
	/**
	 * Set the locations longitude in radians.
	 */
	public void setLongitude(double longitude) { m_longitude = longitude; }
	/**
	 * Get the locations timezone offset.
	 * @return
	 */
	public WTimeSpan getTimezoneOffset() { return m_timezone; }
	/**
	 * Set the locations timezone offset.
	 * @param offset
	 */
	public void setTimezoneOffset(WTimeSpan offset) { m_timezone = offset; }
	/**
	 * Get the locations starting DST offset from January 1st 0:00.
	 * @return
	 */
	public WTimeSpan getStartDST() { return m_startDST; }
	/**
	 * Set the locations starting DST offset from January 1st 0:00.
	 * @param start
	 */
	public void setStartDST(WTimeSpan start) { m_startDST = start; }
	/**
	 * Get the locations ending DST offset from January 1st 0:00.
	 * @return
	 */
	public WTimeSpan getEndDST() { return m_endDST; }
	/**
	 * Set the locations ending DST offset from January 1st 0:00.
	 * @param end
	 */
	public void setEndDST(WTimeSpan end) { m_endDST = end; }
	/**
	 * Get the amount of offset from DST.
	 * @return
	 */
	public WTimeSpan getDSTAmount() { return m_amtDST; }
	/**
	 * Set the amount of offset from DST.
	 * @param amount
	 */
	public void setDSTAmount(WTimeSpan amount) { m_amtDST = amount; }

	protected static final TimeZoneInfo[] m_dst_timezones = {
		new TimeZoneInfo(new WTimeSpan(0, +9, 30, 0), new WTimeSpan(0, 1, 0, 0), "ACDT", "Australian Central Daylight Time", "ui.label.zone.acdt"),//australia//0
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0, 1, 0, 0), "ADT", "Arabia Daylight Time", "ui.label.zone.ardt"),//asia//1
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "ADT", "Atlantic Daylight Time", "ui.label.zone.adt"),//north america//2
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0, 1, 0, 0), "AEDT", "Australian Eastern Daylight Time", "ui.label.zone.aedt"),//australia//3
		new TimeZoneInfo(new WTimeSpan(0, -9, 00, 0), new WTimeSpan(0, 1, 0, 0), "AKDT", "Alaska Daylight Time", "ui.label.zone.akdt"),//north america//4
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "AMST", "Amazon Summer Time", "ui.label.zone.amst"),//south america//5
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0, 1, 0, 0), "AWDT", "Australian Western Daylight Time", "ui.label.zone.awdt"),//australia//6
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0, 1, 0, 0), "AZOST", "Azores Summer Time", "ui.label.zone.azost"),//atlantic//7
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0, 1, 0, 0), "AZST", "Azerbaijan Summer Time", "ui.label.zone.azst"),//asia//8
		new TimeZoneInfo(new WTimeSpan(0, +0, 00, 0), new WTimeSpan(0, 1, 0, 0), "BST", "British Summer Time", "ui.label.zone.bst"),//europe//9
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0, 1, 0, 0), "BRST", "Bras�lia Summer Time ", "ui.label.zone.brst"),//south america//10
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0, 1, 0, 0), "CDT", "Central Daylight Time", "ui.label.zone.cdt"),//north america//11
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0, 1, 0, 0), "CEST", "Central European Summer Time", "ui.label.zone.cest"),//europe//12
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "CDT", "Cuba Daylight Time", "ui.label.zone.cudt"),//caribbean//13
		new TimeZoneInfo(new WTimeSpan(0, +12, 45, 0), new WTimeSpan(0, 1, 0, 0), "CHADT", "Chatham Island Daylight Time", "ui.label.zone.chadt"),//pacific//14
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "CLST", "Chile Summer Time", "ui.label.zone.clst"),//south america//15
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0, 1, 0, 0), "EASST", "Easter Island Summer Time", "ui.label.zone.easst"),//pacific//16
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0, 1, 0, 0), "EDT", "Eastern Daylight Time", "ui.label.zone.edt"),//north america//17
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0, 1, 0, 0), "EEST", "Eastern European Summer Time", "ui.label.zone.eest"),//europe//18
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0, 1, 0, 0), "EGST", "Eastern Greenland Summer Time", "ui.label.zone.egst"),//north america//19
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "FKST", "Falkland Islands Summer Time", "ui.label.zone.fkst"),//south america//20
		new TimeZoneInfo(new WTimeSpan(0, -10, 00, 0), new WTimeSpan(0, 1, 0, 0), "HADT", "Hawaii-Aleutien Daylight Time", "ui.label.zone.hadt"),//north america//21
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0, 1, 0, 0), "IDT", "Israel Daylight Time", "ui.label.zone.idt"),//asia//22
		new TimeZoneInfo(new WTimeSpan(0, +3, 30, 0), new WTimeSpan(0, 1, 0, 0), "IRDT", "Iran Daylight Time", "ui.label.zone.irdt"),//asia//23
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0, 1, 0, 0), "IRKST", "Irkutsk Summer Time", "ui.label.zone.irkst"),//asia//24
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0, 1, 0, 0), "KRAST", "Krasnoyarsk Summer Time", "ui.label.zone.krast"),//asia//25
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0, 1, 0, 0), "LHDT", "Lord Howe Daylight Time", "ui.label.zone.lhdt"),//australia//26
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0, 1, 0, 0), "MAGST", "Magadan Summer Time", "ui.label.zone.magst"),//asia//27
		new TimeZoneInfo(new WTimeSpan(0, -7, 00, 0), new WTimeSpan(0, 1, 0, 0), "MDT", "Mountain Daylight Time", "ui.label.zone.mdt"),//north america//28
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0, 1, 0, 0), "MSD", "Moscow Daylight Time", "ui.label.zone.msd"),//europe//29
		new TimeZoneInfo(new WTimeSpan(0, -3, -30, 0), new WTimeSpan(0, 1, 0, 0), "NDT", "Newfoundland Daylight Time", "ui.label.zone.ndt"),//north america//30
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0, 1, 0, 0), "NOVST", "Novosibirsk Summer Time", "ui.label.zone.novst"),//asia//31
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0, 1, 0, 0), "NZDT", "New Zealand Daylight Time", "ui.label.zone.nzdt"),//pacific//32
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0, 1, 0, 0), "OMSST", "Omsk Summer Time", "ui.label.zone.omsst"),//asia//33
		new TimeZoneInfo(new WTimeSpan(0, -8, 00, 0), new WTimeSpan(0, 1, 0, 0), "PDT", "Pacific Daylight Time", "ui.label.zone.pdt"),//north america//34
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0, 1, 0, 0), "PETST", "Kamchatka Summer Time", "ui.label.zone.petst"),//asia//35
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0, 1, 0, 0), "PMDT", "Pierre & Miquelon Daylight Time", "ui.label.zone.pmdt"),//north america//36
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0, 1, 0, 0), "UYST", "Uruguay Summer Time", "ui.label.zone.uyst"),//south america//37
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0, 1, 0, 0), "VLAST", "Vladivostok Summer Time", "ui.label.zone.vlast"),//asia//38
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "WARST", "Western Argentine Summer Time", "ui.label.zone.warst"),//south america//39
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0, 1, 0, 0), "WAST", "West Africa Summer Time", "ui.label.zone.wast"),//africa//40
		new TimeZoneInfo(new WTimeSpan(0, 0, 00, 0), new WTimeSpan(0, 1, 0, 0), "WEST", "Western European Summer Time", "ui.label.zone.west"),//europe//41
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0, 1, 0, 0), "WGST", "Western Greenland Summer Time", "ui.label.zone.wgst"),//north america//42
		new TimeZoneInfo(new WTimeSpan(0, 0, 00, 0), new WTimeSpan(0, 1, 0, 0), "WST", "Western Sahara Summer Time", "ui.label.zone.wst"),//africa//43
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0, 1, 0, 0), "YAKST", "Yakutsk Summer Time", "ui.label.zone.yakst"),//asia//44
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0, 1, 0, 0), "YEKST", "Yekaterinburg Summer Time", "ui.label.zone.yekst"),//asia//45
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0, 1, 0, 0), "FJST", "Fiji Summer Time", "ui.label.zone.fjst"),//pacific//46
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0, 1, 0, 0), "PYST", "Paraguay Summer Time", "ui.label.zone.pyst"),//south america//47
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0, 1, 0, 0), "AMST", "Armenia Summer Time", "ui.label.zone.armst"),//asia//48
	};

	protected static final TimeZoneInfo[] m_std_timezones = {
		//a
		new TimeZoneInfo(new WTimeSpan(0, +9, 30, 0), new WTimeSpan(0), "ACST", "Australian Central Standard Time", "ui.label.zone.acst"),//australia//0
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "ACT", "Acre Time", "ui.label.zone.act"),//south america//1
		new TimeZoneInfo(new WTimeSpan(0, +8, 45, 0), new WTimeSpan(0), "ACWST", "Australian Central Western Standard Time", "ui.label.zone.acwst"),//australia//2
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "AEST", "Australian Eastern Standard Time", "ui.label.zone.aest"),//australia//3
		new TimeZoneInfo(new WTimeSpan(0, +4, 30, 0), new WTimeSpan(0), "AFT", "Afghanistan Time", "ui.label.zone.aft"),//asia//4
		new TimeZoneInfo(new WTimeSpan(0, -9, 00, 0), new WTimeSpan(0), "AKST", "Alaska Standard Time", "ui.label.zone.akst"),//north america//5
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "ALMT", "Alma-Ata Time", "ui.label.zone.almt"),//asia//6
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "AMT", "Amazon Time", "ui.label.zone.amt"),//south america//7
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "AMT", "Armenia Time", "ui.label.zone.armt"),//asia//8
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "ANAT", "Anadyr Time", "ui.label.zone.anat"),//asia//9
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "AQTT", "Aqtobe Time", "ui.label.zone.aqtt"),//asia//10
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "ART", "Argentina Time", "ui.label.zone.art"),//south america//11
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "AST", "Arabia Standard Time", "ui.label.zone.arst"),//asia//12
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "AST", "Atlantic Standard Time", "ui.label.zone.ast"),//north america//13
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "AWST", "Australian Western Standard Time", "ui.label.zone.awst"),//australia//14
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0), "AZOT", "Azores Time", "ui.label.zone.azot"),//atlantic//15
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "AZT", "Azerbaijan Time", "ui.label.zone.azt"),//asia//16
		new TimeZoneInfo(new WTimeSpan(0, -12, 00, 0), new WTimeSpan(0), "AoE", "Anywhere on Earth", "ui.label.zone.aoe"),//pacific//17
		//b
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "BNT", "Brunei Darussalam Time", "ui.label.zone.bnt"),//asia//18
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "BOT", "Bolivia Time", "ui.label.zone.bot"),//south america//19
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "BRT", "Bras�lia Time", "ui.label.zone.brt"),//south america//20
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "BST", "Bangladesh Standard Time", "ui.label.zone.bast"),//asia//21
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "BTT", "Bhutan Time", "ui.label.zone.btt"),//asia//22
		//c
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "CAST", "Casey Time", "ui.label.zone.cast"),//antarctica//23
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0), "CAT", "Central Africa Time", "ui.label.zone.cat"),//africa//24
		new TimeZoneInfo(new WTimeSpan(0, +6, 30, 0), new WTimeSpan(0), "CCT", "Cocos Islands Time", "ui.label.zone.cct"),//indian ocean//25
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0), "CET", "Central European Time", "ui.label.zone.cet"),//europe//26
		new TimeZoneInfo(new WTimeSpan(0, +12, 45, 0), new WTimeSpan(0), "CHAST", "Chatham Island Standard Time", "ui.label.zone.chast"),//pacific//27
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "CHOT", "Choibalsan Time", "ui.label.zone.chot"),//asia//28
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "CHUT", "Chuuk Time", "ui.label.zone.chut"),//pacific//29
		new TimeZoneInfo(new WTimeSpan(0, -10, 00, 0), new WTimeSpan(0), "CKT", "Cook Island Time", "ui.label.zone.ckt"),//pacific//30
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "CLT", "Chile Standard Time", "ui.label.zone.clt"),//south america//31
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "COT", "Colombia Time", "ui.label.zone.cot"),//south america//32
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0), "CST", "Central Standard Time", "ui.label.zone.cst"),//north america//33
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "CST", "China Standard Time", "ui.label.zone.chst"),//asia//34
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "CST", "Cuba Standard Time", "ui.label.zone.cust"),//caribbean//35
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0), "CVT", "Cape Verde Time", "ui.label.zone.cvt"),//africa//36
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "CXT", "Christmas Island Time", "ui.label.zone.cxt"),//australia//37
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "ChST", "Chamorro Standard Time", "ui.label.zone.chst"),//pacific//38
		//d
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "DAVT", "Davis Time", "ui.label.zone.davt"),//antarctica//39
		//e
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0), "EAST", "Easter Island Standard Time", "ui.label.zone.east"),//pacific//40
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "EAT", "Eastern Africa Time", "ui.label.zone.eat"),//africa//41
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "ECT", "Ecuador Time", "ui.label.zone.ect"),//south america//42
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0), "EET", "Eastern European Time", "ui.label.zone.eet"),//europe//43
		new TimeZoneInfo(new WTimeSpan(0, -1, 00, 0), new WTimeSpan(0), "EGT", "East Greenland Time", "ui.label.zone.egt"),//north america//44
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "EST", "Eastern Standard Time", "ui.label.zone.est"),//north america//45
		//f
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "FET", "Further-Eastern European Time", "ui.label.zone.fet"),//europe//46
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "FJT", "Fiji Time", "ui.label.zone.fjt"),//pacific//47
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "FKT", "Falkland Island Time", "ui.label.zone.fkt"),//south america//48
		new TimeZoneInfo(new WTimeSpan(0, -2, 00, 0), new WTimeSpan(0), "FNT", "Fernando de Noronha Time", "ui.label.zone.fnt"),//south america//49
		//g
		new TimeZoneInfo(new WTimeSpan(0, -6, 00, 0), new WTimeSpan(0), "GALT", "Galapagos Time", "ui.label.zone.galt"),//pacific//50
		new TimeZoneInfo(new WTimeSpan(0, -9, 00, 0), new WTimeSpan(0), "GAMT", "Gambier Time", "ui.label.zone.gamt"),//pacific//51
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "GET", "Georgia Standard Time", "ui.label.zone.get"),//asia//52
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "GFT", "French Guiana Time", "ui.label.zone.gft"),//south america//53
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "GILT", "Gilbert Island Time", "ui.label.zone.gilt"),//pacific//54
		new TimeZoneInfo(new WTimeSpan(0, +0, 00, 0), new WTimeSpan(0), "GMT", "Greenwich Mean Time", "ui.label.zone.gmt"),//europe//55
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "GST", "Gulf Standard Time", "ui.label.zone.gust"),//asia//56
		new TimeZoneInfo(new WTimeSpan(0, -2, 00, 0), new WTimeSpan(0), "GST", "South Georgia Time", "ui.label.zone.gst"),//south america//57
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "GYT", "Guyana Time", "ui.label.zone.gyt"),//south america//58
		//h
		new TimeZoneInfo(new WTimeSpan(0, -10, 00, 0), new WTimeSpan(0), "HAST", "Hawaii-Aleutien Standard Time", "ui.label.zone.hast"),//north america//59
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "HKT", "Hong Kong Time", "ui.label.zone.hkt"),//asia//60
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "HOVT", "Hovd Time", "ui.label.zone.hovt"),//asia//61
		//i
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "ICT", "Indochina Time", "ui.label.zone.ict"),//asia//62
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "IOT", "Indian Chagos Time", "ui.label.zone.iot"),//indian ocean//63
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "IRKT", "Irkutsk Time", "ui.label.zone.irkt"),//asia//64
		new TimeZoneInfo(new WTimeSpan(0, +3, 30, 0), new WTimeSpan(0), "IRST", "Iran Standard Time", "ui.label.zone.irst"),//asia//65
		new TimeZoneInfo(new WTimeSpan(0, +5, 30, 0), new WTimeSpan(0), "IST", "India Standard Time", "ui.label.zone.inst"),//asia//66
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0), "IST", "Irish Standard Time", "ui.label.zone.ist"),//europe//67
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0), "IST", "Israel Standard Time", "ui.label.zone.isst"),//asia//68
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "JST", "Japan Standard Time", "ui.label.zone.jst"),//asia//69
		//k
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "KGT", "Kyrgyzstan Time", "ui.label.zone.kgt"),//asia//70
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "KOST", "Kosrae Time", "ui.label.zone.kost"),//pacific//71
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "KRAT", "Krasnoyarsk Time", "ui.label.zone.krat"),//asia//72
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "KST", "Korea Standard Time", "ui.label.zone.kst"),//asia//73
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "KUYT", "Kuybyshev Time", "ui.label.zone.kuyt"),//europe//74
		//l
		new TimeZoneInfo(new WTimeSpan(0, +10, 30, 0), new WTimeSpan(0), "LHST", "Lord Howe Standard Time", "ui.label.zone.lhst"),//australia//75
		new TimeZoneInfo(new WTimeSpan(0, +14, 00, 0), new WTimeSpan(0), "LINT", "Line Islands Time", "ui.label.zone.lint"),//pacific//76
		//m
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "MAGT", "Magadan Time", "ui.label.zone.magt"),//asia//77
		new TimeZoneInfo(new WTimeSpan(0, -9, -30, 0), new WTimeSpan(0), "MART", "Marquesas Time", "ui.label.zone.mart"),//pacific//78
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "MAWT", "Mawson Time", "ui.label.zone.mawt"),//antarctica//79
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "MHT", "Marshall Islands Time", "ui.label.zone.mht"),//pacific//80
		new TimeZoneInfo(new WTimeSpan(0, +6, 30, 0), new WTimeSpan(0), "MMT", "Myanmar Time", "ui.label.zone.mmt"),//asia//81
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "MSK", "Moscow Standard Time", "ui.label.zone.msk"),//europe//82
		new TimeZoneInfo(new WTimeSpan(0, -7, 00, 0), new WTimeSpan(0), "MST", "Mountain Standard Time", "ui.label.zone.mst"),//north america//83
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "MUT", "Mauritius Time", "ui.label.zone.mut"),//africa//84
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "MVT", "Maldives Time", "ui.label.zone.mvt"),//asia//85
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "MYT", "Malaysia Time", "ui.label.zone.myt"),//asia//86
		//n
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "NCT", "New Caledonia Time", "ui.label.zone.nct"),//pacific//87
		new TimeZoneInfo(new WTimeSpan(0, +11, 30, 0), new WTimeSpan(0), "NFT", "Norfolk Time", "ui.label.zone.nft"),//australia//88
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "NOVT", "Novosibirsk Time", "ui.label.zone.novt"),//asia//89
		new TimeZoneInfo(new WTimeSpan(0, +5, 45, 0), new WTimeSpan(0), "NPT", "Nepal Time", "ui.label.zone.npt"),//asia//90
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "NRT", "Nauru Time", "ui.label.zone.nrt"),//pacific//91
		new TimeZoneInfo(new WTimeSpan(0, -3, -30, 0), new WTimeSpan(0), "NST", "Newfoundland Standard Time", "ui.label.zone.nst"),//north america//92
		new TimeZoneInfo(new WTimeSpan(0, -11, 00, 0), new WTimeSpan(0), "NUT", "Niue Time", "ui.label.zone.nut"),//pacific//93
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "NZST", "New Zealand Standard Time", "ui.label.zone.nzst"),//pacific//94
		//o
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "OMST", "Omsk Standard Time", "ui.label.zone.omst"),//asia//95
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "ORAT", "Oral Time", "ui.label.zone.orat"),//asia//96
		//p
		new TimeZoneInfo(new WTimeSpan(0, -5, 00, 0), new WTimeSpan(0), "PET", "Peru Time", "ui.label.zone.pet"),//south america//97
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "PETT", "Kamchatka Time", "ui.label.zone.pett"),//asia//98
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "PGT", "Papua New Guinea Time", "ui.label.zone.pgt"),//pacific//99
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0), "PHOT", "Phoenix Island Time", "ui.label.zone.phot"),//pacific//100
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "PHT", "Philippine Time", "ui.label.zone.pht"),//asia//101
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "PKT", "Pakistan Standard Time", "ui.label.zone.pkt"),//asia//102
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "PMST", "Pierre & Miquelon Standard Time", "ui.label.zone.pmst"),//north america//103
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "PONT", "Pohnpei Standard Time", "ui.label.zone.pont"),//pacific//104
		new TimeZoneInfo(new WTimeSpan(0, -8, 00, 0), new WTimeSpan(0), "PST", "Pacific Standard Time", "ui.label.zone.pst"),//north america//105
		new TimeZoneInfo(new WTimeSpan(0, -8, 00, 0), new WTimeSpan(0), "PST", "Pitcairn Standard Time", "ui.label.zone.pcst"),//pacific//106
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "PWT", "Palau Time", "ui.label.zone.pwt"),//pacific//107
		new TimeZoneInfo(new WTimeSpan(0, -4, 00, 0), new WTimeSpan(0), "PYT", "Paraguay Time", "ui.label.zone.pyt"),//south america//108
		//q
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "QYZT", "Qyzylorda Time", "ui.label.zone.qyzt"),//asia//109
		//r
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "RET", "Reunion Time", "ui.label.zone.ret"),//africa//110
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "ROTT", "Rothera Time", "ui.label.zone.rott"),//antarctica//111
		//s
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "SAKT", "Sakhalin Time", "ui.label.zone.sakt"),//asia//112
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "SAMT", "Samara Time", "ui.label.zone.samt"),//europe//113
		new TimeZoneInfo(new WTimeSpan(0, +2, 00, 0), new WTimeSpan(0), "SAST", "South Africa Standard Time", "ui.label.zone.sast"),//africa//114
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "SBT", "Solomon Islands Time", "ui.label.zone.sbt"),//pacific//115
		new TimeZoneInfo(new WTimeSpan(0, +4, 00, 0), new WTimeSpan(0), "SCT", "Seychelles Time", "ui.label.zone.sct"),//africa//116
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "SGT", "Singapore Time", "ui.label.zone.sgt"),//asia//117
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "SRET", "Srednekolymsk Time", "ui.label.zone.sret"),//asia//118
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "SRT", "Suriname Time", "ui.label.zone.srt"),//south america//119
		new TimeZoneInfo(new WTimeSpan(0, -11, 00, 0), new WTimeSpan(0), "SST", "Samoa Standard Time", "ui.label.zone.sst"),//pacific//120
		new TimeZoneInfo(new WTimeSpan(0, +3, 00, 0), new WTimeSpan(0), "SYOT", "Syowa Time", "ui.label.zone.syot"),//antarctica//121
		//t
		new TimeZoneInfo(new WTimeSpan(0, -10, 00, 0), new WTimeSpan(0), "TAHT", "Tahiti Time", "ui.label.zone.taht"),//pacific//122
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "TFT", "French Southern and Antarctic Time", "ui.label.zone.tft"),//indian ocean//123
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "TJT", "Tajikistan Time", "ui.label.zone.tjt"),//asia//124
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0), "TKT", "Tokelau Time", "ui.label.zone.tkt"),//pacific//125
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "TLT", "East Timor Time", "ui.label.zone.tlt"),//asia//126
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "TMT", "Turkmenistan Time", "ui.label.zone.tmt"),//asia//127
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0), "TOT", "Tonga Time", "ui.label.zone.tot"),//pacific//128
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "TVT", "Tuvalu Time", "ui.label.zone.tvt"),//pacific//129
		//u
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "ULAT", "Ulaanbaatar Time", "ui.label.zone.ulat"),//asia//130
		new TimeZoneInfo(new WTimeSpan(0, 0, 00, 0), new WTimeSpan(0), "UTC", "Coordinated Universal Time", "ui.label.zone.utc"),//worldwide//131
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "UYT", "Uruguay Time", "ui.label.zone.uyt"),//south america//132
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "UZT", "Uzbekistan Time", "ui.label.zone.uzt"),//asia//133
		//v
		new TimeZoneInfo(new WTimeSpan(0, -4, -30, 0), new WTimeSpan(0), "VET", "Venezuelan Standard Time", "ui.label.zone.vet"),//south america//134
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "VLAT", "Vladivostok Time", "ui.label.zone.vlat"),//asia//135
		new TimeZoneInfo(new WTimeSpan(0, +6, 00, 0), new WTimeSpan(0), "VOST", "Vostok Time", "ui.label.zone.vost"),//antarctica//136
		new TimeZoneInfo(new WTimeSpan(0, +11, 00, 0), new WTimeSpan(0), "VUT", "Vanuatu Time", "ui.label.zone.vut"),//pacific//137
		//w
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "WAKT", "Wake Time", "ui.label.zone.wakt"),//pacific//138
		new TimeZoneInfo(new WTimeSpan(0, +1, 00, 0), new WTimeSpan(0), "WAT", "West Africa Time", "ui.label.zone.wat"),//africa//139
		new TimeZoneInfo(new WTimeSpan(0, +0, 00, 0), new WTimeSpan(0), "WET", "Western European Time", "ui.label.zone.wet"),//europe//140
		new TimeZoneInfo(new WTimeSpan(0, +12, 00, 0), new WTimeSpan(0), "WFT", "Wallis and Futuna Time", "ui.label.zone.wft"),//pacific//141
		new TimeZoneInfo(new WTimeSpan(0, -3, 00, 0), new WTimeSpan(0), "WGT", "West Greenland Time", "ui.label.zone.wgt"),//north america//142
		new TimeZoneInfo(new WTimeSpan(0, +7, 00, 0), new WTimeSpan(0), "WIB", "Western Indonesian Time", "ui.label.zone.wib"),//asia//143
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "WIT", "Eastern Indonesian Time", "ui.label.zone.wit"),//asia//144
		new TimeZoneInfo(new WTimeSpan(0, +8, 00, 0), new WTimeSpan(0), "WITA", "Central Indonesian Time", "ui.label.zone.wita"),//asia//145
		new TimeZoneInfo(new WTimeSpan(0, +13, 00, 0), new WTimeSpan(0), "WST", "West Samoa Time", "ui.label.zone.wsst"),//pacific//146
		new TimeZoneInfo(new WTimeSpan(0, +0, 00, 0), new WTimeSpan(0), "WT", "Western Sahara Standard Time", "ui.label.zone.wt"),//africa//147
		//x
		//y
		new TimeZoneInfo(new WTimeSpan(0, +9, 00, 0), new WTimeSpan(0), "YAKT", "Yakutsk Time", "ui.label.zone.yakt"),//asia//148
		new TimeZoneInfo(new WTimeSpan(0, +10, 00, 0), new WTimeSpan(0), "YAPT", "Yap Time", "ui.label.zone.yapt"),//pacific//149
		new TimeZoneInfo(new WTimeSpan(0, +5, 00, 0), new WTimeSpan(0), "YEKT", "Yekaterinburg Time", "ui.label.zone.yekt"),//asia//150
		//z
	};

	protected static final TimeZoneInfo[] m_mil_timezones = {
			new TimeZoneInfo(new WTimeSpan(0, 0, 0, 0), new WTimeSpan(0), "Z", "Zulu Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 1, 0, 0), new WTimeSpan(0), "A", "Alpha Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 2, 0, 0), new WTimeSpan(0), "B", "Bravo Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 3, 0, 0), new WTimeSpan(0), "C", "Charlie Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 4, 0, 0), new WTimeSpan(0), "D", "Delta Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 5, 0, 0), new WTimeSpan(0), "E", "Echo Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 6, 0, 0), new WTimeSpan(0), "F", "Foxtrot Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 7, 0, 0), new WTimeSpan(0), "G", "Golf Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 8, 0, 0), new WTimeSpan(0), "H", "Hotel Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 9, 0, 0), new WTimeSpan(0), "I", "India Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 10, 0, 0), new WTimeSpan(0), "K",	"Kilo Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 11, 0, 0), new WTimeSpan(0), "L",	"Lima Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, 12, 0, 0), new WTimeSpan(0), "M",	"Mike Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -1, 0, 0), new WTimeSpan(0), "N",	"November Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -2, 0, 0), new WTimeSpan(0), "O",	"Oscar Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -3, 0, 0), new WTimeSpan(0), "P",	"Papa Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -4, 0, 0), new WTimeSpan(0), "Q",	"Quebec Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -5, 0, 0), new WTimeSpan(0), "R",	"Romeo Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -6, 0, 0), new WTimeSpan(0), "S",	"Sierra Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -7, 0, 0), new WTimeSpan(0), "T",	"Tango Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -8, 0, 0), new WTimeSpan(0), "U",	"Uniform Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -9, 0, 0), new WTimeSpan(0), "V",	"Vector Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -10, 0, 0), new WTimeSpan(0), "W", "Whiskey Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -11, 0, 0), new WTimeSpan(0), "X", "X-ray Time Zone" , ""),
			new TimeZoneInfo(new WTimeSpan(0, -12, 0, 0), new WTimeSpan(0), "Y", "Yankee Time Zone", "")
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
				m_dst_timezones[19],
				m_std_timezones[56]
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

	/**
	 * Get a timezone from its offset from UTC.
	 * @param offset the timezones offset from UTC in hours
	 * @return the timezone with the given offset, null if one is not found
	 */
	public static final TimeZoneInfo getTimeZoneFromOffset(int offset) {
		for (TimeZoneInfo tzi : m_std_timezones)
			if (tzi.getTimezoneOffset().GetHours() == offset)
				return tzi;
		return null;
	}

	/**
	 * Get all LST timezones in no order.
	 * @return
	 */
	public static final ArrayList<String> getStandardTimeZones(){
		ArrayList<String> list = new ArrayList<String>(m_std_timezones.length);
		for (int i=0; i < m_std_timezones.length; i++) list.add(i, m_std_timezones[i].toString());
		return list;
	}

	/**
	 * Get all DST timezones in no order.
	 * @return
	 */
	public static final ArrayList<String> getDaylightSavingsTimeZones(){
		ArrayList<String> list = new ArrayList<String>(m_dst_timezones.length);
		for (int i=0; i < m_dst_timezones.length; i++) list.add(i, m_dst_timezones[i].toString());
		return list;
	}

	WTimeSpan m_solar_timezone(WTime solar_time) {
							// used for GMT->solar time math based on longitude only, but don't assume that solar noon means the sun is right over you!

		int	day = (int)solar_time.GetDay(WTime.FORMAT_AS_LOCAL),
			year = (int)solar_time.GetYear(WTime.FORMAT_AS_LOCAL),
			month = (int)solar_time.GetMonth(WTime.FORMAT_AS_LOCAL);

		RiseSetInput sInput = new RiseSetInput();
		sInput.Latitude = RADIAN_TO_DEGREE((float)m_latitude);
		sInput.Longitude = RADIAN_TO_DEGREE((float)m_longitude) * -1;
		sInput.timezone = 0;
		sInput.DaytimeSaving = false;
		sInput.year = year;
		sInput.month = month;
		sInput.day = day;
		RiseSetOut sOut = new RiseSetOut();
		SunriseSunsetCalc.calcSun(sInput,sOut);

		WTimeSpan solarTime = new WTimeSpan(0, sOut.SolarNoonHour - 12, sOut.SolarNoonMin, sOut.SolarNoonSec);
		WTimeSpan result = new WTimeSpan((long)0 - solarTime.GetTotalSeconds());

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

		int day = (int)daytime.GetDay(WTime.FORMAT_AS_LOCAL);
		int year = (int)daytime.GetYear(WTime.FORMAT_AS_LOCAL);
		int month = (int)daytime.GetMonth(WTime.FORMAT_AS_LOCAL);

		RiseSetInput sInput = new RiseSetInput();
		sInput.Latitude = RADIAN_TO_DEGREE(m_latitude);
		sInput.Longitude = -1 * RADIAN_TO_DEGREE(m_longitude);
		sInput.timezone = 0;
		sInput.DaytimeSaving = false;
		sInput.year = year;//solar_time.GetYear();
		sInput.month = month;//solar_time.GetMonth();
		sInput.day = day;//solar_time.GetDay();
		RiseSetOut sOut = new RiseSetOut();
		int success = SunriseSunsetCalc.calcSun(sInput,sOut);

		WTime riseTime = new WTime(sOut.YearRise,sOut.MonthRise,sOut.DayRise,sOut.HourRise,sOut.MinRise,sOut.SecRise,Rise.value.GetTimeManager());
		Rise.value = riseTime;
		WTime setTime = new WTime(sOut.YearSet,sOut.MonthSet,sOut.DaySet,sOut.HourSet,sOut.MinSet,sOut.SecSet,Set.value.GetTimeManager());
		Set.value = setTime;
		WTime noonTime = new WTime(sInput.year,sInput.month,sInput.day, sOut.SolarNoonHour, sOut.SolarNoonMin, sOut.SolarNoonSec,Noon.value.GetTimeManager());
		Noon.value = noonTime;

		return success;
	}

	/**
	 * Default constructor.
	 */
	public WorldLocation() {
		m_latitude = 1000.0;
		m_longitude = 1000.0;
		m_timezone = new WTimeSpan(0);
		m_startDST = new WTimeSpan(0);
		m_endDST = new WTimeSpan(0);
		m_amtDST = new WTimeSpan(0, 1, 0, 0);
	}

	/**
	 * Copy constructor
	 * @param wl the world location to copy
	 */
	public WorldLocation(WorldLocation wl) {
		m_latitude = wl.m_latitude;
		m_longitude = wl.m_longitude;
		m_timezone = wl.m_timezone;
		m_startDST = wl.m_startDST;
		m_endDST = wl.m_endDST;
		m_amtDST = wl.m_amtDST;
	}

	/**
	 * Make a rough estimate of the locations timezone based on its latitude and longitude.
	 * @param set 0 if LST timezones should be used, 1 if DST timezones should be used
	 * @return
	 */
	public TimeZoneInfo GuessTimeZone(short set) {
		if (InsideNewZealand()) {
			if (set == 0)
				return m_std_timezones[15];
			else if (set == 1)
				return m_dst_timezones[15];
		}
		else if (InsideTasmania()) {
			if (set == 0)
				return m_std_timezones[3];
			else
				return m_dst_timezones[0];
		}

		double longitude = m_longitude;
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
			double ideal_longitude = ((double)tz[i].getTimezoneOffset().GetTotalSeconds()) / (double)(12.0 * 60.0 * 60.0) * Math.PI;
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
	public TimeZoneInfo CurrentTimeZone(short set) {
		TimeZoneInfo[] tz;
		if (set == -1)	tz = m_mil_timezones;
		else if (m_startDST == m_endDST)	tz = m_std_timezones;
		else					tz = m_dst_timezones;

		int i=0;
		do
		{
			if (WTimeSpan.Equal(tz[i].getTimezoneOffset(), m_timezone))
				break;
		}
		while (tz[++i].getCode() != null);

		if (tz[i].getCode() != null)
			return tz[i];

		return null;
	}

	boolean InsideCanada(double latitude, double longitude) {
		if (latitude < DEGREE_TO_RADIAN((float)42.0))		return false;
		if (latitude > DEGREE_TO_RADIAN((float)83.0))		return false;
		if (longitude < DEGREE_TO_RADIAN((float)-141.0))	return false;
		if (longitude > DEGREE_TO_RADIAN((float)-52.0))		return false;
		return true;
	}

	boolean InsideCanada() {
		return InsideCanada(m_latitude, m_longitude);
	}

	boolean InsideNewZealand() {
		if ((m_longitude > DEGREE_TO_RADIAN((float)172.5)) && (m_longitude < DEGREE_TO_RADIAN((float)178.6))) {
			if ((m_latitude > DEGREE_TO_RADIAN((float)-41.75)) && (m_latitude < DEGREE_TO_RADIAN((float)-34.3))) {	// general extents of New Zealand's north island
				return true;
			}
		}

		if ((m_longitude > DEGREE_TO_RADIAN((float)166.3)) && (m_longitude < DEGREE_TO_RADIAN((float)174.5))) {
			if ((m_latitude > DEGREE_TO_RADIAN((float)-47.35)) && (m_latitude < DEGREE_TO_RADIAN((float)40.4))) {	// general extents of New Zealand's south island
				return true;
			}
		}
		return false;
	}

	boolean InsideTasmania() {
		if ((m_longitude > DEGREE_TO_RADIAN(143.5)) && (m_longitude < DEGREE_TO_RADIAN(149.0))) {
			if ((m_latitude > DEGREE_TO_RADIAN(-44.0)) && (m_latitude < DEGREE_TO_RADIAN(-39.5))) {
				return true;
			}
		}
		return false;
	}

	boolean InsideAustraliaMainland() {
		if ((m_longitude > DEGREE_TO_RADIAN(113.15)) && (m_longitude < DEGREE_TO_RADIAN(153.6333333))) {
			if ((m_latitude > DEGREE_TO_RADIAN(-39.133333)) && (m_latitude < DEGREE_TO_RADIAN(-10.683333333))) {
				return true;
			}
		}
		return false;
	}
}
