#include <slog.h>

void SimpleLog::slog::set_sql_connectivity(const std::string &server, const std::string &database, const std::string &driver, const SimpleLog::SqlApi &sql_api) {
	m_sql_server = server;
	m_sql_database = database;
	m_sql_driver = driver;
	m_sql_api = sql_api;
}

void SimpleLog::slog::log(Event event) {
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(std::move(event));
	}
	m_cvar.notify_one();
}

void SimpleLog::slog::start() {
	m_thread(&process, this);
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
			m_cvar.wait(lock, [&]{ return !m_queue.empty() || !m_inprogress; });
			
			if (!m_inprogress && m_queue.empty())
				return;
			
			event = m_queue.front();
			m_queue.pop();
		}
		
		emit(event);
	}
}

void SimpleLog::slog::emit(const Event &event) {
	
	if (m_targets & SimpleLog::LogTarget::syslog) {
		// send to syslog
	}
	
	if (m_targets & SimpleLog::LogTarget::csv) {
		// send to csv
	}
	
	if (m_targets & SimpleLog::LogTarget::sql) {
		// send to sql database
	}
}