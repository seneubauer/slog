#include <file_writer.h>
#include <string>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <unordered_map>

void SimpleLog::file_writer::define(const std::string &filepath, const uint16_t &buffersize, const std::string &delimitor, const std::string &file_extension) {
    m_filepath = std::filesystem::path(filepath);
    m_buffersize = buffersize;
    m_delimitor = delimitor;
    m_file_extension = file_extension;
    m_header = std::string("severity")  + m_delimitor +
               std::string("timestamp") + m_delimitor +
               std::string("source")    + m_delimitor +
               std::string("message");
    m_defined = true;
}

uint8_t SimpleLog::file_writer::start() {
    if (!m_defined)
        return PARAMETERS_NOT_DEFINED;

    if (m_filepath.empty())
        return FILE_PATH_NOT_DEFINED;

    std::filesystem::path filepath = m_filepath;
    if (!filepath.has_extension()) {
        filepath += m_file_extension;
    } else {
        if (filepath.extension() != m_file_extension)
            filepath.replace_extension(m_file_extension);
    }

    bool add_header = true;
    std::filesystem::directory_entry file(filepath);
    if (file.exists()) {
        if (!file.is_regular_file())
            return INVALID_FILEPATH;

        add_header = false;
    }

    m_file.open(file.path(), std::ios_base::app);
    if (!m_file.is_open())
        return FILE_ACCESS_ERROR;

    if (add_header) {
        m_file << m_header << "\n";
        m_file.flush();
    }
    m_currentrow = 0;
    return SUCCESS;
}

uint8_t SimpleLog::file_writer::log(const std::string &severity, const std::string &timestamp, const std::string &source, const std::string &message) {
    if (!m_file.is_open())
        return FILE_NOT_OPEN;

    m_file << severity << m_delimitor << timestamp << m_delimitor << source << m_delimitor << message << '\n';

    m_currentrow++;
    if (m_currentrow >= m_buffersize) {
        m_file.flush();
        m_currentrow = 0;
    }

    return SUCCESS;
}

void SimpleLog::file_writer::stop() {
    if (!m_file.is_open())
        return;

    m_file.flush();
    m_file.close();
}

const std::string SimpleLog::file_writer::return_code_def(const uint8_t &return_code) {

    auto iterator = cm_return_codes.find(return_code);
    if (iterator != cm_return_codes.end())
        return iterator->second;

    return std::string();
}
