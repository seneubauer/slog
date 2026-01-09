#ifndef eventlog_writer_header_h
#define eventlog_writer_header_h

#include <cstdint>
#include <unordered_map>
#include <string>

namespace SimpleLog {
    class eventlog_writer {
    private:

        // return code definitions
        static constexpr uint8_t SUCCESS                    = 0;
        static constexpr uint8_t PARAMETERS_NOT_DEFINED     = 1;
        static constexpr uint8_t NOGET_EVENTLOG_HANLDE      = 2;
        static constexpr uint8_t NOGET_PROCESS_TOKEN        = 3;
        static constexpr uint8_t NOGET_USER_TOKEN           = 4;
        static constexpr uint8_t NOGET_USER_PSID            = 5;
        static constexpr uint8_t LOG_FAILURE                = 6;

        const std::unordered_map<uint8_t, std::string> cm_return_codes {
            {SUCCESS,                   std::string("no error")},
            {PARAMETERS_NOT_DEFINED,    std::string("event log writer parameters are not defined")},
            {NOGET_EVENTLOG_HANLDE,     std::string("could not get a handle to the application event log")},
            {NOGET_PROCESS_TOKEN,       std::string("could not get a handle to the current process token")},
            {NOGET_USER_TOKEN,          std::string("could not get a handle to the current user token")},
            {NOGET_USER_PSID,           std::string("could not get a handle to the current user security identifier")},
            {LOG_FAILURE,               std::string("could not log the requested event")}
        };

        // properties
        bool m_defined;
        std::string m_lasterror;
        std::string m_application;
        void* mp_eventloghandle;
        void* mp_user_sid;

        // private methods
        const std::string get_last_error();

    public:
        eventlog_writer() { m_defined = false; }
        ~eventlog_writer() { stop(); }

        void define(const std::string &application);
        uint8_t start();
        uint8_t log(const uint16_t &event_type_id, const uint16_t &category_id, const uint32_t &event_id, const std::string &message);
        void stop();
        const std::string return_code_def(const uint8_t &return_code);
        const std::string last_error() { return get_last_error(); }

    };
}

#endif