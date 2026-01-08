#ifndef file_writer_header_h
#define file_writer_header_h

#include <cstdint>
#include <string>
#include <unordered_map>
#include <fstream>
#include <filesystem>

namespace SimpleLog {
	class file_writer {
    private:
    
        // return code definitions
        static constexpr uint8_t SUCCESS                    = 0;
        static constexpr uint8_t PARAMETERS_NOT_DEFINED     = 1;
        static constexpr uint8_t INVALID_FILEPATH           = 2;
        static constexpr uint8_t FILE_ACCESS_ERROR          = 3;
        static constexpr uint8_t FILE_ALREADY_CLOSED        = 4;
        static constexpr uint8_t FILE_NOT_OPEN              = 5;
        static constexpr uint8_t FILE_PATH_NOT_DEFINED      = 6;
        const std::unordered_map<uint8_t, std::string> cm_return_codes {
            {SUCCESS,                   std::string("no error")},
            {PARAMETERS_NOT_DEFINED,    std::string("writer parameters are not defined")},
            {INVALID_FILEPATH,          std::string("filepath exists but is not a regular file")},
            {FILE_ACCESS_ERROR,         std::string("file could not be opened or created")},
            {FILE_ALREADY_CLOSED,       std::string("file was already closed")},
            {FILE_NOT_OPEN,             std::string("file is closed")},
            {FILE_PATH_NOT_DEFINED,     std::string("file path must be provided")}
        };
        
        // properties
        bool m_defined;
        std::filesystem::path m_filepath;
        std::ofstream m_file;
        uint16_t m_buffersize;
        uint16_t m_currentrow;
        std::string m_delimitor;
        std::string m_file_extension;
        std::string m_header;

	public:
        file_writer() { m_defined = false; }
        ~file_writer() { stop(); }

        void define(const std::string &filepath, const uint16_t &buffersize, const std::string &delimitor, const std::string &file_extension);
        const uint8_t start();
        const uint8_t log(const std::string &severity, const std::string &timestamp, const std::string &source, const std::string &message);
        void stop();
        const std::string return_code_def(const uint8_t &return_code);

	};
}

#endif