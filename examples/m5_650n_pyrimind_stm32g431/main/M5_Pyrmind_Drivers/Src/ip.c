#include "ip.h" 

static __IO uint32_t ip = IP_ADDR_DEFAULT;

void set_ip_high(uint16_t high){
    uint16_t ip_high = ip & 0x0000FFFF;
    ip_high = high << 16;
    ip = ip_high | (ip & 0xFFFF);
}

void set_ip_low(uint16_t low){
    uint16_t ip_low = ip & 0xFFFF;
    ip_low = low;
    ip = (ip & 0xFFFF0000) | ip_low;
}

uint32_t get_ip(void){
    return ip;
}