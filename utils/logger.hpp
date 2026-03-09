#pragma once
#include <iostream>

#define BOLD "\033[1m"

namespace color
{
    inline constexpr const char *red = "\033[31m";
    inline constexpr const char *yellow = "\033[33m";
    inline constexpr const char *green = "\033[32m";
    inline constexpr const char *reset = "\033[0m";
    inline constexpr const char *blue = "\033[34m";
    inline constexpr const char *light_blue = "\033[96m";
    inline constexpr const char *purple = "\033[35m";

    // Special colours true color
    inline constexpr const char *orange_fg = "\033[38;2;255;165;0m";
    inline constexpr const char *pastel_blue_fg = "\033[38;2;173;216;230m";
}

#define LOG_ERROR(msg) \
    std::cerr << BOLD << color::red << "[ERROR] " << msg << color::reset << std::endl

#define LOG_WARNING(msg) \
    std::cout << BOLD << color::yellow << "[WARNING] " << msg << color::reset << std::endl

#define LOG_SUCCESS(msg) \
    std::cout << BOLD << color::green << "[SUCCESS] " << msg << color::reset << std::endl

#define LOG_CLIENT_SET(msg) \
    std::cout << BOLD << color::blue << "[CLIENT_SET] " << msg << color::reset << std::endl

#define LOG_REPLICATION(msg) \
    std::cout << BOLD << color::pastel_blue_fg << "[REPLICATION] " << msg << color::reset << std::endl

#define LOG_STATE(msg) \
    std::cout << BOLD << color::orange_fg << "[STATE] " << msg << color::reset << std::endl

#define LOG_BROADCAST(msg) \
    std::cout << BOLD << color::purple << "[BROADCAST] " << msg << color::reset << std::endl

#define LOG_NODE(msg) \
    std::cout << BOLD << "[NODE] " << msg << color::reset << std::endl

#define LOG_CUSTOM(tag, msg) \
    std::cout << BOLD << "[" << tag << "] " << msg << color::reset << std::endl
