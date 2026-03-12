#pragma once

// Clock

// Required libraries
#include <Wire.h>
#include <DS3231.h>
// Documentation of the RTC: https://github.com/NorthernWidget/DS3231/tree/master

// RTC class
class Clock
{
public:
    static void init();
    static String readClock();
    static void writeClock(int year, int month, int day, int hour, int minute, int second);
    static void test();

    static int year(){
        return rtc.getYear();
    }

    static int month(){
        bool Century;
        return rtc.getMonth(Century);
    }

    static int day(){
        return rtc.getDate();
    }

    static int hour(){
        bool h12, PM_time;
        return rtc.getHour(h12, PM_time);
    }

    static int minute(){
        return rtc.getMinute();
    }

    static int second(){
        return rtc.getSecond();
    }

    static DS3231 rtc;
};