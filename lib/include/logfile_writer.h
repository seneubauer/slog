#ifndef logfile_writer_header_h
#define logfile_writer_header_h

#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>
#include <unordered_map>

namespace SimpleLog {
	class logfile_writer {
	public:
        logfile_writer() {}
        ~logfile_writer() { end(); }

        void set_parameters(const std::string &filepath, const uint16_t &buffersize) {
            m_filepath = filepath;
            m_buffersize = buffersize;
        }
        uint8_t begin();
        void end();
        uint8_t add_entry(const std::string &severity, const std::string &timestamp, const std::string &source, const std::string &message);
        std::string interpret_return_code(const uint8_t &rc) {
            return cm_return_codes[rc];
        }

	private:
        std::filesystem::path m_filepath;
        std::ofstream m_logfile;
        uint16_t m_buffersize;
        uint16_t m_currentrow;

        std::string cm_delimitor = "~|~";
        std::string cm_file_extension = ".slog";
        std::string cm_header = std::string("severity")   + std::string(cm_delimitor) +
                                std::string("timestamp")  + std::string(cm_delimitor) +
                                std::string("source")     + std::string(cm_delimitor) +
                                std::string("message");

        uint8_t SUCCESS = 0;
        uint8_t INVALID_FILEPATH = 1;
        uint8_t FILE_ACCESS_ERROR = 2;
        uint8_t LOGFILE_ALREADY_CLOSED = 3;
        uint8_t LOGFILE_NOT_OPEN = 4;
        uint8_t LOGFILE_PATH_NOT_DEFINED = 5;
        
        std::unordered_map<uint8_t, std::string> cm_return_codes = {
            {SUCCESS,                   std::string("no error")},
            {INVALID_FILEPATH,          std::string("filepath exists but is not a regular file")},
            {FILE_ACCESS_ERROR,         std::string("logfile could not be opened or created")},
            {LOGFILE_ALREADY_CLOSED,    std::string("logfile was already closed")},
            {LOGFILE_NOT_OPEN,          std::string("logfile is closed")},
            {LOGFILE_PATH_NOT_DEFINED,  std::string("logfile path must be provided")}
        };
	};
}

#endif