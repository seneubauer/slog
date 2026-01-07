#ifndef slog_types_header_h
#define slog_types_header_h

#include <string>
#include <cstdint>

namespace SimpleLog {

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

    enum class LogTarget : uint8_t {
        not_set     = 1 << 0,
        os          = 1 << 1,
        file        = 1 << 2,
        sql         = 1 << 3,
        console     = 1 << 4
    };
    constexpr LogTarget operator|(LogTarget a, LogTarget b) { return static_cast<LogTarget>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }
    constexpr LogTarget operator&(LogTarget a, LogTarget b) { return static_cast<LogTarget>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); }
    constexpr LogTarget operator~(LogTarget a) { return static_cast<LogTarget>(~static_cast<uint8_t>(a)); }
    constexpr bool lt_any(const LogTarget &targets) { return static_cast<uint8_t>(targets) != 0; }
    constexpr bool has_target(const LogTarget &targets, const LogTarget &check_value) { return lt_any(targets & check_value); }

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

    struct SeverityDef {
        int value;
        std::string alias;
    };

    struct Event {
        Severity severity;
        std::string source;
        std::string host;
        uint32_t pid;
        std::string message;
    };
}

#endif