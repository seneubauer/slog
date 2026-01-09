#ifndef slog_utility_header_h
#define slog_utility_header_h

#include <slog_types.h>
#include <cstdint>
#include <string>
#include <chrono>
#include <format>
#include <unordered_map>

namespace SimpleLogUtility {

    static std::unordered_map<SimpleLogTypes::Severity, std::string> severity_definitions {
        {SimpleLogTypes::Severity::emergency,       std::string("emerg")},
        {SimpleLogTypes::Severity::alert,           std::string("alert")},
        {SimpleLogTypes::Severity::critical,        std::string("crit")},
        {SimpleLogTypes::Severity::error,           std::string("err")},
        {SimpleLogTypes::Severity::warning,         std::string("warn")},
        {SimpleLogTypes::Severity::notice,          std::string("notice")},
        {SimpleLogTypes::Severity::informational,   std::string("info")},
        {SimpleLogTypes::Severity::debug,           std::string("debug")}
    };
    inline std::string severity_alias(const SimpleLogTypes::Severity &severity) {
        auto iterator = severity_definitions.find(severity);
        if (iterator != severity_definitions.end())
            return iterator->second;

        return std::string();
    }

    template <typename T>
    constexpr bool any_enum(const T &enum_value) { return static_cast<uint8_t>(enum_value) != 0; }

    template <typename T>
    constexpr bool has_enum(const T &enum_composition, const T &enum_value) { return any_enum(enum_composition & enum_value); }

    inline std::chrono::time_point<std::chrono::system_clock> get_timestamp() {
        return std::chrono::system_clock::now();
    }

    inline std::string get_timestamp_str() {
        return std::format("{:%Y-%m-%d %H:%M:%S}", get_timestamp());
    }

}

#endif