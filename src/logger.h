#pragma once

#include <Arduino.h>

enum class LogLevel : uint8_t {
    FATAL = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4,
};

class Logger {
  public:
    static void init(unsigned long baud = 115200) {
        Serial.begin(baud);
        while (!Serial) {
            delay(10);
        }
    }

    static void setLevel(LogLevel lvl) { level = lvl; }
    static LogLevel getLevel() { return level; }

    static void debug(const String &msg) { log(LogLevel::DEBUG, msg); }
    static void info(const String &msg) { log(LogLevel::INFO, msg); }
    static void warn(const String &msg) { log(LogLevel::WARN, msg); }
    static void error(const String &msg) { log(LogLevel::ERROR, msg); }
    static void fatal(const String &msg) { log(LogLevel::FATAL, msg); }

    static void debugln(const String &msg) { logln(LogLevel::DEBUG, msg); }
    static void infoln(const String &msg) { logln(LogLevel::INFO, msg); }
    static void warnln(const String &msg) { logln(LogLevel::WARN, msg); }
    static void errorln(const String &msg) { logln(LogLevel::ERROR, msg); }
    static void fatalln(const String &msg) { logln(LogLevel::FATAL, msg); }

    static void debugf(const char *fmt, ...) { logf(LogLevel::DEBUG, fmt); }
    static void infof(const char *fmt, ...) { logf(LogLevel::INFO, fmt); }
    static void warnf(const char *fmt, ...) { logf(LogLevel::WARN, fmt); }
    static void errorf(const char *fmt, ...) { logf(LogLevel::ERROR, fmt); }
    static void fatalf(const char *fmt, ...) { logf(LogLevel::FATAL, fmt); }


    static void print(const String &msg) { Serial.print(msg); }
    static void println(const String &msg) { Serial.println(msg); }
    static void printf(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        Serial.printf(fmt, args);
        va_end(args);
    }

  private:
    static inline LogLevel level;

    static const char *levelToStr(LogLevel lvl) {
        switch (lvl) {
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

    static void log(LogLevel msgLvl, const String &msg) {
        if (msgLvl < level) return;
        
        Serial.printf("[%s] ", levelToStr(msgLvl));
        Serial.print(msg);
    }

    static void logln(LogLevel msgLvl, const String &msg) {
        if (msgLvl < level) return;

        Serial.printf("[%s] ", levelToStr(msgLvl));
        Serial.println(msg);
    }

    static void logf(LogLevel msgLvl, const char *fmt, ...) {
        if (msgLvl < level) return;

        Serial.printf("[%s] ", levelToStr(msgLvl));

        va_list args;
        va_start(args, fmt);
        Serial.printf(fmt, args);
        va_end(args);
    }
};