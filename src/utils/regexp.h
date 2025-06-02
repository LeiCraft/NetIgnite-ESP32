#ifndef REGEXP_H
#define REGEXP_H

#include <regex.h>

class RegExp {
  public:
  
    static bool match(const char* str, const char* pattern) {
        if (!str || !pattern) return false;

        regex_t regex;
        if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
            // regex compilation failed
            return false;
        }

        int result = regexec(&regex, str, 0, NULL, 0);
        regfree(&regex);
        return result == 0;

    }
};

#endif // REGEXP_H
