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

	static bool ipv4Address(const char *ipAddress) {
		if (!ipAddress) return false;

		const char* pattern = "^(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])(\\.(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])){3}$";
		return RegExp::match(ipAddress, pattern);
	}

	static bool ipv6Address(const char *ipAddress) {
		if (!ipAddress) return false;

		const char* pattern = "^(([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}|::|(([0-9a-fA-F]{1,4}:){1,7}:)|(([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4})|(([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2})|(([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3})|(([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4})|(([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5})|([0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6}))|(:((:[0-9a-fA-F]{1,4}){1,7}|:))|(::ffff:(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])(\\.(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])){3}))$";
		return RegExp::match(ipAddress, pattern);
	}

	static bool ipAddress(const char *ipAddress) {
		return InputValidation::ipv4Address(ipAddress) || InputValidation::ipv6Address(ipAddress);
	}

};


#endif // INPUT_VALIDATION_H
