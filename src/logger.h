#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum class LogLevel : uint8_t
{
    FATAL = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4,
};

class Logger
{
public:
    static inline LogLevel level = LogLevel::INFO;

    static void init(unsigned long baud = 115200)
    {
        Serial.begin(baud);
        while (!Serial)
            delay(10);
    }

    static void setLevel(LogLevel lvl)
    {
        level = lvl;
    }

    static void log(LogLevel msgLevel, const String &msg)
    {
        if (msgLevel < level)
            return;
        Serial.print("[");
        Serial.print(toStr(msgLevel));
        Serial.print("] ");
        Serial.println(msg);
    }

    static void debug(const String &msg) { log(LogLevel::DEBUG, msg); }
    static void info(const String &msg) { log(LogLevel::INFO, msg); }
    static void warn(const String &msg) { log(LogLevel::WARN, msg); }
    static void error(const String &msg) { log(LogLevel::ERROR, msg); }
    static void fatal(const String &msg) { log(LogLevel::FATAL, msg); }

private:
    static const char *toStr(LogLevel lvl)
    {
        switch (lvl)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }
};