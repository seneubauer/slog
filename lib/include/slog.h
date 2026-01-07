#ifndef slog_header_h
#define slog_header_h

#include <slog_types.h>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <queue>
#include <unordered_map>

namespace SimpleLog {
	class slog {
	public:
		slog(const SimpleLog::LogTarget &targets, const bool &inprogress) : m_targets(targets), m_inprogress(inprogress) {}
		~slog() { stop(); }

		void set_logging_targets(const SimpleLog::LogTarget &targets);
		void set_sql_connectivity(const std::string &server, const std::string &database, const std::string &driver, const SimpleLog::SqlApi &sql_api);
		void log(Event event);
		void start();
		void stop();

	private:
		void process();
		void emit(const Event &event);

		// operative members
		SimpleLog::OperatingSystem m_os;
		SimpleLog::LogTarget m_targets;

		// connectivity members
		std::string m_sql_server;
		std::string m_sql_database;
		std::string m_sql_driver;
		SimpleLog::SqlApi m_sql_api;

		// control members
		std::queue<Event> m_events;
		std::mutex m_mutex;
		std::condition_variable m_cvar;
		std::thread m_thread;
		std::atomic<bool> m_inprogress;

		std::unordered_map<Severity, SimpleLog::SeverityDef> m_severity_defs = {
			{SimpleLog::Severity::emergency, 		SimpleLog::SeverityDef {0, std::string("emerg")}},
			{SimpleLog::Severity::alert, 			SimpleLog::SeverityDef {1, std::string("alert")}},
			{SimpleLog::Severity::critical, 		SimpleLog::SeverityDef {2, std::string("crit")}},
			{SimpleLog::Severity::error, 			SimpleLog::SeverityDef {3, std::string("err")}},
			{SimpleLog::Severity::warning, 			SimpleLog::SeverityDef {4, std::string("warn")}},
			{SimpleLog::Severity::notice, 			SimpleLog::SeverityDef {5, std::string("notice")}},
			{SimpleLog::Severity::informational,	SimpleLog::SeverityDef {6, std::string("info")}},
			{SimpleLog::Severity::debug, 			SimpleLog::SeverityDef {7, std::string("debug")}}
		};
	};
}

#endif