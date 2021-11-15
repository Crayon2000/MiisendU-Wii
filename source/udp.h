#pragma once

#include <string_view>

void udp_init(std::string_view ip, unsigned short ipport);
void udp_deinit(void);
void udp_print(const char *str);
