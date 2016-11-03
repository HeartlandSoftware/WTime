[![Apache 2](https://img.shields.io/badge/license-Apache%202-blue.svg)](https://github.com/HeartlandSoftware/WTime/blob/master/LICENSE)

# WTime

WTime stands for "WorldTime".

This is a C++ and Java time library for representing date/time and timespans with support and conversions to/from local, solar, and GMT/UTC times.  Corrections for daylight savings, etc. are also included but are used less frequently in software for which this package was developed.

This utility provides the following features:

* Microsecond accuracy.  Not all libraries and representations provide accurate arithmetic.  If floating point representations are used, then standard error propagation can occur over repeated operations.  Thus, a 64-bit integer representation is used.

* Useful over long period of times.  The Gregorian calendar was introduced in 1582, so this library arbitrarily starts in the year 1600.  Conversions from the prior calendar require overhead and aren't needed for the intent of this library.  With a 64-bit representation, it is useful for centuries into the future.

* various printing and parsing options for presentation and extraction of date and time fields.  Note there's always room for further improvement here.

* Functionality to determine when the sun rises, sets, and is at solar noon, given a specific location and date.

* Basic serialization.

* Consistency w/r to usage, regardless of the precise time, or timezone, being requested.

* Conversion routines to/from some (not all) other existing time formats.

#### Platform Support

The C++ version includes project files for use in Visual Studio 2010 or newer (tested with both MSVC 2010 and Intel Studio 13.0) and autotools files which have been tested with GCC 4.5.3.

The Java version contains Eclipse project files as well as an Ant build script. It requires at least JRE7. The Java library depends on the [HSS_Java library](https://github.com/HeartlandSoftware/HSS_Java).

## Examples

#### C++

```c++
//Create a time for someone in New York City
WTime time(2016, 2, 11, 6, 0, 0, new TimeManager(WorldLocation(40.7127, -74.0059, true)));
```

```c++
//Construct a time from a string of known format
time.ParseDateTime("2016/02/12 22:30:00", WTIME_FORMAT_STRING_YYYYhMMhDD | WTIME_FORMAT_TIME);
```

```c++
//Print a time to a string
std::string str = time.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_YEAR | WTIME_FORMAT_ABBREV);
//Output from example 2: Feb 12, 2016 22:30:00
```

#### Java

```java
//Java Calendar to WTime
Calendar c = Calendar.getInstance();
WTime time = new WTime(c.get(Calendar.YEAR), c.get(Calendar.MONTH) + 1, c.get(Calendar.DAY_OF_MONTH), c.get(Calendar.HOUR_OF_DAY), c.get(Calendar.MINUTE), c.get(Calendar.SECOND), new WTimeManager(new WorldLocation()));
```

```java
//Create a time span of 5 hours and add it to the current time
WTimeSpan span = new WTimeSpan(0, 5, 0, 0);
time.Add(span);
```

```java
WTime time = new WTime(2016, 2, 11, 9, 30, 0, new WTimeManager(new WorldLocation()));
System.out.println(time.toString(WTIME_FORMAT_AS_LOCAL | WTime.FORMAT_WITHDST | WTime.FORMAT_DATE | WTime.FORMAT_TIME | WTime.FORMAT_YEAR | WTime.FORMAT_ABBREV));
//Output: Feb 11, 2016 9:30:00
```

## Limitations

Times are stored as the number of microseconds since January 1, 1600. It is not possible to store times before January 1, 1600.

No concern for leap seconds.  There is discussion on the utility of these in the real world, so until the two camps come to a conclusion, there's no need for this library to handle them, at this time.

Although many time zones are accounted for, the list may never be deemed complete, nor is there a way to express a preference for one time zone or another, when more than one exists for a given correction from UTC.

Arctic and Antarctic circles may or may not have time zones, depending on the source.  This library hasn't been tested for this range.

The library assumes that the client code will configure it appropriately when used for the timezone and settings of the computer's configuration.  The library was built for projects where a computer may be located in a different timezone than the project data.

## Support

Please use GitHub issues and Pull Requests for support.

## Copyright and license

Copyright 2016 Heartland Software Solutions Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
