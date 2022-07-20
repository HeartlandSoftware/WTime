#include <gtest/gtest.h>

#include <iostream>

#include "WTime.h"

using namespace HSS_Time;


namespace
{
TEST(WTimeSpanTest, ParseTest1)
{
    WTimeSpan span(std::string("0:0:10.5"));

    EXPECT_EQ(0, span.GetDays());
    EXPECT_EQ(0, span.GetHours());
    EXPECT_EQ(0, span.GetMinutes());
    EXPECT_EQ(10, span.GetSeconds());
    EXPECT_EQ(500000, span.GetMicroSeconds());
}

TEST(WTimeSpanTest, ParseTest2)
{
    WTimeSpan span(std::string("3 days 2:45:0"));

    EXPECT_EQ(3, span.GetDays());
    EXPECT_EQ(2, span.GetHours());
    EXPECT_EQ(45, span.GetMinutes());
    EXPECT_EQ(0, span.GetSeconds());
}

TEST(WTimeSpanTest, ParseTest3)
{
	WTimeSpan span(std::string("P3DT1H5M10.5S"));

	EXPECT_EQ(3, span.GetDays());
	EXPECT_EQ(1, span.GetHours());
	EXPECT_EQ(5, span.GetMinutes());
	EXPECT_EQ(10, span.GetSeconds());
	EXPECT_EQ(500, span.GetMilliSeconds());
}

TEST(WTimeSpanTest, ToStringTest1)
{
	WTimeSpan span(56, 8, 33, 12, 475);

	std::string str = span.ToString(WTIME_FORMAT_STRING_ISO8601);
	EXPECT_STREQ(str.c_str(), "P56DT8H33M12.000475S");
}

TEST(WTimeSpanTest, ToStringTest2)
{
	WTimeSpan span(56, 8, 33, 12, 475);

	std::string str = span.ToString(WTIME_FORMAT_YEAR | WTIME_FORMAT_DAY | WTIME_FORMAT_INCLUDE_USECS);
	EXPECT_STREQ(str.c_str(), "56 days 08:33:12.000475");
}

TEST(WTimeSpanTest, ToStringTest3)
{
	WTimeSpan span(56, 8, 33, 12, 47500);

	std::string str = span.ToString(WTIME_FORMAT_STRING_ISO8601);
	EXPECT_STREQ(str.c_str(), "P56DT8H33M12.0475S");
}
}