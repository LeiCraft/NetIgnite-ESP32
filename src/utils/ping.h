

#include <Arduino.h>
#include <IPv6Address.h>

#include <math.h>
#include <float.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <net/if.h>
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include "lwip/ip4.h"
#include "lwip/ip6.h"
#include "lwip/err.h"
#include "lwip/icmp.h"
#include "lwip/icmp6.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

static uint16_t ping_seq_num;
static uint8_t stopped = 0;

/*
* Statistics
*/
static uint32_t transmitted = 0;
static uint32_t received = 0;
static float min_time = 0;
static float max_time = 0;
static float mean_time = 0;
static float last_mean_time = 0;
static float var_time = 0;

#define PING_ID 0xAFAF

#ifndef PING_DEFAULT_COUNT
#define PING_DEFAULT_COUNT    10
#endif
#ifndef PING_DEFAULT_INTERVAL
#define PING_DEFAULT_INTERVAL  1
#endif
#ifndef PING_DEFAULT_SIZE
#define PING_DEFAULT_SIZE     32
#endif
#ifndef PING_DEFAULT_TIMEOUT
#define PING_DEFAULT_TIMEOUT   1
#endif



typedef void(*ping_recv_function)(void* arg, void *pdata);
typedef void(*ping_sent_function)(void* arg, void *pdata);

struct ping_option {
    uint32_t count;
    uint32_t ip;
    uint32_t coarse_time;
    ping_recv_function recv_function;
    ping_sent_function sent_function;
    void* reverse;
};

struct ping6_option {
    uint32_t count;
    uint32_t ip[4];
    uint32_t coarse_time;
    ping_recv_function recv_function;
    ping_sent_function sent_function;
    void* reverse;
};

struct ping_resp {
    uint32_t total_count;
    float resp_time;
    uint32_t seqno;
    uint32_t timeout_count;
    uint32_t bytes;
    uint32_t total_bytes;
    float total_time;
    int8_t  ping_err;
};

/*
* Helper functions
*
*/
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t len) {
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = PING_ID;
    iecho->seqno = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

static void ping6_prepare_echo(struct icmp6_echo_hdr *iecho, uint16_t len) {
    size_t i;
    size_t data_len = len - sizeof(struct icmp6_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP6_TYPE_EREQ);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = PING_ID;
    iecho->seqno = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp6_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

static err_t ping_send(int s, ip4_addr_t *addr, int size) {
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + size;
    int err;

    iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
    if (!iecho) {
	mem_free(iecho);    
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (uint16_t)ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    inet_addr_from_ip4addr(&to.sin_addr, addr);

    if ((err = sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to)))) {
        transmitted++;
    }
    mem_free(iecho);
    return (err ? ERR_OK : ERR_VAL);
}

static err_t ping6_send(int s, ip6_addr_t *addr, int size) {
    struct icmp6_echo_hdr *iecho;
    struct sockaddr_in6 to;
    size_t ping_size = sizeof(struct icmp6_echo_hdr) + size;
    int err;

    iecho = (struct icmp6_echo_hdr *)mem_malloc((mem_size_t)ping_size);
    if (!iecho) {
	mem_free(iecho);    
        return ERR_MEM;
    }

    ping6_prepare_echo(iecho, (uint16_t)ping_size);

    to.sin6_len = sizeof(to);
    to.sin6_family = AF_INET6;
    to.sin6_scope_id = if_nametoindex("wlan0");
    inet6_addr_from_ip6addr(&to.sin6_addr, addr);

    if ((err = sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to)))) {
        transmitted++;
    }
    mem_free(iecho);
    return (err ? ERR_OK : ERR_VAL);
}

