#ifndef slog_header_h
#define slog_header_h

#include <slog_types.h>
#include <slog_utility.h>
#include <file_writer.h>

#include <memory>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <queue>
#include <unordered_map>

namespace SimpleLog {
    class slog {
    private:

        // operative members
        SimpleLogTypes::OperatingSystem m_os;
        SimpleLogTypes::LoggingTarget m_targets;

        // concurrency members
        std::queue<SimpleLogTypes::Event> m_events;
        std::mutex m_mutex;
        std::condition_variable m_cvar;
        std::thread m_thread;
        std::atomic<bool> m_inprogress;

        // writer members
        std::shared_ptr<SimpleLog::file_writer> mp_filewriter;

        // concurrency functions
        void process(std::shared_ptr<SimpleLog::file_writer> p_filewriter);
        void emit(const SimpleLogTypes::Event &event, std::shared_ptr<SimpleLog::file_writer> p_filewriter);

    public:
        slog(const SimpleLogTypes::LoggingTarget &targets, const SimpleLogTypes::OperatingSystem &os) : m_targets(targets), m_os(os) {
            m_inprogress = true;

            mp_filewriter = nullptr;
            if (SimpleLogUtility::has_enum<SimpleLogTypes::LoggingTarget>(m_targets, SimpleLogTypes::LoggingTarget::file))
                mp_filewriter = std::make_shared<SimpleLog::file_writer>();
            
        }
        ~slog() { stop(); }

        const bool set_parameters_os_windows(std::string &error);
        const bool set_parameters_os_nonwindows(std::string &error);
        const bool set_parameters_file(const std::string &filepath, const uint16_t &buffersize, const std::string &delimitor, const std::string &file_extension, std::string &error);
        const bool set_parameters_sql(std::string &error);
        const bool start(std::string &error);
        void log(SimpleLogTypes::Event event);
        void stop();

    };
}

#endif