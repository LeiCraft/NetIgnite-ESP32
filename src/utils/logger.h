#ifndef LOGGER_H
#define LOGGER_H

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

    static void print(const String &msg) { Serial.print(msg); }
    static void println(const String &msg) { Serial.println(msg); }

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
};

#endif // LOGGER_H