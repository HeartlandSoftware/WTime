/**
 * WorldLocation.cpp
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

#include "times_internal.h"
#include "worldlocation.h"
#include "SunriseSunsetCalc.h"
#include "str_printf.h"

#include <cmath>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
#include "boost_bimap.h"

extern bool insideCanadaDetail(class Canada **canada, const double latitude, const double longitude);
extern void insideCanadaCleanup(class Canada *canada);

using namespace HSS_Time;
using namespace HSS_Time_Private;


#define Pi 3.14159265358979323846264
#define TwoPi 6.28318530717958647692529

static __inline double DEGREE_TO_RADIAN(const double X) {
	return (X / 180.0) * 3.14159265358979323846264;
}


static __inline double RADIAN_TO_DEGREE(const double X) {
	return (X * 180.0) * 0.318309886183790671537768;
}


// this list is generated from: http://www.timeanddate.com/library/abbreviations/timezones/

constexpr int STD_TIMEZONE_ID = 0x10000;
constexpr int DST_TIMEZONE_ID = 0x20000;
constexpr int MIL_TIMEZONE_ID = 0x40000;
constexpr std::uint32_t MAKE_ID(int type, int id) { return type | id; }
constexpr bool IS_STD(int id) { return (STD_TIMEZONE_ID & id) != 0; }
constexpr bool IS_DST(int id) { return (DST_TIMEZONE_ID & id) != 0; }
constexpr bool IS_MIL(int id) { return (MIL_TIMEZONE_ID & id) != 0; }


const TimeZoneInfo WorldLocation::m_std_timezones[] = {
	{ WTimeSpan(0, 9, 30, 0),	WTimeSpan(0),			"ACST",	"Australian Central Standard Time", MAKE_ID(STD_TIMEZONE_ID, 0) },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),			"AEST",	"Australian Eastern Standard Time", MAKE_ID(STD_TIMEZONE_ID, 1) },
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0),			"AKST",	"Alaska Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 2) },
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0),			"AST",	"Atlantic Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 3) },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),			"AWST", "Australian Western Standard Time", MAKE_ID(STD_TIMEZONE_ID, 4) },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0),			"CET",	"Central European Time",			MAKE_ID(STD_TIMEZONE_ID, 5) },	// 5
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0),			"CST",	"Central Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 6) },
	{ WTimeSpan(0, 7, 0, 0),	WTimeSpan(0),			"CXT",	"Christmas Island Time",			MAKE_ID(STD_TIMEZONE_ID, 7) },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),			"EET",	"Eastern European Time",			MAKE_ID(STD_TIMEZONE_ID, 8) },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0),			"EST",	"Eastern Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 9) },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0),			"HAST",	"Hawaii-Aleutian Standard Time",	MAKE_ID(STD_TIMEZONE_ID, 10) },	// 10
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),			"MSK",	"Moscow Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 11) },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0),			"MST",	"Mountain Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 12) },
	{ WTimeSpan(0, 11, 30, 0),	WTimeSpan(0),			"NFT",	"Norfolk (Island) Time",			MAKE_ID(STD_TIMEZONE_ID, 13) },
	{ WTimeSpan(0, -3, -30, 0),	WTimeSpan(0),			"NST",	"Newfoundland Standard Time",		MAKE_ID(STD_TIMEZONE_ID, 14) },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0),			"NZST",	"New Zealand Standard Time",		MAKE_ID(STD_TIMEZONE_ID, 15) },	// 15
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0),			"PST",	"Pacific Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 16) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			"UTC",	"Universal Coordinated Time",		MAKE_ID(STD_TIMEZONE_ID, 17) },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),			"RZ1",	"Russian Zone 1",					MAKE_ID(STD_TIMEZONE_ID, 18) },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),			"RZ2",	"Russian Zone 2",					MAKE_ID(STD_TIMEZONE_ID, 19) },
	{ WTimeSpan(0, 4, 0, 0),	WTimeSpan(0),			"RZ3",	"Russian Zone 3",					MAKE_ID(STD_TIMEZONE_ID, 20) },	// 20
	{ WTimeSpan(0, -1, 0, 0),	WTimeSpan(0),			"WAT",	"West African Time",				MAKE_ID(STD_TIMEZONE_ID, 21) },
	{ WTimeSpan(0, -2, 0, 0),	WTimeSpan(0),			"AT",	"Azores Time",						MAKE_ID(STD_TIMEZONE_ID, 22) },
	{ WTimeSpan(0, -11, 0, 0),	WTimeSpan(0),			"NT",	"Nome Time",						MAKE_ID(STD_TIMEZONE_ID, 23) },
	{ WTimeSpan(0, 5, 30, 0),	WTimeSpan(0),			"IST",	"Indian Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 24) },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),			"CCT",	"China Coast Time",					MAKE_ID(STD_TIMEZONE_ID, 25) },	// 25
	{ WTimeSpan(0, 9, 0, 0),	WTimeSpan(0),			"JST",	"Japan Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 26) },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),			"GST",	"Guam Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 27) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			NULL,	NULL,								0 }
};


const TimeZoneInfo WorldLocation::m_std_extra_timezones[] = {
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0),			"ACT",	"Acre Time",						MAKE_ID(STD_TIMEZONE_ID, 28) },
	{ WTimeSpan(0, +8, 45, 0),	WTimeSpan(0),			"ACWST","Guam Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 29) },
	{ WTimeSpan(0, +4, 30, 0),	WTimeSpan(0),			"AFT",	"Afghanistan Time",					MAKE_ID(STD_TIMEZONE_ID, 30) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"ALMT",	"Alma-Ata Time",					MAKE_ID(STD_TIMEZONE_ID, 31) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0),			"AMT",	"Amazon Time",						MAKE_ID(STD_TIMEZONE_ID, 32) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"AMT",	"Armenia Time",						MAKE_ID(STD_TIMEZONE_ID, 33) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"ANAT",	"Anadyr Time",						MAKE_ID(STD_TIMEZONE_ID, 34) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"AQTT",	"Aqtobe Time",						MAKE_ID(STD_TIMEZONE_ID, 35) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0),			"ART",	"Argentina Time",					MAKE_ID(STD_TIMEZONE_ID, 36) },
	{ WTimeSpan(0, +3, 00, 0),	WTimeSpan(0),			"AST",	"Arabia Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 37) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"AZT",	"Azerbaijan Time",					MAKE_ID(STD_TIMEZONE_ID, 38) },
	{ WTimeSpan(0, -12, 00, 0),	WTimeSpan(0),			"AoE",	"Anywhere on Earth",				MAKE_ID(STD_TIMEZONE_ID, 39) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"BNT",	"Brunei Darussalam Time",			MAKE_ID(STD_TIMEZONE_ID, 40) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0),			"BOT",	"Bolivia Time",						MAKE_ID(STD_TIMEZONE_ID, 41) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0),			"BRT",	"Bras\u00EDlia Time",				MAKE_ID(STD_TIMEZONE_ID, 42) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"BST",	"Bangladesh Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 43) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"BTT",	"Guam Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 44) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"CAST",	"Casey Time",						MAKE_ID(STD_TIMEZONE_ID, 45) },
	{ WTimeSpan(0, +2, 00, 0),	WTimeSpan(0),			"CAT",	"Central Africa Time",				MAKE_ID(STD_TIMEZONE_ID, 46) },
	{ WTimeSpan(0, +6, 30, 0),	WTimeSpan(0),			"CCT",	"Cocos Islands Time",				MAKE_ID(STD_TIMEZONE_ID, 47) },
	{ WTimeSpan(0, +12, 45, 0),	WTimeSpan(0),			"CHAST","Chatham Island Standard Time",		MAKE_ID(STD_TIMEZONE_ID, 48) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"CHOT",	"Choibalsan Time",					MAKE_ID(STD_TIMEZONE_ID, 49) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0),			"CHUT",	"Chuuk Time",						MAKE_ID(STD_TIMEZONE_ID, 50) },
	{ WTimeSpan(0, -10, 00, 0),	WTimeSpan(0),			"CKT",	"Cook Island Time",					MAKE_ID(STD_TIMEZONE_ID, 51) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0),			"CLT",	"Chile Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 52) },
	{ WTimeSpan(0, -5, 00, 0),	WTimeSpan(0),			"COT",	"Colombia Time",					MAKE_ID(STD_TIMEZONE_ID, 53) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"CST",	"China Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 54) },
	{ WTimeSpan(0, -5, 00, 0),	WTimeSpan(0),			"CST",	"Cuba Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 55) },
	{ WTimeSpan(0, -1, 00, 0),	WTimeSpan(0),			"CVT",	"Cape Verde Time",					MAKE_ID(STD_TIMEZONE_ID, 56) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0),			"ChST",	"Chamorro Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 57) },
	{ WTimeSpan(0, +7, 00, 0),	WTimeSpan(0),			"DAVT",	"Davis Time",						MAKE_ID(STD_TIMEZONE_ID, 58) },
	{ WTimeSpan(0, -6, 00, 0),	WTimeSpan(0),			"EAST",	"Easter Island Standard Time",		MAKE_ID(STD_TIMEZONE_ID, 59) },
	{ WTimeSpan(0, +3, 00, 0),	WTimeSpan(0),			"EAT",	"Eastern Africa Time",				MAKE_ID(STD_TIMEZONE_ID, 60) },
	{ WTimeSpan(0, -5, 00, 0),	WTimeSpan(0),			"ECT",	"Ecuador Time",						MAKE_ID(STD_TIMEZONE_ID, 61) },
	{ WTimeSpan(0, -1, 00, 0),	WTimeSpan(0),			"EGT",	"East Greenland Time",				MAKE_ID(STD_TIMEZONE_ID, 62) },
	{ WTimeSpan(0, +3, 00, 0),	WTimeSpan(0),			"FET",	"Further-Eastern European Time",	MAKE_ID(STD_TIMEZONE_ID, 63) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"FJT",	"Fiji Time",						MAKE_ID(STD_TIMEZONE_ID, 64) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0),			"FKT",	"Falkland Island Time",				MAKE_ID(STD_TIMEZONE_ID, 65) },
	{ WTimeSpan(0, -2, 00, 0),	WTimeSpan(0),			"FNT",	"Fernando de Noronha Time",			MAKE_ID(STD_TIMEZONE_ID, 66) },
	{ WTimeSpan(0, -6, 00, 0),	WTimeSpan(0),			"GALT",	"Galapagos Time",					MAKE_ID(STD_TIMEZONE_ID, 67) },
	{ WTimeSpan(0, -9, 00, 0),	WTimeSpan(0),			"GAMT",	"Gambier Time",						MAKE_ID(STD_TIMEZONE_ID, 68) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"GET",	"Georgia Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 69) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0),			"GFT",	"French Guiana Time",				MAKE_ID(STD_TIMEZONE_ID, 70) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"GILT",	"Gilbert Island Time",				MAKE_ID(STD_TIMEZONE_ID, 71) },
	{ WTimeSpan(0, +0, 00, 0),	WTimeSpan(0),			"GMT",	"Greenwich Mean Time",				MAKE_ID(STD_TIMEZONE_ID, 72) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"GST",	"Gulf Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 73) },
	{ WTimeSpan(0, -2, 00, 0),	WTimeSpan(0),			"GST",	"South Georgia Time",				MAKE_ID(STD_TIMEZONE_ID, 74) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0),			"GYT",	"Guyana Time",						MAKE_ID(STD_TIMEZONE_ID, 75) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"HKT",	"Hong Kong Time",					MAKE_ID(STD_TIMEZONE_ID, 76) },
	{ WTimeSpan(0, +7, 00, 0),	WTimeSpan(0),			"HOVT",	"Hovd Time",						MAKE_ID(STD_TIMEZONE_ID, 77) },
	{ WTimeSpan(0, +7, 00, 0),	WTimeSpan(0),			"ICT",	"Indochina Time",					MAKE_ID(STD_TIMEZONE_ID, 78) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"IOT",	"Indian Chagos Time",				MAKE_ID(STD_TIMEZONE_ID, 79) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"IRKT",	"Irkutsk Time",						MAKE_ID(STD_TIMEZONE_ID, 80) },
	{ WTimeSpan(0, +3, 30, 0),	WTimeSpan(0),			"IRST",	"Iran Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 81) },
	{ WTimeSpan(0, +1, 00, 0),	WTimeSpan(0),			"IST",	"Irish Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 82) },
	{ WTimeSpan(0, +2, 00, 0),	WTimeSpan(0),			"IST",	"Israel Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 83) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"KGT",	"Kyrgyzstan Time",					MAKE_ID(STD_TIMEZONE_ID, 84) },
	{ WTimeSpan(0, +11, 00, 0),	WTimeSpan(0),			"KOST",	"Kosrae Time",						MAKE_ID(STD_TIMEZONE_ID, 85) },
	{ WTimeSpan(0, +7, 00, 0),	WTimeSpan(0),			"KRAT",	"Krasnoyarsk Time",					MAKE_ID(STD_TIMEZONE_ID, 86) },
	{ WTimeSpan(0, +9, 00, 0),	WTimeSpan(0),			"KST",	"Korea Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 87) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"KUYT",	"Kuybyshev Time",					MAKE_ID(STD_TIMEZONE_ID, 88) },
	{ WTimeSpan(0, +10, 30, 0),	WTimeSpan(0),			"LHST",	"Lord Howe Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 89) },
	{ WTimeSpan(0, +14, 00, 0),	WTimeSpan(0),			"LINT",	"Line Islands Time",				MAKE_ID(STD_TIMEZONE_ID, 90) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0),			"MAGT",	"Magadan Time",						MAKE_ID(STD_TIMEZONE_ID, 91) },
	{ WTimeSpan(0, -9, -30, 0),	WTimeSpan(0),			"MART",	"Marquesas Time",					MAKE_ID(STD_TIMEZONE_ID, 92) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"MAWT",	"Mawson Time",						MAKE_ID(STD_TIMEZONE_ID, 93) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"MHT",	"Marshall Islands Time",			MAKE_ID(STD_TIMEZONE_ID, 94) },
	{ WTimeSpan(0, +6, 30, 0),	WTimeSpan(0),			"MMT",	"Myanmar Time",						MAKE_ID(STD_TIMEZONE_ID, 95) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"MUT",	"Mauritius Time",					MAKE_ID(STD_TIMEZONE_ID, 96) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"MVT",	"Maldives Time",					MAKE_ID(STD_TIMEZONE_ID, 97) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"MYT",	"Malaysia Time",					MAKE_ID(STD_TIMEZONE_ID, 98) },
	{ WTimeSpan(0, +11, 00, 0),	WTimeSpan(0),			"NCT",	"New Caledonia Time",				MAKE_ID(STD_TIMEZONE_ID, 99) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"NOVT",	"Novosibirsk Time",					MAKE_ID(STD_TIMEZONE_ID, 100) },
	{ WTimeSpan(0, +5, 45, 0),	WTimeSpan(0),			"NPT",	"Nepal Time",						MAKE_ID(STD_TIMEZONE_ID, 101) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"NRT",	"Nauru Time",						MAKE_ID(STD_TIMEZONE_ID, 102) },
	{ WTimeSpan(0, -11, 00, 0),	WTimeSpan(0),			"NUT",	"Niue Time",						MAKE_ID(STD_TIMEZONE_ID, 103) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"OMST",	"Omsk Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 104) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"ORAT",	"Oral Time",						MAKE_ID(STD_TIMEZONE_ID, 105) },
	{ WTimeSpan(0, -5, 00, 0),	WTimeSpan(0),			"PET",	"Peru Time",						MAKE_ID(STD_TIMEZONE_ID, 106) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"PETT",	"Kamchatka Time",					MAKE_ID(STD_TIMEZONE_ID, 107) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0),			"PGT",	"Papua New Guinea Time",			MAKE_ID(STD_TIMEZONE_ID, 108) },
	{ WTimeSpan(0, +13, 00, 0),	WTimeSpan(0),			"PHOT",	"Phoenix Island Time",				MAKE_ID(STD_TIMEZONE_ID, 109) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"PHT",	"Philippine Time",					MAKE_ID(STD_TIMEZONE_ID, 110) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"PKT",	"Pakistan Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 111) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0),			"PMST",	"Pierre & Miquelon Standard Time",	MAKE_ID(STD_TIMEZONE_ID, 112) },
	{ WTimeSpan(0, +11, 00, 0),	WTimeSpan(0),			"PONT",	"Pohnpei Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 113) },
	{ WTimeSpan(0, -8, 00, 0),	WTimeSpan(0),			"PST",	"Pitcairn Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 114) },
	{ WTimeSpan(0, +9, 00, 0),	WTimeSpan(0),			"PWT",	"Palau Time",						MAKE_ID(STD_TIMEZONE_ID, 115) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0),			"PYT",	"Paraguay Time",					MAKE_ID(STD_TIMEZONE_ID, 116) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"QYZT",	"Qyzylorda Time",					MAKE_ID(STD_TIMEZONE_ID, 117) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"RET",	"Reunion Time",						MAKE_ID(STD_TIMEZONE_ID, 118) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0),			"ROTT",	"Rothera Time",						MAKE_ID(STD_TIMEZONE_ID, 119) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0),			"SAKT",	"Sakhalin Time",					MAKE_ID(STD_TIMEZONE_ID, 120) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"SAMT",	"Samara Time",						MAKE_ID(STD_TIMEZONE_ID, 121) },
	{ WTimeSpan(0, +2, 00, 0),	WTimeSpan(0),			"SAST",	"South Africa Standard Time",		MAKE_ID(STD_TIMEZONE_ID, 122) },
	{ WTimeSpan(0, +11, 00, 0),	WTimeSpan(0),			"SBT",	"Solomon Islands Time",				MAKE_ID(STD_TIMEZONE_ID, 123) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0),			"SCT",	"Seychelles Time",					MAKE_ID(STD_TIMEZONE_ID, 124) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"SGT",	"Singapore Time",					MAKE_ID(STD_TIMEZONE_ID, 125) },
	{ WTimeSpan(0, +11, 00, 0),	WTimeSpan(0),			"SRET",	"Srednekolymsk Time",				MAKE_ID(STD_TIMEZONE_ID, 126) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0),			"SRT",	"Suriname Time",					MAKE_ID(STD_TIMEZONE_ID, 127) },
	{ WTimeSpan(0, -11, 00, 0),	WTimeSpan(0),			"SST",	"Samoa Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 128) },
	{ WTimeSpan(0, +3, 00, 0),	WTimeSpan(0),			"SYOT",	"Syowa Time",						MAKE_ID(STD_TIMEZONE_ID, 129) },
	{ WTimeSpan(0, -10, 00, 0),	WTimeSpan(0),			"TAHT",	"Tahiti Time",						MAKE_ID(STD_TIMEZONE_ID, 130) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"TFT",	"French Southern and Antarctic Time",MAKE_ID(STD_TIMEZONE_ID, 131) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"TJT",	"Tajikistan Time",					MAKE_ID(STD_TIMEZONE_ID, 132) },
	{ WTimeSpan(0, +13, 00, 0),	WTimeSpan(0),			"TKT",	"Tokelau Time",						MAKE_ID(STD_TIMEZONE_ID, 133) },
	{ WTimeSpan(0, +9, 00, 0),	WTimeSpan(0),			"TLT",	"East Timor Time",					MAKE_ID(STD_TIMEZONE_ID, 134) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"TMT",	"Turkmenistan Time",				MAKE_ID(STD_TIMEZONE_ID, 135) },
	{ WTimeSpan(0, +13, 00, 0),	WTimeSpan(0),			"TOT",	"Tonga Time",						MAKE_ID(STD_TIMEZONE_ID, 136) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"TVT",	"Tuvalu Time",						MAKE_ID(STD_TIMEZONE_ID, 137) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"ULAT",	"Ulaanbaatar Time",					MAKE_ID(STD_TIMEZONE_ID, 138) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0),			"UYT",	"Uruguay Time",						MAKE_ID(STD_TIMEZONE_ID, 139) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"UZT",	"Uzbekistan Time",					MAKE_ID(STD_TIMEZONE_ID, 140) },
	{ WTimeSpan(0, -4, -30, 0),	WTimeSpan(0),			"VET",	"Venezuelan Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 141) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0),			"VLAT",	"Vladivostok Time",					MAKE_ID(STD_TIMEZONE_ID, 142) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0),			"VOST",	"Vostok Time",						MAKE_ID(STD_TIMEZONE_ID, 143) },
	{ WTimeSpan(0, +11, 00, 0),	WTimeSpan(0),			"VUT",	"Vanuatu Time",						MAKE_ID(STD_TIMEZONE_ID, 144) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"WAKT",	"Wake Time",						MAKE_ID(STD_TIMEZONE_ID, 145) },
	{ WTimeSpan(0, +0, 00, 0),	WTimeSpan(0),			"WET",	"Western European Time",			MAKE_ID(STD_TIMEZONE_ID, 146) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0),			"WFT",	"Wallis and Futuna Time",			MAKE_ID(STD_TIMEZONE_ID, 147) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0),			"WGT",	"West Greenland Time",				MAKE_ID(STD_TIMEZONE_ID, 148) },
	{ WTimeSpan(0, +7, 00, 0),	WTimeSpan(0),			"WIB",	"Western Indonesian Time",			MAKE_ID(STD_TIMEZONE_ID, 149) },
	{ WTimeSpan(0, +9, 00, 0),	WTimeSpan(0),			"WIT",	"Eastern Indonesian Time",			MAKE_ID(STD_TIMEZONE_ID, 150) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0),			"WITA",	"Central Indonesian Time",			MAKE_ID(STD_TIMEZONE_ID, 151) },
	{ WTimeSpan(0, +13, 00, 0),	WTimeSpan(0),			"WST",	"West Samoa Time",					MAKE_ID(STD_TIMEZONE_ID, 152) },
	{ WTimeSpan(0, +0, 00, 0),	WTimeSpan(0),			"WT",	"Western Sahara Standard Time",		MAKE_ID(STD_TIMEZONE_ID, 153) },
	{ WTimeSpan(0, +9, 00, 0),	WTimeSpan(0),			"YAKT",	"Yakutsk Time",						MAKE_ID(STD_TIMEZONE_ID, 154) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0),			"YAPT",	"Yap Time",							MAKE_ID(STD_TIMEZONE_ID, 155) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0),			"YEKT",	"Yekaterinburg Time",				MAKE_ID(STD_TIMEZONE_ID, 156) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			NULL,	NULL,								0 }
};


const ::TimeZoneInfo WorldLocation::m_dst_timezones[] = {
	{ WTimeSpan(0, 9, 30, 0),	WTimeSpan(0, 1, 0, 0),	"ACDT",	"Australian Central Daylight Time", MAKE_ID(DST_TIMEZONE_ID, 0) },
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0, 1, 0, 0),	"ADT",	"Atlantic Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 1) },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0, 1, 0, 0),	"AEDT",	"Australian Eastern Daylight Time", MAKE_ID(DST_TIMEZONE_ID, 2) },
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0, 1, 0, 0),	"AKDT",	"Alaska Daylight Time",				MAKE_ID(DST_TIMEZONE_ID, 3) },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0, 1, 0, 0), 	"AWDT", "Australian Western Daylight Time", MAKE_ID(DST_TIMEZONE_ID, 4) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"BST",	"British Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 5) },	// 5
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0, 1, 0, 0),	"CDT",	"Central Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 6) },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0, 1, 0, 0),	"CEDT",	"Central European Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 7) },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0, 1, 0, 0),	"EDT",	"Eastern Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 8) },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0, 1, 0, 0),	"EEDT",	"Eastern European Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 9) },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0, 1, 0, 0),	"HADT",	"Hawaii-Aleutian Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 10) },	// 10
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"IST",	"Irish Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 11) },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0, 1, 0, 0),	"MDT",	"Mountain Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 12) },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0, 1, 0, 0),	"MSD",	"Moscow Daylight Time",				MAKE_ID(DST_TIMEZONE_ID, 13) },
	{ WTimeSpan(0, -3, -30, 0),	WTimeSpan(0, 1, 0, 0),	"NDT",	"Newfoundland Daylight Time",		MAKE_ID(DST_TIMEZONE_ID, 14) },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0, 1, 0, 0),	"NZDT",	"New Zealand Daylight Time",		MAKE_ID(DST_TIMEZONE_ID, 15) },	// 15
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0, 1, 0, 0),	"PDT",	"Pacific Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 16) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"WEDT",	"Western European Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 17) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			NULL,	NULL,								0 }
};


const ::TimeZoneInfo WorldLocation::m_dst_extra_timezones[] = {
	{ WTimeSpan(0, +3, 00, 0),	WTimeSpan(0, 1, 0, 0),	"ADT",	"Arabia Daylight Time",				MAKE_ID(DST_TIMEZONE_ID, 18) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0, 1, 0, 0),	"AMST",	"Amazon Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 19) },
	{ WTimeSpan(0, -1, 00, 0),	WTimeSpan(0, 1, 0, 0),	"AZOST","Azores Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 20) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0, 1, 0, 0),	"AZST",	"Azerbaijan Summer Time",			MAKE_ID(DST_TIMEZONE_ID, 21) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0, 1, 0, 0),	"BRST",	"Bras\u00EDlia Summer Time ",		MAKE_ID(DST_TIMEZONE_ID, 22) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0, 1, 0, 0),	"CDT",	"Cuba Daylight Time",				MAKE_ID(DST_TIMEZONE_ID, 23) },
	{ WTimeSpan(0, +12, 45, 0),	WTimeSpan(0, 1, 0, 0),	"CHADT","Chatham Island Daylight Time",		MAKE_ID(DST_TIMEZONE_ID, 24) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0, 1, 0, 0),	"CLST",	"Chile Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 25) },
	{ WTimeSpan(0, -6, 00, 0),	WTimeSpan(0, 1, 0, 0),	"EASST","Easter Island Summer Time",		MAKE_ID(DST_TIMEZONE_ID, 26) },
	{ WTimeSpan(0, -1, 00, 0),	WTimeSpan(0, 1, 0, 0),	"EGST",	"Eastern Greenland Summer Time",	MAKE_ID(DST_TIMEZONE_ID, 27) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0, 1, 0, 0),	"FKST",	"Falkland Islands Summer Time",		MAKE_ID(DST_TIMEZONE_ID, 28) },
	{ WTimeSpan(0, +2, 00, 0),	WTimeSpan(0, 1, 0, 0),	"IDT",	"Israel Daylight Time",				MAKE_ID(DST_TIMEZONE_ID, 29) },
	{ WTimeSpan(0, +3, 30, 0),	WTimeSpan(0, 1, 0, 0),	"IRDT",	"Iran Daylight Time",				MAKE_ID(DST_TIMEZONE_ID, 30) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0, 1, 0, 0),	"IRKST","Irkutsk Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 31) },
	{ WTimeSpan(0, +7, 00, 0),	WTimeSpan(0, 1, 0, 0),	"KRAST","Krasnoyarsk Summer Time",			MAKE_ID(DST_TIMEZONE_ID, 32) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0, 1, 0, 0),	"LHDT",	"Lord Howe Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 33) },
	{ WTimeSpan(0, +11, 00, 0),	WTimeSpan(0, 1, 0, 0),	"MAGST","Magadan Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 34) },
	{ WTimeSpan(0, +6, 00, 0),	WTimeSpan(0, 1, 0, 0),	"NOVST","Novosibirsk Summer Time",			MAKE_ID(DST_TIMEZONE_ID, 35) },
	{ WTimeSpan(0, +8, 00, 0),	WTimeSpan(0, 1, 0, 0),	"OMSST","Omsk Summer Time",					MAKE_ID(DST_TIMEZONE_ID, 36) },
	{ WTimeSpan(0, +13, 00, 0),	WTimeSpan(0, 1, 0, 0),	"PETST","Kamchatka Summer Time",			MAKE_ID(DST_TIMEZONE_ID, 37) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0, 1, 0, 0),	"PMDT",	"Pierre & Miquelon Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 38) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0, 1, 0, 0),	"UYST",	"Uruguay Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 39) },
	{ WTimeSpan(0, +10, 00, 0),	WTimeSpan(0, 1, 0, 0),	"VLAST","Vladivostok Summer Time",			MAKE_ID(DST_TIMEZONE_ID, 40) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0, 1, 0, 0),	"WARST","Western Argentine Summer Time",	MAKE_ID(DST_TIMEZONE_ID, 41) },
	{ WTimeSpan(0, +1, 00, 0),	WTimeSpan(0, 1, 0, 0),	"WAST",	"West Africa Summer Time",			MAKE_ID(DST_TIMEZONE_ID, 42) },
	{ WTimeSpan(0, -3, 00, 0),	WTimeSpan(0, 1, 0, 0),	"WGST",	"Western Greenland Summer Time",	MAKE_ID(DST_TIMEZONE_ID, 43) },
	{ WTimeSpan(0, 0, 00, 0),	WTimeSpan(0, 1, 0, 0),	"WST",	"Western Sahara Summer Time",		MAKE_ID(DST_TIMEZONE_ID, 44) },
	{ WTimeSpan(0, +9, 00, 0),	WTimeSpan(0, 1, 0, 0),	"YAKST","Yakutsk Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 45) },
	{ WTimeSpan(0, +5, 00, 0),	WTimeSpan(0, 1, 0, 0),	"YEKST","Yekaterinburg Summer Time",		MAKE_ID(DST_TIMEZONE_ID, 46) },
	{ WTimeSpan(0, +12, 00, 0),	WTimeSpan(0, 1, 0, 0),	"FJST",	"Fiji Summer Time",					MAKE_ID(DST_TIMEZONE_ID, 47) },
	{ WTimeSpan(0, -4, 00, 0),	WTimeSpan(0, 1, 0, 0),	"PYST",	"Paraguay Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 48) },
	{ WTimeSpan(0, +4, 00, 0),	WTimeSpan(0, 1, 0, 0),	"AMST",	"Armenia Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 49) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			NULL,	NULL,								0 }
};


const ::TimeZoneInfo WorldLocation::m_mil_timezones[] = {
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),		"Z",	"Zulu Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 0) },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0),		"A",	"Alpha Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 1) },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),		"B",	"Bravo Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 2) },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),		"C",	"Charlie Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 3) },
	{ WTimeSpan(0, 4, 0, 0),	WTimeSpan(0),		"D",	"Delta Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 4) },
	{ WTimeSpan(0, 5, 0, 0),	WTimeSpan(0),		"E",	"Echo Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 5) },	// 5
	{ WTimeSpan(0, 6, 0, 0),	WTimeSpan(0),		"F",	"Foxtrot Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 6) },
	{ WTimeSpan(0, 7, 0, 0),	WTimeSpan(0),		"G",	"Golf Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 7) },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),		"H",	"Hotel Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 8) },
	{ WTimeSpan(0, 9, 0, 0),	WTimeSpan(0),		"I",	"India Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 9) },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),		"K",	"Kilo Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 10) },	// 10
	{ WTimeSpan(0, 11, 0, 0),	WTimeSpan(0),		"L",	"Lima Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 11) },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0),		"M",	"Mike Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 12) },
	{ WTimeSpan(0, -1, 0, 0),	WTimeSpan(0),		"N",	"November Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 13) },
	{ WTimeSpan(0, -2, 0, 0),	WTimeSpan(0),		"O",	"Oscar Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 14) },
	{ WTimeSpan(0, -3, 0, 0),	WTimeSpan(0),		"P",	"Papa Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 15) },	// 15
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0),		"Q",	"Quebec Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 16) },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0),		"R",	"Romeo Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 17) },
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0),		"S",	"Sierra Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 18) },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0),		"T",	"Tango Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 19) },
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0),		"U",	"Uniform Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 20) }, // 20
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0),		"V",	"Vector Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 21) },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0),		"W",	"Whiskey Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 22) },
	{ WTimeSpan(0, -11, 0, 0),	WTimeSpan(0),		"X",	"X-ray Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 23) },
	{ WTimeSpan(0, -12, 0, 0),	WTimeSpan(0),		"Y",	"Yankee Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 24) },
	{ WTimeSpan(0),				WTimeSpan(0),		NULL,	NULL,					0 }
};


struct WindowsTimezoneData
{
	const std::string WindowsName;
	const std::uint32_t HssId;
};

struct RegionZone
{
	const std::string Name;
	const std::uint32_t HssId;
};

//https://raw.githubusercontent.com/unicode-org/cldr/master/common/supplemental/windowsZones.xml
const std::vector<WindowsTimezoneData> WindowsMap =
{
	{ "Dateline Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 12) },
	{ "UTC-11", MAKE_ID(MIL_TIMEZONE_ID, 23) },
	{ "Aleutian Standard Time", MAKE_ID(STD_TIMEZONE_ID, 10) },
	{ "Hawaiian Standard Time", MAKE_ID(STD_TIMEZONE_ID, 10) },
	{ "Marquesas Standard Time", MAKE_ID(STD_TIMEZONE_ID, 92) },
	{ "Alaskan Standard Time", MAKE_ID(STD_TIMEZONE_ID, 2) },
	{ "UTC-09", MAKE_ID(MIL_TIMEZONE_ID, 21) },
	{ "Pacific Standard Time (Mexico)", MAKE_ID(STD_TIMEZONE_ID, 92) },
	{ "UTC-08", MAKE_ID(MIL_TIMEZONE_ID, 20) },
	{ "Pacific Standard Time", MAKE_ID(STD_TIMEZONE_ID, 16) },
	{ "US Mountain Standard Time", MAKE_ID(STD_TIMEZONE_ID, 12) },
	{ "Mountain Standard Time (Mexico)", MAKE_ID(STD_TIMEZONE_ID, 12) },
	{ "Mountain Standard Time", MAKE_ID(STD_TIMEZONE_ID, 12) },
	{ "Central America Standard Time", MAKE_ID(STD_TIMEZONE_ID, 6) },
	{ "Central Standard Time (Mexico)", MAKE_ID(STD_TIMEZONE_ID, 6) },
	{ "Central Standard Time", MAKE_ID(STD_TIMEZONE_ID, 6) },
	{ "Canada Central Standard Time", MAKE_ID(STD_TIMEZONE_ID, 6) },
	{ "Easter Island Standard Time", MAKE_ID(STD_TIMEZONE_ID, 59) },
	{ "SA Pacific Standard Time", MAKE_ID(STD_TIMEZONE_ID, 53) },
	{ "US Eastern Standard Time", MAKE_ID(STD_TIMEZONE_ID, 9) },
	{ "Eastern Standard Time (Mexico)", MAKE_ID(STD_TIMEZONE_ID, 9) },
	{ "Eastern Standard Time", MAKE_ID(STD_TIMEZONE_ID, 9) },
	{ "Haiti Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 17) },
	{ "Cuba Standard Time", MAKE_ID(STD_TIMEZONE_ID, 55) },
	{ "Turks And Caicos Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 17) },
	{ "Paraguay Standard Time", MAKE_ID(STD_TIMEZONE_ID, 116) },
	{ "Atlantic Standard Time", MAKE_ID(STD_TIMEZONE_ID, 3) },
	{ "Venezuela Standard Time", MAKE_ID(STD_TIMEZONE_ID, 141) },
	{ "Central Brazilian Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 16) },
	{ "SA Western Standard Time", MAKE_ID(STD_TIMEZONE_ID, 75) },
	{ "Pacific SA Standard Time", MAKE_ID(STD_TIMEZONE_ID, 52) },
	{ "Newfoundland Standard Time", MAKE_ID(STD_TIMEZONE_ID, 14) },
	{ "Tocantins Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 15) },
	{ "E. South America Standard Time", MAKE_ID(STD_TIMEZONE_ID, 42) },
	{ "SA Eastern Standard Time", MAKE_ID(STD_TIMEZONE_ID, 119) },
	{ "Argentina Standard Time", MAKE_ID(STD_TIMEZONE_ID, 36) },
	{ "Greenland Standard Time", MAKE_ID(STD_TIMEZONE_ID, 148) },
	{ "Montevideo Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 15) },
	{ "Magallanes Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 15) },
	{ "Saint Pierre Standard Time", MAKE_ID(STD_TIMEZONE_ID, 112) },
	{ "Bahia Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 15) },
	{ "UTC-02", MAKE_ID(MIL_TIMEZONE_ID, 14) },
	{ "Azores Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 14) },
	{ "Cape Verde Standard Time", MAKE_ID(STD_TIMEZONE_ID, 56) },
	{ "UTC", MAKE_ID(STD_TIMEZONE_ID, 17) },
	{ "GMT Standard Time", MAKE_ID(STD_TIMEZONE_ID, 72) },
	{ "Greenwich Standard Time", MAKE_ID(STD_TIMEZONE_ID, 72) },
	{ "Sao Tome Standard Time", MAKE_ID(STD_TIMEZONE_ID, 17) },
	{ "Morocco Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 1) },
	{ "W. Europe Standard Time", MAKE_ID(STD_TIMEZONE_ID, 5) },
	{ "Central Europe Standard Time", MAKE_ID(STD_TIMEZONE_ID, 5) },
	{ "Central European Standard Time", MAKE_ID(STD_TIMEZONE_ID, 5) },
	{ "Romance Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 1) },
	{ "W. Central Africa Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 1) },
	{ "Jordan Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 2) },
	{ "GTB Standard Time", MAKE_ID(STD_TIMEZONE_ID, 8) },
	{ "E. Europe Standard Time", MAKE_ID(STD_TIMEZONE_ID, 8) },
	{ "Middle East Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 2) },
	{ "Egypt Standard Time", MAKE_ID(STD_TIMEZONE_ID, 46) },
	{ "Syria Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 2) },
	{ "West Bank Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 2) },
	{ "South Africa Standard Time", MAKE_ID(STD_TIMEZONE_ID, 122) },
	{ "FLE Standard Time", MAKE_ID(STD_TIMEZONE_ID, 8) },
	{ "Israel Standard Time", MAKE_ID(STD_TIMEZONE_ID, 83) },
	{ "Kaliningrad Standard Time", MAKE_ID(STD_TIMEZONE_ID, 18) },
	{ "Sudan Standard Time", MAKE_ID(STD_TIMEZONE_ID, 46) },
	{ "Libya Standard Time", MAKE_ID(STD_TIMEZONE_ID, 46) },
	{ "Namibia Standard Time", MAKE_ID(STD_TIMEZONE_ID, 46) },
	{ "Arabic Standard Time", MAKE_ID(STD_TIMEZONE_ID, 37) },
	{ "Arab Standard Time", MAKE_ID(STD_TIMEZONE_ID, 37) },
	{ "Turkey Standard Time", MAKE_ID(STD_TIMEZONE_ID, 63) },
	{ "Belarus Standard Time", MAKE_ID(STD_TIMEZONE_ID, 63) },
	{ "Russian Standard Time", MAKE_ID(STD_TIMEZONE_ID, 11) },
	{ "E. Africa Standard Time", MAKE_ID(STD_TIMEZONE_ID, 60) },
	{ "Iran Standard Time", MAKE_ID(STD_TIMEZONE_ID, 81) },
	{ "Arabian Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 4) },
	{ "Astrakhan Standard Time", MAKE_ID(STD_TIMEZONE_ID, 20) },
	{ "Russia Time Zone 3", MAKE_ID(STD_TIMEZONE_ID, 20) },
	{ "Azerbaijan Standard Time", MAKE_ID(STD_TIMEZONE_ID, 38) },
	{ "Mauritius Standard Time", MAKE_ID(STD_TIMEZONE_ID, 96) },
	{ "Saratov Standard Time", MAKE_ID(STD_TIMEZONE_ID, 20) },
	{ "Georgian Standard Time", MAKE_ID(STD_TIMEZONE_ID, 69) },
	{ "Volgograd Standard Time", MAKE_ID(STD_TIMEZONE_ID, 20) },
	{ "Caucasus Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 4) },
	{ "Afghanistan Standard Time", MAKE_ID(STD_TIMEZONE_ID, 30) },
	{ "West Asia Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 5) },
	{ "Ekaterinburg Standard Time", MAKE_ID(STD_TIMEZONE_ID, 156) },
	{ "Pakistan Standard Time", MAKE_ID(STD_TIMEZONE_ID, 111) },
	{ "Qyzylorda Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 5) },
	{ "India Standard Time", MAKE_ID(STD_TIMEZONE_ID, 24) },
	{ "Sri Lanka Standard Time", MAKE_ID(STD_TIMEZONE_ID, 24) },
	{ "Nepal Standard Time", MAKE_ID(STD_TIMEZONE_ID, 101) },
	{ "Central Asia Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 6) },
	{ "Bangladesh Standard Time", MAKE_ID(STD_TIMEZONE_ID, 43) },
	{ "Omsk Standard Time", MAKE_ID(STD_TIMEZONE_ID, 104) },
	{ "Myanmar Standard Time", MAKE_ID(STD_TIMEZONE_ID, 95) },
	{ "SE Asia Standard Time", MAKE_ID(STD_TIMEZONE_ID, 7) },
	{ "Altai Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 7) },
	{ "W. Mongolia Standard Time", MAKE_ID(STD_TIMEZONE_ID, 77) },
	{ "North Asia Standard Time", MAKE_ID(STD_TIMEZONE_ID, 86) },
	{ "N. Central Asia Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 7) },
	{ "Tomsk Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 7) },
	{ "China Standard Time", MAKE_ID(STD_TIMEZONE_ID, 54) },
	{ "North Asia East Standard Time", MAKE_ID(STD_TIMEZONE_ID, 80) },
	{ "Singapore Standard Time", MAKE_ID(STD_TIMEZONE_ID, 125) },
	{ "W. Australia Standard Time", MAKE_ID(STD_TIMEZONE_ID, 4) },
	{ "Taipei Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 8) },
	{ "Ulaanbaatar Standard Time", MAKE_ID(STD_TIMEZONE_ID, 138) },
	{ "Aus Central W. Standard Time", MAKE_ID(STD_TIMEZONE_ID, 29) },
	{ "Transbaikal Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 9) },
	{ "Tokyo Standard Time", MAKE_ID(STD_TIMEZONE_ID, 26) },
	{ "North Korea Standard Time", MAKE_ID(STD_TIMEZONE_ID, 87) },
	{ "Korea Standard Time", MAKE_ID(STD_TIMEZONE_ID, 87) },
	{ "Yakutsk Standard Time", MAKE_ID(STD_TIMEZONE_ID, 154) },
	{ "Cen. Australia Standard Time", MAKE_ID(STD_TIMEZONE_ID, 0) },
	{ "AUS Central Standard Time", MAKE_ID(STD_TIMEZONE_ID, 0) },
	{ "E. Australia Standard Time", MAKE_ID(STD_TIMEZONE_ID, 1) },
	{ "AUS Eastern Standard Time", MAKE_ID(STD_TIMEZONE_ID, 1) },
	{ "West Pacific Standard Time", MAKE_ID(STD_TIMEZONE_ID, 27) },
	{ "Tasmania Standard Time", MAKE_ID(STD_TIMEZONE_ID, 1) },
	{ "Vladivostok Standard Time", MAKE_ID(STD_TIMEZONE_ID, 142) },
	{ "Lord Howe Standard Time", MAKE_ID(STD_TIMEZONE_ID, 89) },
	{ "Bougainville Standard Time", MAKE_ID(STD_TIMEZONE_ID, 123) },
	{ "Russia Time Zone 10", MAKE_ID(STD_TIMEZONE_ID, 126) },
	{ "Magadan Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 11) },
	{ "Norfolk Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 11) },
	{ "Sakhalin Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 11) },
	{ "Central Pacific Standard Time", MAKE_ID(MIL_TIMEZONE_ID, 11) },
	{ "Russia Time Zone 11", MAKE_ID(MIL_TIMEZONE_ID, 11) },
	{ "New Zealand Standard Time", MAKE_ID(STD_TIMEZONE_ID, 15) },
	{ "UTC+12", MAKE_ID(MIL_TIMEZONE_ID, 12) },
	{ "Fiji Standard Time", MAKE_ID(STD_TIMEZONE_ID, 64) },
	{ "Chatham Islands Standard Time", MAKE_ID(STD_TIMEZONE_ID, 48) },
	{ "UTC+13", MAKE_ID(STD_TIMEZONE_ID, 109) },
	{ "Tonga Standard Time", MAKE_ID(STD_TIMEZONE_ID, 136) },
	{ "Samoa Standard Time", MAKE_ID(STD_TIMEZONE_ID, 152) },
	{ "Line Islands Standard Time", MAKE_ID(STD_TIMEZONE_ID, 90) }
};


//https://data.iana.org/time-zones/releases/
const std::vector<RegionZone> RegionMap =
{
#include "RegionMap.inl"
};



boost::bimap<std::uint32_t, std::uint32_t> daylightStandardMap =
makeBimap<std::uint32_t, std::uint32_t>({
	{ MAKE_ID(STD_TIMEZONE_ID, 0), MAKE_ID(DST_TIMEZONE_ID, 0) },
	{ MAKE_ID(STD_TIMEZONE_ID, 3), MAKE_ID(DST_TIMEZONE_ID, 1) },
	{ MAKE_ID(STD_TIMEZONE_ID, 1), MAKE_ID(DST_TIMEZONE_ID, 2) },
	{ MAKE_ID(STD_TIMEZONE_ID, 2), MAKE_ID(DST_TIMEZONE_ID, 3) },
	{ MAKE_ID(STD_TIMEZONE_ID, 4), MAKE_ID(DST_TIMEZONE_ID, 4) },
	{ MAKE_ID(STD_TIMEZONE_ID, 6), MAKE_ID(DST_TIMEZONE_ID, 6) },
	{ MAKE_ID(STD_TIMEZONE_ID, 5), MAKE_ID(DST_TIMEZONE_ID, 7) },
	{ MAKE_ID(STD_TIMEZONE_ID, 9), MAKE_ID(DST_TIMEZONE_ID, 8) },
	{ MAKE_ID(STD_TIMEZONE_ID, 8), MAKE_ID(DST_TIMEZONE_ID, 9) },
	{ MAKE_ID(STD_TIMEZONE_ID, 10), MAKE_ID(DST_TIMEZONE_ID, 10) },
	{ MAKE_ID(STD_TIMEZONE_ID, 12), MAKE_ID(DST_TIMEZONE_ID, 12) },
	{ MAKE_ID(STD_TIMEZONE_ID, 11), MAKE_ID(DST_TIMEZONE_ID, 13) },
	{ MAKE_ID(STD_TIMEZONE_ID, 14), MAKE_ID(DST_TIMEZONE_ID, 14) },
	{ MAKE_ID(STD_TIMEZONE_ID, 15), MAKE_ID(DST_TIMEZONE_ID, 15) },
	{ MAKE_ID(STD_TIMEZONE_ID, 16), MAKE_ID(DST_TIMEZONE_ID, 16) },
	{ MAKE_ID(STD_TIMEZONE_ID, 146), MAKE_ID(DST_TIMEZONE_ID, 17) },
	{ MAKE_ID(STD_TIMEZONE_ID, 37), MAKE_ID(DST_TIMEZONE_ID, 18) },
	{ MAKE_ID(STD_TIMEZONE_ID, 32), MAKE_ID(DST_TIMEZONE_ID, 19) },
	{ MAKE_ID(STD_TIMEZONE_ID, 22), MAKE_ID(DST_TIMEZONE_ID, 20) },
	{ MAKE_ID(STD_TIMEZONE_ID, 38), MAKE_ID(DST_TIMEZONE_ID, 21) },
	{ MAKE_ID(STD_TIMEZONE_ID, 42), MAKE_ID(DST_TIMEZONE_ID, 22) },
	{ MAKE_ID(STD_TIMEZONE_ID, 48), MAKE_ID(DST_TIMEZONE_ID, 24) },
	{ MAKE_ID(STD_TIMEZONE_ID, 52), MAKE_ID(DST_TIMEZONE_ID, 25) },
	{ MAKE_ID(STD_TIMEZONE_ID, 59), MAKE_ID(DST_TIMEZONE_ID, 26) },
	{ MAKE_ID(STD_TIMEZONE_ID, 62), MAKE_ID(DST_TIMEZONE_ID, 27) },
	{ MAKE_ID(STD_TIMEZONE_ID, 65), MAKE_ID(DST_TIMEZONE_ID, 28) },
	{ MAKE_ID(STD_TIMEZONE_ID, 83), MAKE_ID(DST_TIMEZONE_ID, 29) },
	{ MAKE_ID(STD_TIMEZONE_ID, 81), MAKE_ID(DST_TIMEZONE_ID, 30) },
	{ MAKE_ID(STD_TIMEZONE_ID, 80), MAKE_ID(DST_TIMEZONE_ID, 31) },
	{ MAKE_ID(STD_TIMEZONE_ID, 86), MAKE_ID(DST_TIMEZONE_ID, 32) },
	{ MAKE_ID(STD_TIMEZONE_ID, 89), MAKE_ID(DST_TIMEZONE_ID, 33) },
	{ MAKE_ID(STD_TIMEZONE_ID, 100), MAKE_ID(DST_TIMEZONE_ID, 35) },
	{ MAKE_ID(STD_TIMEZONE_ID, 112), MAKE_ID(DST_TIMEZONE_ID, 38) },
	{ MAKE_ID(STD_TIMEZONE_ID, 139), MAKE_ID(DST_TIMEZONE_ID, 39) },
	{ MAKE_ID(STD_TIMEZONE_ID, 142), MAKE_ID(DST_TIMEZONE_ID, 40) },
	{ MAKE_ID(STD_TIMEZONE_ID, 148), MAKE_ID(DST_TIMEZONE_ID, 43) },
	{ MAKE_ID(STD_TIMEZONE_ID, 153), MAKE_ID(DST_TIMEZONE_ID, 44) },
	{ MAKE_ID(STD_TIMEZONE_ID, 154), MAKE_ID(DST_TIMEZONE_ID, 45) },
	{ MAKE_ID(STD_TIMEZONE_ID, 156), MAKE_ID(DST_TIMEZONE_ID, 46) },
	{ MAKE_ID(STD_TIMEZONE_ID, 64), MAKE_ID(DST_TIMEZONE_ID, 47) },
	{ MAKE_ID(STD_TIMEZONE_ID, 116), MAKE_ID(DST_TIMEZONE_ID, 48) },
	{ MAKE_ID(STD_TIMEZONE_ID, 33), MAKE_ID(DST_TIMEZONE_ID, 49) }
	});


bool WorldLocation::InsideCanada() const {
	return ((WorldLocation *)this)->InsideCanada(_latitude, _longitude);
}


bool WorldLocation::InsideCanada(const double latitude, const double longitude) {
	if (latitude < DEGREE_TO_RADIAN(41.0))		return false;
	if (latitude > DEGREE_TO_RADIAN(83.0))		return false;
	if (longitude < DEGREE_TO_RADIAN(-141.0))	return false;
	if (longitude > DEGREE_TO_RADIAN(-52.0))	return false;

#if defined(_MSC_VER) || defined(_USE_CANADA)
	return insideCanadaDetail(&canada, RADIAN_TO_DEGREE(latitude), RADIAN_TO_DEGREE(longitude));
#else
	if (longitude < DEGREE_TO_RADIAN(-122.8)) {
		if (latitude < DEGREE_TO_RADIAN(48.3))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-95.153)) {
		if (latitude < DEGREE_TO_RADIAN(49.0))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-88.0)) {
		if (latitude < DEGREE_TO_RADIAN(48.0))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-83.5)) {
		if (latitude < DEGREE_TO_RADIAN(45.5))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-78.7)) {
		if (latitude < DEGREE_TO_RADIAN(41.66))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-74.75)) {
		if (latitude < DEGREE_TO_RADIAN(43.65))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-67.31)) {
		if (latitude < DEGREE_TO_RADIAN(45))	return false;
	}
	else {
		if (latitude < DEGREE_TO_RADIAN(43.25))	return false;
	}
	return true;
#endif
}


bool WorldLocation::InsideNewZealand() const {
	if ((_longitude > DEGREE_TO_RADIAN(172.5)) && (_longitude < DEGREE_TO_RADIAN(178.6))) {
		if ((_latitude > DEGREE_TO_RADIAN(-41.75)) && (_latitude < DEGREE_TO_RADIAN(-34.3))) {	// general extents of New Zealand's north island
			return true;
		}
	}

	if ((_longitude > DEGREE_TO_RADIAN(166.3)) && (_longitude < DEGREE_TO_RADIAN(174.5))) {
		if ((_latitude > DEGREE_TO_RADIAN(-47.35)) && (_latitude < DEGREE_TO_RADIAN(40.4))) {	// general extents of New Zealand's south island
			return true;
		}
	}
	return false;
}


bool WorldLocation::InsideTasmania() const {
	if ((_longitude > DEGREE_TO_RADIAN(143.5)) && (_longitude < DEGREE_TO_RADIAN(149.0))) {
		if ((_latitude > DEGREE_TO_RADIAN(-44.0)) && (_latitude < DEGREE_TO_RADIAN(-39.5))) {	// general extents of Tasmania
			return true;
		}
	}

	return false;
}


bool WorldLocation::InsideAustraliaMainland() const {
	if ((_longitude > DEGREE_TO_RADIAN(113.15)) && (_longitude < DEGREE_TO_RADIAN(153.633333))) {
		if ((_latitude > DEGREE_TO_RADIAN(-39.133333)) && (_latitude < DEGREE_TO_RADIAN(-10.683333))) {	// general extents of continental Australia
			return true;
		}
	}

	return false;
}


void WorldLocation::SetTimeZoneOffset(const TimeZoneInfo* timezone)
{
	if (timezone) {
		_amtDST = timezone->m_dst;
		__timezone = timezone->m_timezone;
		if (_startDST.GetTotalMicroSeconds() != 0)
			_startDST = WTimeSpan(0);
		if (_amtDST.GetTotalMicroSeconds() != 0)
			_endDST = WTimeSpan(366, 0, 0, 0);
	}
	_timezoneInfo = timezone;
}


bool WorldLocation::SetTimeZoneOffset(std::uint32_t id)
{
	auto tz = TimeZoneFromId(id);
	if (tz)
	{
		SetTimeZoneOffset(tz);
		return true;
	}
	return false;
}


const ::TimeZoneInfo *WorldLocation::GuessTimeZone(INTNM::int16_t set) const {
	bool valid;
	const TimeZoneInfo *tzi = WorldLocation::TimeZoneFromLatLon(_latitude, _longitude, set, &valid);
	if (!valid)
	{
		if (InsideNewZealand()) {
			if (set == 0)
				return &m_std_timezones[15];
			else if (set == 1)
				return &m_dst_timezones[15];
		} else if (InsideTasmania()) {
			if (set == 0)
				return &m_std_timezones[1];
			else if (set == 1)
				return &m_dst_timezones[2];
		}

		double longitude = _longitude;
		while (longitude < -Pi)
			longitude += TwoPi;
		while (longitude > Pi)
			longitude -= TwoPi;

		const ::TimeZoneInfo *tz;
		double variation = TwoPi;
		if (set == 1)		tz = m_dst_timezones;
		else if (set == -1)	tz = m_mil_timezones;
		else if (set == 0)	tz = m_std_timezones;
		else			return NULL;

		while (tz->m_name) {
			double ideal_longitude = ((double)tz->m_timezone.GetTotalSeconds()) / (double)(12.0 * 60.0 * 60.0) * Pi;
			double offset_longitude = fabs(longitude - ideal_longitude);
			if (variation > offset_longitude) {
				variation = offset_longitude;
				tzi = tz;
			}
			tz++;
		}
	}
	return tzi;
}


const ::TimeZoneInfo *WorldLocation::CurrentTimeZone(INTNM::int16_t set, bool* hidden) const
{
	if (_timezoneInfo != nullptr)
	{
		if (hidden)
		{
			if (IS_STD(_timezoneInfo->m_id))
			{
				*hidden = true;
				const ::TimeZoneInfo* tz = m_std_timezones;
				while (tz->m_code)
				{
					if (tz->m_id == _timezoneInfo->m_id)
					{
						*hidden = false;
						break;
					}
					tz++;
				}
			}
			else if (IS_DST(_timezoneInfo->m_id))
			{
				*hidden = true;
				const ::TimeZoneInfo* tz = m_dst_timezones;
				while (tz->m_code)
				{
					if (tz->m_id == _timezoneInfo->m_id)
					{
						*hidden = false;
						break;
					}
					tz++;
				}
			}
		}
		return _timezoneInfo;
	}
	else
	{
		const ::TimeZoneInfo* tz;
		const ::TimeZoneInfo* tzSecondary = nullptr;
		if (set == -1)
			tz = m_mil_timezones;
		else if (_startDST == _endDST)
		{
			tz = m_std_timezones;
			tzSecondary = m_std_extra_timezones;
		}
		else
		{
			tz = m_dst_timezones;
			tzSecondary = m_dst_extra_timezones;
		}

		while (tz->m_code)
		{
			if (tz->m_timezone == __timezone)
				break;
			tz++;
		}
		if (tz->m_code)
		{
			if (hidden)
				*hidden = false;
			return tz;
		}
		else if (tzSecondary)
		{
			while (tzSecondary->m_code)
			{
				if (tzSecondary->m_timezone == __timezone)
					break;
				tzSecondary++;
			}
			if (tzSecondary->m_code)
			{
				if (hidden)
					*hidden = true;
				return tzSecondary;
			}
		}
		return nullptr;
	}
}

const ::TimeZoneInfo* WorldLocation::TimeZoneFromName(const std::string& name, INTNM::int16_t set, bool* hidden)
{
	const ::TimeZoneInfo *tz;
	const ::TimeZoneInfo* tzSecondary = nullptr;
	if (set == -1)
		tz = m_mil_timezones;
	else if (set)
	{
		tz = m_dst_timezones;
		tzSecondary = m_dst_extra_timezones;
	}
	else
	{
		tz = m_std_timezones;
		tzSecondary = m_std_extra_timezones;
	}

	while (tz->m_code)
	{
		if (boost::iequals(tz->m_code, name) || boost::iequals(tz->m_name, name))
			break;
		tz++;
	}
	if (tz->m_code)
	{
		if (hidden)
			*hidden = false;
		return tz;
	}
	else if (tzSecondary)
	{
		while (tzSecondary->m_code)
		{
			if (boost::iequals(tzSecondary->m_code, name) || boost::iequals(tzSecondary->m_name, name))
				break;
			tzSecondary++;
		}
		if (tzSecondary->m_code)
		{
			if (hidden)
				*hidden = true;
			return tzSecondary;
		}
	}
	return nullptr;
}


const ::TimeZoneInfo* WorldLocation::TimeZoneFromId(std::uint32_t id, bool* hidden)
{
	if (IS_STD(id))
	{
		const TimeZoneInfo* tz = m_std_timezones;
		while (tz->m_name)
		{
			if (tz->m_id == id)
			{
				if (hidden)
					*hidden = false;
				return tz;
			}
			tz++;
		}
		tz = m_std_extra_timezones;
		while (tz->m_name)
		{
			if (tz->m_id == id)
			{
				if (hidden)
					*hidden = true;
				return tz;
			}
			tz++;
		}
	}
	else if (IS_DST(id))
	{
		const TimeZoneInfo* tz = m_dst_timezones;
		while (tz->m_name)
		{
			if (tz->m_id == id)
			{
				if (hidden)
					*hidden = false;
				return tz;
			}
			tz++;
		}
		tz = m_dst_extra_timezones;
		while (tz->m_name)
		{
			if (tz->m_id == id)
			{
				if (hidden)
					*hidden = true;
				return tz;
			}
			tz++;
		}
	}
	else
	{
		const TimeZoneInfo* tz = m_mil_timezones;
		while (tz->m_name)
		{
			if (tz->m_id == id)
			{
				if (hidden)
					*hidden = false;
				return tz;
			}
			tz++;
		}
	}
	return nullptr;
}


const ::TimeZoneInfo* WorldLocation::TimeZoneFromWindowsName(const std::string& name)
{
	for (auto windows : WindowsMap)
	{
		if (boost::equals(windows.WindowsName, name))
			return TimeZoneFromId(windows.HssId);
	}
	return nullptr;
}


const ::TimeZoneInfo* WorldLocation::TimeZoneFromRegionName(const std::string& regionName)
{
	for (auto region : RegionMap)
	{
		if (boost::iequals(region.Name, regionName))
			return TimeZoneFromId(region.HssId);
	}
	return nullptr;
}


const TimeZoneInfo* WorldLocation::GetDaylightSavingsTimeZone(const TimeZoneInfo* info)
{
	if (bimapContainsLeft(daylightStandardMap, info->m_id))
		return TimeZoneFromId(daylightStandardMap.left.at(info->m_id));
	return info;
}


const TimeZoneInfo* WorldLocation::GetStandardTimeZone(const TimeZoneInfo* info)
{
	if (bimapContainsRight(daylightStandardMap, info->m_id))
		return TimeZoneFromId(daylightStandardMap.right.at(info->m_id));
	return info;
}


WorldLocation::WorldLocation()
	: _timezoneInfo(nullptr)
#ifdef HSS_USE_CACHING
	, m_sunCache(2), m_solarCache(2)
#endif
{
	_latitude = 1000.0;
	_longitude = 1000.0;
	__timezone = WTimeSpan(0);
	_startDST = WTimeSpan(0);
	_endDST = WTimeSpan(0);
	_amtDST = WTimeSpan(0, 1, 0, 0);

	canada = nullptr;
}


WorldLocation::WorldLocation(const WorldLocation &wl)
	: _timezoneInfo(nullptr)
#ifdef HSS_USE_CACHING
	, m_sunCache(2), m_solarCache(2)
#endif
{
	*this = wl;

	canada = nullptr;
}


WorldLocation::WorldLocation(double latitude, double longitude, bool guessTimezone)
	: _timezoneInfo(nullptr)
#ifdef HSS_USE_CACHING
	, m_sunCache(2), m_solarCache(2)
#endif
{
	_latitude = DEGREE_TO_RADIAN(latitude);
	_longitude = DEGREE_TO_RADIAN(longitude);
	if (guessTimezone)
	{
		_timezoneInfo = GuessTimeZone(0);
		__timezone = _timezoneInfo->m_timezone;
		if (_timezoneInfo->m_dst.GetTotalSeconds() > 0)
		{
			_startDST = WTimeSpan(0);
			_endDST = WTimeSpan(366, 0, 0, 0);
			_amtDST = _timezoneInfo->m_dst;
		}
	}

	canada = nullptr;
}


WorldLocation::~WorldLocation() {
#if defined(_MSC_VER) || defined(_USE_CANADA)
	insideCanadaCleanup(canada);
#endif
}


WorldLocation &WorldLocation::operator=(const WorldLocation &wl) {
	if (&wl != this) {
		_latitude = wl._latitude;
		_longitude = wl._longitude;
		__timezone = wl.__timezone;
		_startDST = wl._startDST;
		_endDST = wl._endDST;
		_amtDST = wl._amtDST;

#ifdef HSS_USE_CACHING
		m_sunCache.Clear();
		m_solarCache.Clear();
#endif

	}
	return *this;
}


bool WorldLocation::operator==(const WorldLocation &wl) const {
	if (&wl == this)
		return true;
	if ((_latitude == wl._latitude) &&
	    (_longitude == wl._longitude) &&
	    (__timezone == wl.__timezone) &&
	    (_startDST == wl._startDST) &&
	    (_endDST == wl._endDST) &&
	    (_amtDST == wl._amtDST))
		return true;
	return false;
}


bool WorldLocation::operator!=(const WorldLocation &wl) const {
	return !(*this == wl);
}


WTimeSpan WorldLocation::m_solar_timezone(const WTime &solar_time) const {
#ifdef HSS_USE_CACHING
	struct sun_key sk;
	sk.m_sun_cache_lat = m_latitude;
	sk.m_sun_cache_long = m_longitude;
	sk.m_sun_cache_tm = solar_time.GetTime(0);
	WTimeSpan retval;
	if (m_solarCache.Retrieve(&sk, &retval))
		return retval;
#endif

	INTNM::int32_t	day = solar_time.GetDay(WTIME_FORMAT_AS_LOCAL),
		year = solar_time.GetYear(WTIME_FORMAT_AS_LOCAL),
		month = solar_time.GetMonth(WTIME_FORMAT_AS_LOCAL);

	CSunriseSunsetCalc calculator;
	RISESET_IN_STRUCT sInput;
	sInput.Latitude = RADIAN_TO_DEGREE(_latitude);
	sInput.Longitude = -RADIAN_TO_DEGREE(_longitude);
	sInput.timezone = 0;
	sInput.DaytimeSaving = false;
	sInput.year = year;
	sInput.month = month;
	sInput.day = day;
	RISESET_OUT_STRUCT sOut;
	calculator.calcSun(sInput,&sOut);

	WTimeSpan solarTime(0, sOut.SolarNoonHour - 12, sOut.SolarNoonMin, (INTNM::int32_t)sOut.SolarNoonSec);
	WTimeSpan result((INTNM::int64_t)0 - solarTime.GetTotalSeconds());
	
#ifdef HSS_USE_CACHING
	m_solarCache.Store(&sk, &result);
#endif
	return result;
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
CArchive& HSS_Time::operator>>(CArchive& is, WorldLocation &wl) {
	union {
		short svalue[4];
		double dvalue;
	} loader;
	is >> loader.svalue[0] >> loader.svalue[1];
	if (loader.svalue[0] == -1) {
		if ((loader.svalue[1] < 1) || (loader.svalue[1] > 6))
			AfxThrowArchiveException(CArchiveException::badSchema, _T("World Location"));
		bool found = false;
		if (loader.svalue[1] >= 5) {
			uint32_t id;
			is >> id;
			if (id != (uint32_t)-1) {
				wl.SetTimeZoneOffset(id);
				found = true;
			}
		}
		is >> wl._latitude >> wl._longitude;
		if (!found) {
			if (loader.svalue[1] < 3) {
				INTNM::int32_t timezone;
				is >> timezone;
				wl.__timezone = WTimeSpan(timezone);
			} else	is >> wl.__timezone;
			if (loader.svalue[1] < 4) {
				INTNM::int16_t spheroid;
				is >> spheroid;
			}
			if (loader.svalue[1] == 2) {
				INTNM::int32_t ts;
				is >> ts; wl._startDST = WTimeSpan(ts);
				is >> ts; wl._endDST = WTimeSpan(ts);
				is >> ts; wl._amtDST = WTimeSpan(ts);
			} else if (loader.svalue[1] >= 3)
				is >> wl._startDST >> wl._endDST >> wl._amtDST;

			// try to guess what the timezone ID is
			const ::TimeZoneInfo* tz;
			if ((wl._amtDST.GetTotalSeconds() == 0) && (!wl.dstExists()))	tz = wl.m_std_timezones;
			else															tz = wl.m_dst_timezones;
			while (tz->m_name) {
				if ((tz->m_timezone == wl.__timezone) && (tz->m_dst == wl._amtDST)) {
					wl._timezoneInfo = tz;
					break;
				}
				tz++;
			}
		}
	} else {
		is >> loader.svalue[2] >> loader.svalue[3];
		wl._latitude = loader.dvalue;
		is >> wl._longitude >> wl.__timezone;
	}
	return is;
}


CArchive& HSS_Time::operator<<(CArchive& os, const WorldLocation &wl) {
	short hickup = -1;
	short version = 6;		// 4 removed m_spheroid
							// 5 added id
							// 6 re-added latitude, longitude
	os << hickup << version;
	uint32_t id;
	if (wl.m_timezoneInfo()) {
		id = wl.m_timezoneInfo()->m_id;
		os << id;
		os << wl._latitude << wl._longitude;
	} else {
		id = (uint32_t)-1;
		os << id << wl._latitude << wl._longitude << wl.__timezone;
		os << wl._startDST << wl._endDST << wl._amtDST;
	}
	return os;
}
#endif


INTNM::int16_t WorldLocation::m_sun_rise_set(const WTime &daytime, WTime *Rise, WTime *Set, WTime *Noon) const {
#ifdef HSS_USE_CACHING
	struct sun_key sk;
	sk.m_sun_cache_lat = m_latitude;
	sk.m_sun_cache_long = m_longitude;
	sk.m_sun_cache_tm = daytime.GetTime(0);
	struct sun_val sv;
	if (m_sunCache.Retrieve(&sk, &sv)) {
		*Rise = WTime(sv.m_sun_cache_rise, Rise->GetTimeManager());
		*Set = WTime(sv.m_sun_cache_set, Set->GetTimeManager());
		*Noon = WTime(sv.m_sun_cache_noon, Noon->GetTimeManager());
		return sv.m_success;
	}
#endif

	INTNM::int32_t	day = daytime.GetDay(WTIME_FORMAT_AS_SOLAR),
		year = daytime.GetYear(WTIME_FORMAT_AS_SOLAR),
		month = daytime.GetMonth(WTIME_FORMAT_AS_SOLAR);

	CSunriseSunsetCalc calculator;
	RISESET_IN_STRUCT sInput;
	sInput.Latitude = RADIAN_TO_DEGREE(_latitude);
	sInput.Longitude = -RADIAN_TO_DEGREE(_longitude);
	sInput.timezone = 0;
	sInput.DaytimeSaving = false;
	sInput.year = year;
	sInput.month = month;
	sInput.day = day;
	RISESET_OUT_STRUCT sOut;
	INTNM::int16_t success = calculator.calcSun(sInput,&sOut);

	WTime riseTime(0ULL, Rise->GetTimeManager()),
		setTime(0ULL, Set->GetTimeManager()),
		noonTime(0ULL, Noon->GetTimeManager());
	if (!(success & NO_SUNRISE))
		riseTime = WTime(sOut.YearRise,sOut.MonthRise,sOut.DayRise,sOut.HourRise,sOut.MinRise,(INTNM::int32_t)sOut.SecRise,Rise->GetTimeManager());
	*Rise = riseTime;
	if (!(success & NO_SUNSET))
		setTime = WTime(sOut.YearSet,sOut.MonthSet,sOut.DaySet,sOut.HourSet,sOut.MinSet,(INTNM::int32_t)sOut.SecSet,Set->GetTimeManager());
	*Set = setTime;
	noonTime = WTime(sInput.year,sInput.month,sInput.day, sOut.SolarNoonHour, sOut.SolarNoonMin, (INTNM::int32_t)sOut.SolarNoonSec,Noon->GetTimeManager());
	*Noon = noonTime;
		
#ifdef HSS_USE_CACHING
	sk.m_sun_cache_tm = daytime.GetTime(0);
	sk.m_sun_cache_lat = m_latitude;
	sk.m_sun_cache_long = m_longitude;
	sv.m_sun_cache_rise = Rise->GetTotalSeconds();
	sv.m_sun_cache_set = Set->GetTotalSeconds();
	sv.m_sun_cache_noon = Noon->GetTotalSeconds();
	sv.m_success = success;
	m_sunCache.Store(&sk, &sv);
#endif
	return success;
}
