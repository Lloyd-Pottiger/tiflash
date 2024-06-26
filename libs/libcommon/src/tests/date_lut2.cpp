// Copyright 2023 PingCAP, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <common/DateLUT.h>

#include <cstring>
#include <iostream>


static std::string toString(time_t Value)
{
    struct tm tm;
    char buf[96];

    localtime_r(&Value, &tm);
    snprintf(
        buf,
        sizeof(buf),
        "%04d-%02d-%02d %02d:%02d:%02d",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec);

    return buf;
}

static time_t orderedIdentifierToDate(unsigned value)
{
    struct tm tm;

    memset(&tm, 0, sizeof(tm));

    tm.tm_year = value / 10000 - 1900;
    tm.tm_mon = (value % 10000) / 100 - 1;
    tm.tm_mday = value % 100;
    tm.tm_isdst = -1;

    return mktime(&tm);
}


void loop(time_t begin, time_t end, int step)
{
    const auto & date_lut = DateLUT::instance();

    for (time_t t = begin; t < end; t += step)
        std::cout << toString(t) << ", " << toString(date_lut.toTime(t)) << ", " << date_lut.toHour(t) << std::endl;
}


int main(int argc, char ** argv)
{
    loop(orderedIdentifierToDate(20101031), orderedIdentifierToDate(20101101), 15 * 60);
    loop(orderedIdentifierToDate(20100328), orderedIdentifierToDate(20100330), 15 * 60);
    loop(orderedIdentifierToDate(20141020), orderedIdentifierToDate(20141106), 15 * 60);

    return 0;
}
