#ifndef slog_types_header_h
#define slog_types_header_h

#include <string>
#include <cstdint>

namespace SimpleLogTypes {

    enum class SqlApi {
        not_set,
        odbc
    };

    enum class LoggingTarget : uint8_t {
        not_set     = 1 << 0,
        eventlog    = 1 << 1,
        syslog      = 1 << 2,
        file        = 1 << 3,
        sql         = 1 << 4,
        console     = 1 << 5
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