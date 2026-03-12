#include "Clock.h"

DS3231 Clock::rtc(Wire);

void Clock::init()
{
    Wire.begin();
    rtc.setClockMode(false); // Mode 24h (00:00-23:59)
#if AJUSTAR_RELOJ
    writeClock(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND);
#endif
}

String Clock::readClock()
{
    bool h12, pm, siglo;
    char buf[23];
    snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d ",
             rtc.getYear() + 2000,
             rtc.getMonth(siglo),
             rtc.getDate(),
             rtc.getHour(h12, pm),
             rtc.getMinute(),
             rtc.getSecond());
    return String(buf);
}

void Clock::writeClock(int year, int month, int day, int hour, int minute, int second)
{
    rtc.setYear(year % 100);
    rtc.setMonth(month);
    rtc.setDate(day);
    rtc.setHour(hour);
    rtc.setMinute(minute);
    rtc.setSecond(second);
}

void Clock::test()
{
    Serial.println(readClock());
}