#include <windows_writer.h>
#include <cstdint>
#include <string>

void SimpleLog::windows_writer::define() {
    
}

const uint8_t SimpleLog::windows_writer::start() {
    
}

const uint8_t SimpleLog::windows_writer::log() {
    
}

void SimpleLog::windows_writer::stop() {
    
}

const std::string SimpleLog::windows_writer::return_code_def(const uint8_t &return_code) {
    
    auto iterator = cm_return_codes.find(return_code);
    if (iterator != cm_return_codes.end())
        return iterator->second;
    
    return std::string();
}
