#include <slog.h>
#include <slog_types.h>
#include <slog_utility.h>

#include <iostream>
#include <memory>
#include <utility>
#include <condition_variable>
#include <mutex>
#include <string>
#include <cstdint>
#include <thread>

void SimpleLog::slog::process(std::shared_ptr<SimpleLog::file_writer> p_filewriter, std::shared_ptr<SimpleLog::eventlog_writer> p_elwriter) {
    while (true) {
        SimpleLogTypes::Event event;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cvar.wait(lock, [&]{ return !m_events.empty() || !m_inprogress; });

            if (!m_inprogress && m_events.empty())
                return;

            event = m_events.front();
            m_events.pop();
        }
        emit(event, p_filewriter, p_elwriter);
    }
}

void SimpleLog::slog::emit(const SimpleLogTypes::Event &event, std::shared_ptr<SimpleLog::file_writer> p_filewriter, std::shared_ptr<SimpleLog::eventlog_writer> p_elwriter) {

    std::lock_guard<std::mutex> lock(m_mutex);

    // log to eventlog
    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::eventlog)) {

        uint16_t event_type_id = 0;
        switch (event.severity) {
        case SimpleLogTypes::Severity::emergency:
            event_type_id = 0x0001;     // maps to EVENTLOG_ERROR_TYPE
            break;
        case SimpleLogTypes::Severity::alert:
            event_type_id = 0x0002;     // maps to EVENTLOG_WARNING_TYPE
            break;
        case SimpleLogTypes::Severity::critical:
            event_type_id = 0x0001;     // maps to EVENTLOG_ERROR_TYPE
            break;
        case SimpleLogTypes::Severity::error:
            event_type_id = 0x0001;     // maps to EVENTLOG_ERROR_TYPE
            break;
        case SimpleLogTypes::Severity::warning:
            event_type_id = 0x0002;     // maps to EVENTLOG_WARNING_TYPE
            break;
        case SimpleLogTypes::Severity::notice:
            event_type_id = 0x0004;     // maps to EVENTLOG_INFORMATION_TYPE
            break;
        case SimpleLogTypes::Severity::informational:
            event_type_id = 0x0004;     // maps to EVENTLOG_INFORMATION_TYPE
            break;
        case SimpleLogTypes::Severity::debug:
            event_type_id = 0x0004;     // maps to EVENTLOG_INFORMATION_TYPE
            break;
        }

        uint16_t category_id = 0;
        uint32_t event_id = 0;

        p_elwriter->log(event_type_id, category_id, event_id, event.message);
    }

    // log to file
    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::file)) {
        std::string severity = SimpleLogUtility::severity_alias(event.severity);
        std::string timestamp = SimpleLogUtility::get_timestamp_str();
        std::string source = event.source;
        std::string message = static_cast<std::string>(event.message);
        p_filewriter->log(severity, timestamp, source, message);
    }

    // log to sql database
    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::sql)) {}

    // log to console
    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::console)) {
        std::string severity = SimpleLogUtility::severity_alias(event.severity);
        std::string timestamp = SimpleLogUtility::get_timestamp_str();
        std::string source = event.source;
        std::string message = static_cast<std::string>(event.message);
        std::cout << std::left
                  << std::setw(10) << severity
                  << std::setw(30) << timestamp
                  << std::setw(20) << source
                  << std::setw(20) << message
                  << std::endl;
    }
}

bool SimpleLog::slog::set_parameters_eventlog(const std::string &application, std::string &error) {
    if (!mp_elwriter) {
        error = std::string("event log handler evaluates to nullptr");
        return false;
    }
    mp_elwriter->define(application);
    return true;
}

bool SimpleLog::slog::set_parameters_file(const std::string &filepath, const uint16_t &buffersize, const std::string &delimitor, const std::string &file_extension, std::string &error) {
    if (!mp_filewriter) {
        error = std::string("file handler evaluates to nullptr");
        return false;
    }
    mp_filewriter->define(filepath, buffersize, delimitor, file_extension);
    return true;
}

bool SimpleLog::slog::set_parameters_sql(std::string &error) {
    if (false) {
        error = std::string("sql handler evaluates to nullptr");
        return false;
    }
    // sql logging not implemented yet
    return true;
}

bool SimpleLog::slog::start(std::string &error) {
    uint8_t rc;
    bool no_writers = true;

    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::file)) {
        rc = mp_filewriter->start();
        if (rc) {
            error = mp_filewriter->return_code_def(rc);
            return false;
        }
        no_writers = false;
    }

    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::eventlog)) {
        rc = mp_elwriter->start();
        if (rc) {
            error = mp_elwriter->return_code_def(rc);
            std::string lasterror = mp_elwriter->last_error();
            if (!lasterror.empty())
                error += " (" + mp_elwriter->last_error() + ")";
            return false;
        }
        no_writers = false;
    }

    if (no_writers) {
        error = std::string("no writers were selected");
        return false;
    }

    m_thread = std::thread(&SimpleLog::slog::process, this, mp_filewriter, mp_elwriter);
    return true;
}

void SimpleLog::slog::log(SimpleLogTypes::Event event) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_events.push(std::move(event));
    }
    m_cvar.notify_one();
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
