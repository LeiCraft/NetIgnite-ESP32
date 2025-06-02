#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include "utils/regexp.h"

class InputValidation {
  public:

    static bool macAddressDashed(const char *macAddress) {
        if (!macAddress) return false;

        const char* pattern = "^([0-9A-Fa-f]{2}-){5}[0-9A-Fa-f]{2}$";
        return RegExp::match(macAddress, pattern);
    }

    static bool macAddressColon(const char *macAddress) {
        if (!macAddress) return false;

        const char* pattern = "^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$";
        return RegExp::match(macAddress, pattern);
    }

    static bool macAddress(const char *macAddress) {
		return InputValidation::macAddressColon(macAddress) || InputValidation::macAddressDashed(macAddress);
	}

    
	static bool port(uint16_t port) {
		return port > 0 && port <= 65535;
	}

};


#endif // INPUT_VALIDATION_H
