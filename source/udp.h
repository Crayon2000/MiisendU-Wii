#pragma once

#include <string_view>
#include <cstdint>

void udp_init(std::string_view ip, uint16_t ipport);
void udp_deinit(void);
void udp_print(const char *str);
