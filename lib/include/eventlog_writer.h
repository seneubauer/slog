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
        static constexpr uint8_t COULD_NOT_SET_KEYPATH      = 2;
        static constexpr uint8_t COULD_NOT_SET_KEY          = 3;

        const std::unordered_map<uint8_t, std::string> cm_return_codes {
            {SUCCESS,                   std::string("no error")},
            {PARAMETERS_NOT_DEFINED,    std::string("event log writer parameters are not defined")},
            {COULD_NOT_SET_KEYPATH,     std::string("could not set the application's event registry path")},
            {COULD_NOT_SET_KEY,         std::string("could not set the application's event registry key")}
        };

        // properties
        bool m_defined;
        std::wstring m_application;

        // private methods
        const std::string get_last_error();

    public:
        eventlog_writer() { m_defined = false; }
        ~eventlog_writer() { stop(); }

        void define(const std::wstring &application);
        uint8_t start();
        uint8_t log();
        void stop();
        const std::string return_code_def(const uint8_t &return_code);

    };
}

#endif