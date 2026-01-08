#ifndef slog_types_header_h
#define slog_types_header_h

#include <string>
#include <cstdint>
#include <unordered_map>

namespace SimpleLogTypes {

    enum class OperatingSystem {
        not_set,
        windows,
        macos,
        linux,
        unix
    };

    enum class SqlApi {
        not_set,
        odbc
    };

    enum class LoggingTarget : uint8_t {
        not_set     = 1 << 0,
        os          = 1 << 1,
        file        = 1 << 2,
        sql         = 1 << 3,
        console     = 1 << 4
    };
    constexpr LoggingTarget operator|(LoggingTarget a, LoggingTarget b) { return static_cast<LoggingTarget>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }
    constexpr LoggingTarget operator&(LoggingTarget a, LoggingTarget b) { return static_cast<LoggingTarget>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); }
    constexpr LoggingTarget operator~(LoggingTarget a) { return static_cast<LoggingTarget>(~static_cast<uint8_t>(a)); }

    enum class Severity {
        emergency,
        alert,
        critical,
        error,
        warning,
        notice,
        informational,
        debug
    };

    struct Event {
        Severity severity;
        std::string source;
        std::string host;
        std::string message;
    };
}

#endif