#ifndef ICMP_H
#define ICMP_H

#include <Arduino.h>
#include <IPAddress.h>
#include <IPv6Address.h>
#include <ping/ping_sock.h>
#include <lwip/inet.h>
#include <lwip/netdb.h>
#include <net/if.h>
#include <lwip/ip4.h>
#include <lwip/ip6.h>
#include <esp_netif_net_stack.h>

class ICMP {
  public:

    static bool ping(IPAddress dest, size_t count = 5) {

        ip_addr_t target_addr;
        target_addr.type = IPADDR_TYPE_V4;
        target_addr.u_addr.ip4.addr = static_cast<const uint32_t>(dest);

        initialize_ping(target_addr, count);

        return true;
    }

    static bool ping(IPv6Address dest, size_t count = 5) {
        
        ip_addr_t target_addr;
        target_addr.type = IPADDR_TYPE_V6;
        // target_addr.u_addr.ip6.zone = get_netif_index();

        memcpy(target_addr.u_addr.ip6.addr, static_cast<const uint32_t*>(dest), sizeof(target_addr.u_addr.ip6.addr));

        initialize_ping(target_addr, count);

        return true;
    }

  private:
    static void initialize_ping(ip_addr_t target_addr, size_t count) {

        esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
        ping_config.target_addr = target_addr;
        ping_config.count = count;
        // ping_config.interface = get_netif_index();

        esp_ping_callbacks_t cbs;
        cbs.on_ping_success = on_ping_success;
        cbs.on_ping_timeout = on_ping_timeout;
        cbs.on_ping_end = on_ping_end;
        cbs.cb_args = NULL;

        esp_ping_handle_t ping;
        esp_ping_new_session(&ping_config, &cbs, &ping);
        esp_ping_start(ping);
    }

    static void on_ping_success(esp_ping_handle_t hdl, void *args) {

        uint8_t ttl;
        uint16_t seqno;
        uint32_t elapsed_time, recv_len;
        ip_addr_t target_addr;
        esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
        esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
        esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
        esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
        esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
        
        // printf("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\n", recv_len, ipaddr_ntoa((ip_addr_t*)&target_addr), seqno, ttl, elapsed_time);

    }

    static void on_ping_timeout(esp_ping_handle_t hdl, void *args) {
        uint16_t seqno;
        ip_addr_t target_addr;
        esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
        esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
        
        // printf("From %s icmp_seq=%d timeout\n", ipaddr_ntoa((ip_addr_t*)&target_addr), seqno);
        
    }

    static void on_ping_end(esp_ping_handle_t hdl, void *args) {
        uint32_t transmitted;
        uint32_t received;
        uint32_t total_time_ms;

        esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
        esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
        esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
        printf("%d packets transmitted, %d received, time %dms\n", transmitted, received, total_time_ms);

        esp_ping_delete_session(hdl);
    }

    static int get_netif_index()
    {
        struct netif *netif = netif_list;
        while (netif) {
            if (netif->name[0] == 's' && netif->name[1] == 't') { // "st" usually stands for STA
                return netif_get_index(netif); // this gives the scope ID
            }
            netif = netif->next;
        }
        // Serial.println("No suitable network interface found for ping.");
        return 0;
    }


};


#endif // ICMP_H