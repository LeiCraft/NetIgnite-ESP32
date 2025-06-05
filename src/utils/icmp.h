#ifndef ICMP_H
#define ICMP_H

#include <Arduino.h>
#include <IPAddress.h>
#include <IPv6Address.h>
#include "utils/ping.h"

extern "C" void esp_schedule(void) {};
extern "C" void esp_yield(void) {};


#define DEBUG_PING(...) Serial.printf(__VA_ARGS__)



class ICMP {
  public:

    static bool ping(IPAddress dest, byte count = 5) {
        _expected_count = count;
        _errors = 0;
        _success = 0;

        _avg_time = 0;

        ping_option _options;

        // Repeat count (how many time send a ping message to destination)
        _options.count = count;
        // Time interval between two ping (seconds??)
        _options.coarse_time = 1;
        // Destination machine
        _options.ip = static_cast<uint32_t>(dest);

        // Callbacks
        _options.recv_function = reinterpret_cast<ping_recv_function>(&_ping_recv_cb);
        _options.sent_function = NULL; //reinterpret_cast<ping_sent_function>(&_ping_sent_cb);

        
        // Suspend till the process end
        esp_yield(); // ????????? Where should this be placed?
        
        // Let's go!
        ping_start(&_options); // Here we do all the work

        // Returns true if at least 1 ping had a pong response 
        return (_success > 0); //_success variable is changed by the callback function

    }

    static bool ping(IPv6Address dest, byte count = 5) {
        
        _expected_count = count;
        _errors = 0;
        _success = 0;

        _avg_time = 0;

        ping6_option _options;

        // Repeat count (how many time send a ping message to destination)
        _options.count = count;
        // Time interval between two ping (seconds??)
        _options.coarse_time = 1;
        // Destination machine
        memcpy(_options.ip, static_cast<const uint8_t*>(dest), 16);
        

        // Callbacks
        _options.recv_function = reinterpret_cast<ping_recv_function>(&_ping_recv_cb);
        _options.sent_function = NULL; //reinterpret_cast<ping_sent_function>(&_ping_sent_cb);

        
        // Suspend till the process end
        esp_yield(); // ????????? Where should this be placed?
        
        // Let's go!
        ping6_start(&_options); // Here we do all the work

        // Returns true if at least 1 ping had a pong response 
        return (_success > 0); //_success variable is changed by the callback function

    }

  private:
    static void _ping_recv_cb(void *opt, void *resp) {
        // // Cast the parameters to get some usable info
        // ping_resp *ping_resp = reinterpret_cast<struct ping_resp *>(resp);
        // //ping_option* ping_opt  = reinterpret_cast<struct ping_option*>(opt);

        // // Error or success?
        // _errors = ping_resp->timeout_count;
        // _success = ping_resp->total_count - ping_resp->timeout_count;
        // _avg_time = ping_resp->resp_time;
        
        // // Done, return to main function
        // esp_schedule();

        // Cast the parameters to get some usable info
        ping_resp *ping_resp = reinterpret_cast<struct ping_resp *>(resp);
        //ping_option* ping_opt  = reinterpret_cast<struct ping_option*>(opt);

        // Error or success?
        _errors = ping_resp->timeout_count;
        _success = ping_resp->total_count - ping_resp->timeout_count;
        _avg_time = ping_resp->resp_time;
        

        // Some debug info
        DEBUG_PING(
                "DEBUG: ping reply\n"
                        "\ttotal_count = %d \n"
                        "\tresp_time = %f ms\n"
                        "\tseqno = %d \n"
                        "\ttimeout_count = %d \n"
                        "\tbytes = %d \n"
                        "\ttotal_bytes = %d \n"
                        "\ttotal_time = %f s\n"
                        "\tping_err = %d \n",
                ping_resp->total_count, ping_resp->resp_time, ping_resp->seqno,
                ping_resp->timeout_count, ping_resp->bytes, ping_resp->total_bytes,
                ping_resp->total_time, ping_resp->ping_err
        );

        // Is it time to end?
        DEBUG_PING("Avg resp time %f ms\n", _avg_time);
        
        // Done, return to main function
        esp_schedule();
        
        // just a check ...
        if (_success + _errors != _expected_count) {
            DEBUG_PING("Something went wrong: _success=%d and _errors=%d do not sum up to _expected_count=%d\n",_success, _errors, _expected_count );
        }
    }

    static inline byte _expected_count = 0;
    static inline byte _errors = 0;
    static inline byte _success = 0;
    static inline float _avg_time = 0;

};

void pingTask(void *pvParameters) {
    // This task is used to run the ping process
    // It will be suspended until the ping process is done
    while (true) {

        ICMP::ping(IPAddress(192, 168, 2, 1)); // Example IPv4 address

        const uint8_t ip[16] = { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x7c, 0xf5, 0x9c, 0x9d, 0x77, 0x7a, 0x27, 0xb7 };

        ICMP::ping(IPv6Address(ip));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#endif // ICMP_H