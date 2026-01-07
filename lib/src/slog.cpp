#include <slog.h>
#include <utility>
#include <iostream>

void SimpleLog::slog::set_sql_connectivity(const std::string &server, const std::string &database, const std::string &driver, const SimpleLog::SqlApi &sql_api) {
	m_sql_server = server;
	m_sql_database = database;
	m_sql_driver = driver;
	m_sql_api = sql_api;
}

void SimpleLog::slog::log(Event event) {
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_events.push(std::move(event));
	}
	m_cvar.notify_one();
}

void SimpleLog::slog::start() {
	m_thread = std::thread(&SimpleLog::slog::process, this);
}

void SimpleLog::slog::stop() {
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_inprogress = false;
	}
	m_cvar.notify_one();
	if (m_thread.joinable())
		m_thread.join();
}

void SimpleLog::slog::process() {
	while (true) {
		Event event;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cvar.wait(lock, [&]{ return !m_events.empty() || !m_inprogress; });
			
			if (!m_inprogress && m_events.empty())
				return;
			
			event = m_events.front();
			m_events.pop();
		}
		
		emit(event);
	}
}

void SimpleLog::slog::emit(const Event &event) {
	
	std::this_thread::sleep_for(std::chrono::seconds(3));
	
	if (has_target(m_targets, SimpleLog::LogTarget::syslog)) {
		std::cout << std::string("sending to syslog: ") << event.message << std::endl;
	}
	
	if (has_target(m_targets, SimpleLog::LogTarget::csv)) {
		std::cout << std::string("sending to csv: ") << event.message << std::endl;
	}
	
	if (has_target(m_targets, SimpleLog::LogTarget::sql)) {
		std::cout << std::string("sending to sql database: ") << event.message << std::endl;
	}
}