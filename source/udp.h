#pragma once

#include <cstdint>
#include <string_view>

void udp_init(std::string_view ipString, std::uint16_t ipport);
void udp_deinit();
void udp_print(const char *str);
