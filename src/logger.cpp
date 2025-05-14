#include "Logger.h"

LogLevel Logger::currentLevel = LogLevel::INFO;

void Logger::init(unsigned long baud) {
  Serial.begin(baud);
  while (!Serial) delay(10);  // wait for Serial to initialize
}

void Logger::setLevel(LogLevel level) {
  currentLevel = level;
}

void Logger::log(LogLevel level, const String& message) {
  if (level < currentLevel) return;

  Serial.print("[");
  Serial.print(levelToString(level));
  Serial.print("] ");
  Serial.println(message);
}

void Logger::debug(const String& message) { log(LogLevel::DEBUG, message); }
void Logger::info(const String& message)  { log(LogLevel::INFO,  message); }
void Logger::warn(const String& message)  { log(LogLevel::WARN,  message); }
void Logger::error(const String& message) { log(LogLevel::ERROR, message); }
void Logger::fatal(const String& message) { log(LogLevel::FATAL, message); }

const char* Logger::levelToString(LogLevel level) {
  switch (level) {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO:  return "INFO";
    case LogLevel::WARN:  return "WARN";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::FATAL: return "FATAL";
    default:              return "UNKNOWN";
  }
}