static void ping_recv(int s) {
    char buf[64];
    int fromlen, len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho = NULL;
    char ipa[16];
    struct timeval begin;
    struct timeval end;
    uint64_t micros_begin;
    uint64_t micros_end;
    float elapsed;

    // Register begin time
    gettimeofday(&begin, NULL);

    // Send
    while ((len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) {
        if (len >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
            // Register end time
            gettimeofday(&end, NULL);

            /// Get from IP address
            ip4_addr_t fromaddr;
            inet_addr_to_ip4addr(&fromaddr, &from.sin_addr);

            strcpy(ipa, inet_ntoa(fromaddr));

            // Get echo
            iphdr = (struct ip_hdr *)buf;
            iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));

            // Print ....
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
                received++;

                // Get elapsed time in milliseconds
                micros_begin = begin.tv_sec * 1000000;
                micros_begin += begin.tv_usec;

                micros_end = end.tv_sec * 1000000;
                micros_end += end.tv_usec;

                elapsed = (float)(micros_end - micros_begin) / (float)1000.0;

                // Update statistics
                // Mean and variance are computed in an incremental way
                if (elapsed < min_time) {
                    min_time = elapsed;
                }

                if (elapsed > max_time) {
                    max_time = elapsed;
                }

                last_mean_time = mean_time;
                mean_time = (((received - 1) * mean_time) + elapsed) / received;

                if (received > 1) {
                    var_time = var_time + ((elapsed - last_mean_time) * (elapsed - mean_time));
                }

                // Print ...
                log_d("%d bytes from %s: icmp_seq=%d time=%.3f ms\r\n", len, ipa,
                      ntohs(iecho->seqno), elapsed
                );

                return;
            }
            else {
                // TODO
            }
        }
    }

    if (len < 0) {
        log_d("Request timeout for icmp_seq %d\r\n", ping_seq_num);
    }
}

static void ping6_recv(int s) {
    char buf[64];
    int fromlen, len;
    struct sockaddr_in6 from;
    struct ip6_hdr *iphdr;
    struct icmp6_echo_hdr *iecho = NULL;
    struct timeval begin;
    struct timeval end;
    uint64_t micros_begin;
    uint64_t micros_end;
    float elapsed;

    // Register begin time
    gettimeofday(&begin, NULL);

    // Send
    while ((len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) {

        if (len >= (int)(sizeof(struct ip6_hdr) + sizeof(struct icmp6_echo_hdr))) {
            Serial.printf("Received %d bytes from %s\r\n", len, inet6_ntoa(from.sin6_addr));
            // Register end time
            gettimeofday(&end, NULL);

            /// Get from IP address
            ip6_addr_t fromaddr;
            inet6_addr_to_ip6addr(&fromaddr, &from.sin6_addr);

            // Get echo
            iphdr = (struct ip6_hdr *)buf;
            iecho = (struct icmp6_echo_hdr *)(buf + 40);


            // Print ....
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
                Serial.printf("Received ping reply from %s\r\n", inet6_ntoa(from.sin6_addr));
                received++;

                // Get elapsed time in milliseconds
                micros_begin = begin.tv_sec * 1000000;
                micros_begin += begin.tv_usec;

                micros_end = end.tv_sec * 1000000;
                micros_end += end.tv_usec;

                elapsed = (float)(micros_end - micros_begin) / (float)1000.0;

                // Update statistics
                // Mean and variance are computed in an incremental way
                if (elapsed < min_time) {
                    min_time = elapsed;
                }

                if (elapsed > max_time) {
                    max_time = elapsed;
                }

                last_mean_time = mean_time;
                mean_time = (((received - 1) * mean_time) + elapsed) / received;

                if (received > 1) {
                    var_time = var_time + ((elapsed - last_mean_time) * (elapsed - mean_time));
                }

                return;
            }
            else {
                // TODO
            }
        }
    }

    if (len < 0) {
        log_d("Request timeout for icmp_seq %d\r\n", ping_seq_num);
    }
}





