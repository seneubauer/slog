#ifndef slog_types_header_h
#define slog_types_header_h

#include <string>
#include <chrono>

namespace SimpleLog {

	enum class OperatingSystem {
		windows,
		macos,
		linux,
		unix
	};

	enum class SqlApi {
		not_set,
		odbc
	};

	enum class LogTarget : unsigned int {
		not_set = 0,
		syslog = 1 << 0,
		csv = 1 << 1,
		sql = 1 << 2
	};
	inline LogTarget operator|(LogTarget a, LogTarget b) { return static_cast<LogTarget>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b)); }

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
		slog::Severity severity;
		std::string facility;
		std::chrono::time_point timestamp;
		std::string host;
		int pid;
		std::string message;
	};
}

#endif