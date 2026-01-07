#include <slog.h>
#include <utility>
#include <sstream>
#include <iostream>
#include <format>

void SimpleLog::slog::set_file_parameters(const std::string &filepath, const uint16_t &buffersize) {
    m_logfile_path = filepath;
    m_logfile_buffersize = buffersize;
}

void SimpleLog::slog::set_sql_parameters(const std::string &server, const std::string &database, const std::string &driver, const SimpleLog::SqlApi &sql_api) {
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

bool SimpleLog::slog::start(std::string &error) {

    if (has_target(m_targets, SimpleLog::LogTarget::file)) {
        m_lfwriter.set_parameters(m_logfile_path, m_logfile_buffersize);
        uint8_t rx = m_lfwriter.begin();
        if (rx) {
            error = m_lfwriter.interpret_return_code(rx);
            return false;
        }
    }

    m_thread = std::thread(&SimpleLog::slog::process, this, &m_lfwriter);
    return true;
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

void SimpleLog::slog::process(logfile_writer *p_lfwriter) {
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
        emit(event, p_lfwriter);
    }
}

void SimpleLog::slog::emit(const Event &event, logfile_writer *p_lfwriter) {
    
    std::lock_guard<std::mutex> lock(m_mutex);

    if (has_target(m_targets, SimpleLog::LogTarget::os)) {
        // log to operating system
    }

    if (has_target(m_targets, SimpleLog::LogTarget::file)) {
        std::string severity = m_severity_defs[event.severity].alias;
        std::string timestamp = timestamp_str();
        std::string source = event.source;
        std::string message = event.message;
        p_lfwriter->add_entry(severity, timestamp, source, message);
    }

    if (has_target(m_targets, SimpleLog::LogTarget::sql)) {
        // log to sql database
    }
    
    if (has_target(m_targets, SimpleLog::LogTarget::console)) {
        std::cout << "logging to console" << std::endl;
    }
}

std::string SimpleLog::slog::timestamp_str() {
    std::string fmt = "{:%Y-%m-%d %H:%M:%S}";
    auto now = std::chrono::system_clock::now();
    return std::vformat(fmt, std::make_format_args(now));
}