bool internal_ping_start(IPAddress adr, int count = 0, int interval = 0, int size = 0, int timeout = 0, struct ping_option* ping_o = NULL) {
//	driver_error_t *error;
    struct sockaddr_in address;
    ip4_addr_t ping_target;
    int s;
    // Get default values if argument are not provided
    if (count == 0) {
        count = PING_DEFAULT_COUNT;
    }

    if (interval == 0) {
        interval = PING_DEFAULT_INTERVAL;
    }

    if (size == 0) {
        size = PING_DEFAULT_SIZE;
    }

    if (timeout == 0) {
        timeout = PING_DEFAULT_TIMEOUT;
    }

    // Create socket
    if ((s = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
        // TODO: error
        return false;
    }

    address.sin_addr.s_addr = adr;
    ping_target.addr = address.sin_addr.s_addr;

    // Setup socket
    struct timeval tout;

    // Timeout
    tout.tv_sec = timeout;
    tout.tv_usec = 0;

    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tout, sizeof(tout)) < 0) {
        closesocket(s);
        // TODO: error
        return false;
    }

    stopped = 0;
    transmitted = 0;
    received = 0;
    min_time = 1.E+9;// FLT_MAX;
    max_time = 0.0;
    mean_time = 0.0;
    var_time = 0.0;

    // Register signal for stop ping
    //signal(SIGINT, stop_action);

    // Begin ping ...
    char ipa[16];

    strcpy(ipa, inet_ntoa(ping_target));
    log_i("PING %s: %d data bytes\r\n",  ipa, size);

    ping_seq_num = 0;
    
    unsigned long ping_started_time = millis();
    while ((ping_seq_num < count) && (!stopped)) {
        if (ping_send(s, &ping_target, size) == ERR_OK) {
            ping_recv(s);
        }
        if(ping_seq_num < count){
            delay( interval*1000L);
        }
    }

    closesocket(s);

    log_i("%d packets transmitted, %d packets received, %.1f%% packet loss\r\n",
          transmitted,
          received,
          ((((float)transmitted - (float)received) / (float)transmitted) * 100.0)
    );
    
    
    if (ping_o) {
        ping_resp pingresp;
        log_i("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\r\n", min_time, mean_time, max_time, sqrt(var_time / received));
        pingresp.total_count = count; //Number of pings
        pingresp.resp_time = mean_time; //Average time for the pings
        pingresp.seqno = 0; //not relevant
        pingresp.timeout_count = transmitted - received; //number of pings which failed
        pingresp.bytes = size; //number of bytes received for 1 ping
        pingresp.total_bytes = size * count; //number of bytes for all pings
        pingresp.total_time = (millis() - ping_started_time) / 1000.0; //Time consumed for all pings; it takes into account also timeout pings
        pingresp.ping_err = transmitted - received; //number of pings failed
        // Call the callback function
        ping_o->recv_function(ping_o, &pingresp);
    }
    
    // Return true if at least one ping had a successfull "pong" 
    return (received > 0);
}

bool internal_ping6_start(IPv6Address adr, int count = 0, int interval = 0, int size = 0, int timeout = 0, struct ping6_option* ping_o = NULL) {
    //	driver_error_t *error;
    struct sockaddr_in6 address;
    ip6_addr_t ping_target;
    int s;
    // Get default values if argument are not provided
    if (count == 0) {
        count = PING_DEFAULT_COUNT;
    }

    if (interval == 0) {
        interval = PING_DEFAULT_INTERVAL;
    }

    if (size == 0) {
        size = PING_DEFAULT_SIZE;
    }

    if (timeout == 0) {
        timeout = PING_DEFAULT_TIMEOUT;
    }

    // Create socket
    if ((s = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0) {
        // TODO: error
        return false;
    }

    address.sin6_scope_id = if_nametoindex("wlan0");
    memcpy(address.sin6_addr.s6_addr, (const uint8_t*)adr, 16);
    memcpy(ping_target.addr, address.sin6_addr.s6_addr, 16);

    // Setup socket
    struct timeval tout;

    // Timeout
    tout.tv_sec = timeout;
    tout.tv_usec = 0;

    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tout, sizeof(tout)) < 0) {
        closesocket(s);
        // TODO: error
        return false;
    }

    stopped = 0;
    transmitted = 0;
    received = 0;
    min_time = 1.E+9;// FLT_MAX;
    max_time = 0.0;
    mean_time = 0.0;
    var_time = 0.0;

    ping_seq_num = 0;
    
    unsigned long ping_started_time = millis();
    while ((ping_seq_num < count) && (!stopped)) {
        if (ping6_send(s, &ping_target, size) == ERR_OK) {
            ping6_recv(s);
        }
        if(ping_seq_num < count){
            delay( interval*1000L);
        }
    }

    closesocket(s);
    
    
    if (ping_o) {
        ping_resp pingresp;
        log_i("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\r\n", min_time, mean_time, max_time, sqrt(var_time / received));
        pingresp.total_count = count; //Number of pings
        pingresp.resp_time = mean_time; //Average time for the pings
        pingresp.seqno = 0; //not relevant
        pingresp.timeout_count = transmitted - received; //number of pings which failed
        pingresp.bytes = size; //number of bytes received for 1 ping
        pingresp.total_bytes = size * count; //number of bytes for all pings
        pingresp.total_time = (millis() - ping_started_time) / 1000.0; //Time consumed for all pings; it takes into account also timeout pings
        pingresp.ping_err = transmitted - received; //number of pings failed
        // Call the callback function
        ping_o->recv_function(ping_o, &pingresp);
    }
    
    // Return true if at least one ping had a successfull "pong" 
    return (received > 0);
}


bool ping_start(struct ping_option* ping_o) {
    return internal_ping_start(ping_o->ip, ping_o->count, 0, 0, 0, ping_o);

}


bool ping6_start(struct ping6_option* ping_o) {
    return internal_ping6_start(ping_o->ip, ping_o->count, 0, 0, 0, ping_o);

}

