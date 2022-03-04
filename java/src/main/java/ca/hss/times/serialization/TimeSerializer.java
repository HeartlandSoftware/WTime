/**
 * TimeSerializer.cpp
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

package ca.hss.times.serialization;

import com.google.protobuf.Int32Value;
import com.google.protobuf.StringValue;

import ca.hss.times.TimeZoneInfo;
import ca.hss.times.WTime;
import ca.hss.times.WTimeSpan;
import ca.hss.times.WorldLocation;

public class TimeSerializer {

	public static ca.hss.times.proto.WTime serializeTime(final WTime time) {
		ca.hss.times.proto.WTime.Builder builder = ca.hss.times.proto.WTime.newBuilder();
		
		builder.setTime(time.toString(WTime.FORMAT_STRING_ISO8601));
		if (time.getTimeManager().getWorldLocation().getTimeZoneInfo() != null)
		    builder.setTimezoneId(Int32Value.of(time.getTimeManager().getWorldLocation().getTimeZoneInfo().getUUID()));
		//utc
		if (WTimeSpan.equal(time.getTimeManager().getWorldLocation().getStartDST(), time.getTimeManager().getWorldLocation().getEndDST()) &&
				WTimeSpan.equal(time.getTimeManager().getWorldLocation().getTimezoneOffset(), new WTimeSpan(0))) {
			builder.setTimezone(StringValue.of("UTC"));
		}
		else {
			TimeZoneInfo zone = time.getTimeManager().getWorldLocation().currentTimeZone((short)0);
			if (zone != null)
				builder.setTimezone(StringValue.of(zone.getCode()));
			else {
				builder.setTimezone(StringValue.of(time.getTimeManager().getWorldLocation().getTimezoneOffset().toString(WTime.FORMAT_EXCLUDE_SECONDS)));
				if (!WTimeSpan.equal(time.getTimeManager().getWorldLocation().getEndDST(), time.getTimeManager().getWorldLocation().getStartDST()) &&
						WTimeSpan.greaterThan(time.getTimeManager().getWorldLocation().getDSTAmount(), new WTimeSpan(0))) {
					builder.setDaylight(StringValue.of(time.getTimeManager().getWorldLocation().getDSTAmount().toString(WTime.FORMAT_EXCLUDE_SECONDS)));
				}
			}
		}
		
		return builder.build();
	}
	
	public static ca.hss.times.proto.WTimeSpan serializeTimeSpan(final WTimeSpan span) {
		return ca.hss.times.proto.WTimeSpan.newBuilder().setTime(span.toString(WTime.FORMAT_YEAR | WTime.FORMAT_DAY | WTime.FORMAT_INCLUDE_USECS)).build();
	}
	
	public static ca.hss.times.proto.WTimeZone serializeTimezone(final WorldLocation location) {
	    ca.hss.times.proto.WTimeZone.Builder builder = ca.hss.times.proto.WTimeZone.newBuilder()
	            .setVersion(1);
	    if (location.getTimeZoneInfo() != null) {
	        builder.setTimezoneIndex(location.getTimeZoneInfo().getUUID());
	    }
	    else {
	        builder.getTimezoneDetailsBuilder()
	            .setAmtTimeZone(serializeTimeSpan(location.getTimezoneOffset()))
	            .setStartDST(serializeTimeSpan(location.getStartDST()))
	            .setEndDST(serializeTimeSpan(location.getEndDST()))
	            .setAmtDST(serializeTimeSpan(location.getDSTAmount()));
	    }
	    return builder.build();
	}
}
