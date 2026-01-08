#ifndef windows_writer_header_h
#define windows_writer_header_h

#include <cstdint>
#include <unordered_map>
#include <string>

namespace SimpleLog {
    class windows_writer {
    private:
    
        // return code definitions
        static constexpr uint8_t SUCCESS            = 0;
        
        const std::unordered_map<uint8_t, std::string> cm_return_codes {
            {SUCCESS                    std::string("no error")}
        };

        // properties
        bool m_defined;
        
    public:
        windows_writer() { m_defined = false; }
        ~windows_writer() { stop(); }

        void define();
        const uint8_t start();
        const uint8_t log();
        void stop();
        const std::string return_code_def(const uint8_t &return_code);

    };
}

#endif