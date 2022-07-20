#include <gtest/gtest.h>

#include <iostream>

#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>

#include "WTime.h"

using namespace HSS_Time;
using namespace google::protobuf;
using namespace google::protobuf::util;


namespace
{
TEST(WTimeTest, WTimeToJsonTest)
{
    WorldLocation location;
    location.m_timezone(WTimeSpan(0, -7, 0, 0.0));
    WTimeManager manager(location);
    WTime time = WTime(2018, 10, 4, 4, 12, 15, &manager);

    Message* message = HSS_Time::Serialization::TimeSerializer().serializeTime(time);

    EXPECT_FALSE(message == nullptr);

    std::string json;
    JsonOptions options;
    options.add_whitespace = true;

    MessageToJsonString(*message, &json, options);
    EXPECT_TRUE(json.length() > 0);

    delete message;
}

/**
 * Test serialization of a time with a standard timezone offset and no DST.
 */
TEST(WTimeTest, SerializationTest1)
{
    WorldLocation location;
    location.m_timezone(WTimeSpan(0, -7, 0, 0.0));
    WTimeManager manager(location);
    WTime time = WTime(2018, 10, 4, 4, 12, 15, &manager);

    //serialize the time
    Message* message = HSS_Time::Serialization::TimeSerializer::serializeTime(time);

    EXPECT_FALSE(message == nullptr);

    //deserialize the timezone information
    WorldLocation location2;
    auto data = HSS_Time::Serialization::TimeSerializer::deserializeTime(*dynamic_cast<HSS::Times::WTime*>(message), nullptr, "");
    location2.m_timezone(WTimeSpan(data.offset));
    if (data.dst)
    {
        location2.m_endDST(WTimeSpan(366, 0, 0, 0));
        location2.m_amtDST(WTimeSpan(data.dst));
    }
    WTimeManager manager2(location2);
    //deserialize the time using the deserialized timezone information
    WTime* time2 = HSS_Time::Serialization::TimeSerializer::deserializeTime(*dynamic_cast<HSS::Times::WTime*>(message), &manager2, nullptr, "");

    //make sure the times before and after serialization are the same
    EXPECT_EQ(time.GetTotalMicroSeconds(), time2->GetTotalMicroSeconds());
    //make sure the timezones before and after serialization are the same
    EXPECT_EQ(
    (std::int32_t)(time.GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time.GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()),
    (std::int32_t)(time2->GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time2->GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()));

    delete time2;
    delete message;
}

/**
 * Test serialization of a time with a standard timezone offset and DST.
 */
TEST(WTimeTest, SerializationTest2)
{
    WorldLocation location;
    location.m_timezone(WTimeSpan(0, -6, 0, 0.0));
    location.m_endDST(WTimeSpan(366, 0, 0, 0));
    location.m_amtDST(WTimeSpan(0, 1, 0, 0));
    WTimeManager manager(location);
    WTime time = WTime(2018, 10, 4, 4, 12, 15, &manager);

    //serialize the time
    Message* message = HSS_Time::Serialization::TimeSerializer::serializeTime(time);

    EXPECT_FALSE(message == nullptr);

    //deserialize the timezone information
    WorldLocation location2;
    auto data = HSS_Time::Serialization::TimeSerializer::deserializeTime(*dynamic_cast<HSS::Times::WTime*>(message), nullptr, "");
    location2.m_timezone(WTimeSpan(data.offset));
    if (data.dst)
    {
        location2.m_endDST(WTimeSpan(366, 0, 0, 0));
        location2.m_amtDST(WTimeSpan(data.dst));
    }
    WTimeManager manager2(location2);
    //deserialize the time using the deserialized timezone information
    WTime* time2 = HSS_Time::Serialization::TimeSerializer::deserializeTime(*dynamic_cast<HSS::Times::WTime*>(message), &manager2, nullptr, "");

    //make sure the times before and after serialization are the same
    EXPECT_EQ(time.GetTotalMicroSeconds(), time2->GetTotalMicroSeconds());
    //make sure the timezones before and after serialization are the same
    EXPECT_EQ(
        (std::int32_t)(time.GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time.GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()),
        (std::int32_t)(time2->GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time2->GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()));

    delete time2;
    delete message;
}

/**
 * Test serialization of a time with a non-standard timezone offset and no DST.
 */
TEST(WTimeTest, SerializationTest3)
{
    WorldLocation location;
    location.m_timezone(WTimeSpan(0, -6, -14, 0.0));
    WTimeManager manager(location);
    WTime time = WTime(2018, 10, 4, 4, 12, 15, &manager);

    //serialize the time
    Message* message = HSS_Time::Serialization::TimeSerializer::serializeTime(time);

    EXPECT_FALSE(message == nullptr);

    //deserialize the timezone information
    WorldLocation location2;
    auto data = HSS_Time::Serialization::TimeSerializer::deserializeTime(*dynamic_cast<HSS::Times::WTime*>(message), nullptr, "");
    location2.m_timezone(WTimeSpan(data.offset));
    if (data.dst)
    {
        location2.m_endDST(WTimeSpan(366, 0, 0, 0));
        location2.m_amtDST(WTimeSpan(data.dst));
    }
    WTimeManager manager2(location2);
    //deserialize the time using the deserialized timezone information
    WTime* time2 = HSS_Time::Serialization::TimeSerializer::deserializeTime(*dynamic_cast<HSS::Times::WTime*>(message), &manager2, nullptr, "");

    //make sure the times before and after serialization are the same
    EXPECT_EQ(time.GetTotalMicroSeconds(), time2->GetTotalMicroSeconds());
    //make sure the timezones before and after serialization are the same
    EXPECT_EQ(
        (std::int32_t)(time.GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time.GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()),
        (std::int32_t)(time2->GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time2->GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()));

    delete time2;
    delete message;
}

/**
 * Test serialization of a time with a non-standard timezone offset and DST.
 */
TEST(WTimeTest, SerializationTest4)
{
    WorldLocation location;
    location.m_timezone(WTimeSpan(0, -6, -14, 0.0));
    location.m_endDST(WTimeSpan(366, 0, 0, 0));
    location.m_amtDST(WTimeSpan(0, 1, 12, 0));
    WTimeManager manager(location);
    WTime time = WTime(2018, 10, 4, 4, 12, 15, &manager);
    std::string temp1 = time.ToString(WTIME_FORMAT_STRING_ISO8601);

    //serialize the time
    Message* message = HSS_Time::Serialization::TimeSerializer::serializeTime(time);

    EXPECT_FALSE(message == nullptr);

    //deserialize the timezone information
    WorldLocation location2;
    auto data = HSS_Time::Serialization::TimeSerializer::deserializeTime(*dynamic_cast<HSS::Times::WTime*>(message), nullptr, "");
    location2.m_timezone(WTimeSpan(data.offset));
    if (data.dst)
    {
        location2.m_endDST(WTimeSpan(366, 0, 0, 0));
        location2.m_amtDST(WTimeSpan(data.dst));
    }
    WTimeManager manager2(location2);
    //deserialize the time using the deserialized timezone information
    WTime* time2 = HSS_Time::Serialization::TimeSerializer::deserializeTime(*dynamic_cast<HSS::Times::WTime*>(message), &manager2, nullptr, "");
    std::string temp2 = time2->ToString(WTIME_FORMAT_STRING_ISO8601);

    //make sure the times before and after serialization are the same
    EXPECT_EQ(time.GetTotalMicroSeconds(), time2->GetTotalMicroSeconds());
    //make sure the timezones before and after serialization are the same
    EXPECT_EQ(
        (std::int32_t)(time.GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time.GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()),
        (std::int32_t)(time2->GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time2->GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()));

    delete time2;
    delete message;
}

// /**
//  * Test serialization of a time with a timezone different than the one in the ISO8601 string.
//  */
// TEST(WTimeTest, SerializationTest5)
// {
//     WorldLocation location;
//     location.m_timezone(WTimeSpan(0, -5, 0, 0.0));
//     location.m_endDST(WTimeSpan(366, 0, 0, 0));
//     location.m_amtDST(WTimeSpan(0, 1, 0, 0));
//     WTimeManager manager(location);
//     WTime time = WTime(2018, 10, 4, 4, 12, 15, &manager);

//     //serialize the time
//     HSS::Times::WTime* message = HSS_Time::Serialization::TimeSerializer::serializeTime(time);
//     //change the timezone to be different from the ISO8601 string
//     auto daylight = new google::protobuf::StringValue();
//     daylight->set_value("01:00");
//     message->set_allocated_daylight(daylight);
//     message->mutable_timezone()->set_value("-6:00");

//     EXPECT_FALSE(message == nullptr);

//     //deserialize the timezone information
//     WorldLocation location2;
//     auto data = HSS_Time::Serialization::TimeSerializer::deserializeTime(*message, nullptr, "");
//     location2.m_timezone(WTimeSpan(data.offset));
//     if (data.dst)
//     {
//         location2.m_endDST(WTimeSpan(366, 0, 0, 0));
//         location2.m_amtDST(WTimeSpan(data.dst));
//     }
//     WTimeManager manager2(location2);
//     //deserialize the time using the deserialized timezone information
//     WTime* time2 = HSS_Time::Serialization::TimeSerializer::deserializeTime(*message, &manager2, nullptr, "");

//     //make sure the times before and after serialization are the same
//     EXPECT_EQ(time.GetTotalMicroSeconds(), time2->GetTotalMicroSeconds());
//     //make sure the timezones before and after serialization are the same
//     EXPECT_EQ(
//         -18'000,
//         (std::int32_t)(time2->GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time2->GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()));

//     delete time2;
//     delete message;
// }

// /**
//  * Test deserialization of a time with no timezone information in the ISO8601 string.
//  */
// TEST(WTimeTest, SerializationTest6)
// {
//     //serialize the time
//     HSS::Times::WTime* message = new HSS::Times::WTime();
//     //change the timezone to be different from the ISO8601 string
//     auto daylight = new google::protobuf::StringValue();
//     daylight->set_value("01:00");
//     message->set_allocated_daylight(daylight);
//     auto timezone = new google::protobuf::StringValue();
//     timezone->set_value("-5:00");
//     message->set_allocated_timezone(timezone);
//     message->set_time("2018-10-04T00:12:15");

//     EXPECT_FALSE(message == nullptr);

//     //deserialize the timezone information
//     WorldLocation location2;
//     auto data = HSS_Time::Serialization::TimeSerializer::deserializeTime(*message, nullptr, "");
//     location2.m_timezone(WTimeSpan(data.offset));
//     if (data.dst)
//     {
//         location2.m_endDST(WTimeSpan(366, 0, 0, 0));
//         location2.m_amtDST(WTimeSpan(data.dst));
//     }
//     WTimeManager manager2(location2);
//     //deserialize the time using the deserialized timezone information
//     WTime* time2 = HSS_Time::Serialization::TimeSerializer::deserializeTime(*message, &manager2, nullptr, "");

//     //make sure the times before and after serialization are the same
//     EXPECT_EQ(13214722335000000UL, time2->GetTotalMicroSeconds());
//     //make sure the timezones before and after serialization are the same
//     EXPECT_EQ(
//         -14'400,
//         (std::int32_t)(time2->GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time2->GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()));

//     delete time2;
//     delete message;
// }

/**
 * Test serialization of a time with a timezone different than the one in the ISO8601 string.
 */
TEST(WTimeTest, SerializationTest7)
{
    WorldLocation location;
    location.m_timezone(WTimeSpan(0, 0, 0, 0.0));
    WTimeManager manager(location);
    WTime time = WTime(2018, 10, 4, 4, 12, 15, &manager);
    std::string temp1 = time.ToString(WTIME_FORMAT_STRING_ISO8601);

    //serialize the time
    HSS::Times::WTime* message = HSS_Time::Serialization::TimeSerializer::serializeTime(time);
    //change the timezone to be different from the ISO8601 string
    auto daylight = new google::protobuf::StringValue();
    daylight->set_value("01:00");
    message->set_allocated_daylight(daylight);
    message->mutable_timezone()->set_value("-6:00");

    EXPECT_FALSE(message == nullptr);

    //deserialize the timezone information
    WorldLocation location2;
    auto data = HSS_Time::Serialization::TimeSerializer::deserializeTime(*message, nullptr, "");
    location2.m_timezone(WTimeSpan(data.offset));
    if (data.dst)
    {
        location2.m_endDST(WTimeSpan(366, 0, 0, 0));
        location2.m_amtDST(WTimeSpan(data.dst));
    }
    WTimeManager manager2(location2);
    //deserialize the time using the deserialized timezone information
    WTime* time2 = HSS_Time::Serialization::TimeSerializer::deserializeTime(*message, &manager2, nullptr, "");

    //make sure the times before and after serialization are the same
    EXPECT_EQ(time.GetTotalMicroSeconds(), time2->GetTotalMicroSeconds());
    //make sure the timezones before and after serialization are the same
    EXPECT_EQ(
        -18'000,
        (std::int32_t)(time2->GetTimeManager()->m_worldLocation.m_timezone().GetTotalSeconds() + time2->GetTimeManager()->m_worldLocation.m_amtDST().GetTotalSeconds()));

    delete time2;
    delete message;
}

TEST(WTimeTest, ParseTest1)
{
    WorldLocation location;
    location.m_timezone(WTimeSpan(0, 0, 0, 0.0));
    WTimeManager manager(location);
    WTime time = WTime(2018, 10, 4, 4, 12, 15, &manager);
    time.ParseDateTime(std::string("2018-10-22"), WTIME_FORMAT_STRING_ISO8601);

    EXPECT_EQ(2018, time.GetYear(WTIME_FORMAT_AS_LOCAL));
    EXPECT_EQ(10, time.GetMonth(WTIME_FORMAT_AS_LOCAL));
    EXPECT_EQ(22, time.GetDay(WTIME_FORMAT_AS_LOCAL));
    EXPECT_EQ(0, time.GetHour(WTIME_FORMAT_AS_LOCAL));
    EXPECT_EQ(0, time.GetMinute(WTIME_FORMAT_AS_LOCAL));
}

TEST(WTimeTest, ParseTest2)
{
	WorldLocation location;
	location.m_timezone(WTimeSpan(0, 5, 30, 0.0));
	WTimeManager manager(location);
	WTime time = WTime(&manager);
	time.ParseDateTime(std::string("2018-01-20T12:31:00Z"), WTIME_FORMAT_STRING_ISO8601);
	std::string dt = time.ToString(WTIME_FORMAT_STRING_ISO8601);

	EXPECT_STREQ("2018-01-20T18:01:00+05:30", dt.c_str());
}

TEST(WTimeTest, ParseTest3)
{
	WorldLocation location;
	location.m_timezone(WTimeSpan(0, 5, 30, 0.0));
	WTimeManager manager(location);
	WTime time = WTime(&manager);
	time.ParseDateTime(std::string("2018-01-20T12:31:00"), WTIME_FORMAT_STRING_ISO8601);
	std::string dt = time.ToString(WTIME_FORMAT_STRING_ISO8601);

	EXPECT_STREQ("2018-01-20T12:31:00+05:30", dt.c_str());
}
}