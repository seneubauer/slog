#include <slog.h>
#include <slog_utility.h>
#include <iostream>
#include <memory>

void SimpleLog::slog::process(std::shared_ptr<SimpleLog::file_writer> p_filewriter) {
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
        emit(event, p_filewriter);
    }
}

void SimpleLog::slog::emit(const SimpleLogTypes::Event &event, std::shared_ptr<SimpleLog::file_writer> p_filewriter) {
    
    std::lock_guard<std::mutex> lock(m_mutex);

    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::os)) {
        // log to operating system
    }

    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::file)) {
        std::string severity = SimpleLogUtility::severity_alias(event.severity);
        std::string timestamp = SimpleLogUtility::get_timestamp_str();
        std::string source = event.source;
        std::string message = event.message;
        p_filewriter->log(severity, timestamp, source, message);
    }

    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::sql)) {
        // log to sql database
    }

    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::console)) {
        std::string severity = SimpleLogUtility::severity_alias(event.severity);
        std::string timestamp = SimpleLogUtility::get_timestamp_str();
        std::string source = event.source;
        std::string message = event.message;
        std::cout << std::left
                  << std::setw(10) << severity
                  << std::setw(30) << timestamp
                  << std::setw(20) << source
                  << std::setw(20) << message
                  << std::endl;
    }
}

const bool SimpleLog::slog::set_parameters_os_windows(std::string &error) {
    if (!mp_winwriter) {
        error = std::string("windows logger evaluates to nullptr");
        return false;
    }
    mp_winwriter->define();
    return true;
}

const bool SimpleLog::slog::set_parameters_os_nonwindows(std::string &error) {
    if (false) {
        error = std::string("nonwindows logger evaluates to nullptr");
        return false;
    }
    // nonwindows os logging not implemented yet
    return true;
}

const bool SimpleLog::slog::set_parameters_file(const std::string &filepath, const uint16_t &buffersize, const std::string &delimitor, const std::string &file_extension, std::string &error) {
    if (!mp_filewriter) {
        error = std::string("file logger evaluates to nullptr");
        return false;
    }
    mp_filewriter->define(filepath, buffersize, delimitor, file_extension);
    return true;
}

const bool SimpleLog::slog::set_parameters_sql(std::string &error) {
    if (false) {
        error = std::string("sql logger evaluates to nullptr");
        return false;
    }
    // sql logging not implemented yet
    return true;
}

const bool SimpleLog::slog::start(std::string &error) {

    if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::file)) {
        uint8_t rx = mp_filewriter->start();
        if (rx) {
            error = mp_filewriter->return_code_def(rx);
            return false;
        }
    }

    m_thread = std::thread(&SimpleLog::slog::process, this, mp_filewriter);
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
