#include <logfile_writer.h>

uint8_t SimpleLog::logfile_writer::begin() {
    if (m_filepath.empty())
        return LOGFILE_PATH_NOT_DEFINED;

    std::filesystem::path filepath = m_filepath;
    if (!filepath.has_extension()) {
        filepath += cm_file_extension;
    } else {
        if (filepath.extension() != cm_file_extension)
            filepath.replace_extension(cm_file_extension);
    }

    bool add_header = true;
    std::filesystem::directory_entry logfile(filepath);
    if (logfile.exists()) {
        if (!logfile.is_regular_file())
            return INVALID_FILEPATH;

        add_header = false;
    }

    m_logfile.open(logfile.path(), std::ios_base::app);
    if (!m_logfile.is_open())
        return FILE_ACCESS_ERROR;

    if (add_header) {
        m_logfile << cm_header << "\n";
        m_logfile.flush();
    }
    m_currentrow = 0;
    return SUCCESS;
}

void SimpleLog::logfile_writer::end() {
    if (!m_logfile.is_open())
        return;

    m_logfile.flush();
    m_logfile.close();
}

uint8_t SimpleLog::logfile_writer::add_entry(const std::string &severity, const std::string &timestamp, const std::string &source, const std::string &message) {
    if (!m_logfile.is_open())
        return LOGFILE_NOT_OPEN;

    m_logfile << severity << cm_delimitor << timestamp << cm_delimitor << source << cm_delimitor << message << '\n';

    m_currentrow++;
    if (m_currentrow >= m_buffersize) {
        m_logfile.flush();
        m_currentrow = 0;
    }

    return SUCCESS;
}