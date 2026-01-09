#ifndef slog_header_h
#define slog_header_h

#include <slog_types.h>
#include <slog_utility.h>
#include <file_writer.h>
#include <eventlog_writer.h>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <queue>
#include <cstdint>
#include <string>

namespace SimpleLog {
    class slog {
    private:

        // operative members
        SimpleLogTypes::LoggingTarget m_targets;

        // concurrency members
        std::queue<SimpleLogTypes::Event> m_events;
        std::mutex m_mutex;
        std::condition_variable m_cvar;
        std::thread m_thread;
        std::atomic<bool> m_inprogress;

        // writer members
        std::shared_ptr<SimpleLog::file_writer> mp_filewriter;
        std::shared_ptr<SimpleLog::eventlog_writer> mp_elwriter;

        // concurrency functions
        void process(std::shared_ptr<SimpleLog::file_writer> p_filewriter, std::shared_ptr<SimpleLog::eventlog_writer> p_elwriter);
        void emit(const SimpleLogTypes::Event &event, std::shared_ptr<SimpleLog::file_writer> p_filewriter, std::shared_ptr<SimpleLog::eventlog_writer> p_elwriter);

    public:
        slog(const SimpleLogTypes::LoggingTarget &targets) : m_targets(targets) {
            m_inprogress = true;

            mp_filewriter = nullptr;
            if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::file))
                mp_filewriter = std::make_shared<SimpleLog::file_writer>();

            mp_elwriter = nullptr;
            if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::eventlog))
                mp_elwriter = std::make_shared<SimpleLog::eventlog_writer>();

        }
        ~slog() { stop(); }

        bool set_parameters_eventlog(const std::wstring &application, std::string &error);
        bool set_parameters_file(const std::string &filepath, const uint16_t &buffersize, const std::string &delimitor, const std::string &file_extension, std::string &error);
        bool set_parameters_sql(std::string &error);
        bool start(std::string &error);
        void log(SimpleLogTypes::Event event);
        void stop();

    };
}

#endif