#include <eventlog_writer.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

// im tired, boss
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS               // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES           // VK_*
#define NOWINMESSAGES               // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES                 // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS                // SM_*
#define NOMENUS                     // MF_*
#define NOICONS                     // IDI_*
#define NOKEYSTATES                 // MK_*
#define NOSYSCOMMANDS               // SC_*
#define NORASTEROPS                 // Binary and Tertiary raster ops
#define NOSHOWWINDOW                // SW_*
#define OEMRESOURCE                 // OEM Resource values
#define NOATOM                      // Atom Manager routines
#define NOCLIPBOARD                 // Clipboard routines
#define NOCOLOR                     // Screen colors
#define NOCTLMGR                    // Control and Dialog routines
#define NODRAWTEXT                  // DrawText() and DT_*
#define NOGDI                       // All GDI defines and routines
#define NOKERNEL                    // All KERNEL defines and routines
#define NOUSER                      // All USER defines and routines
#define NONLS                       // All NLS defines and routines
#define NOMB                        // MB_* and MessageBox()
#define NOMEMMGR                    // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE                  // typedef METAFILEPICT
// #define NOMINMAX                 // Macros min(a,b) and max(a,b) || already set in mingw os_defines.h
#define NOMSG                       // typedef MSG and associated routines
#define NOOPENFILE                  // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL                    // SB_* and scrolling routines
#define NOSERVICE                   // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND                     // Sound driver routines
#define NOTEXTMETRIC                // typedef TEXTMETRIC and associated routines
#define NOWH                        // SetWindowsHook and WH_*
#define NOWINOFFSETS                // GWL_*, GCL_*, associated routines
#define NOCOMM                      // COMM driver routines
#define NOKANJI                     // Kanji support stuff.
#define NOHELP                      // Help engine interface.
#define NOPROFILER                  // Profiler interface.
#define NODEFERWINDOWPOS            // DeferWindowPos routines
#define NOMCX                       // Modem Configuration Extensions
#include <windows.h>

void SimpleLog::eventlog_writer::define(const std::string &application) {
    m_defined = true;
    m_application = application;
}

uint8_t SimpleLog::eventlog_writer::start() {
    if (!m_defined)
        return PARAMETERS_NOT_DEFINED;

    /*
    key_path needs to admin privilage to be created, so wrap it into the installer
    key_path=SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\app_name
    */

    mp_eventloghandle = RegisterEventSource(nullptr, m_application.c_str());
    if (!mp_eventloghandle) {
        m_lasterror = get_last_error();
        return NOGET_EVENTLOG_HANLDE;
    }

    // need to define mp_user_sid (PSID)
    HANDLE h_token = nullptr;
    DWORD length = 0;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &h_token)) {
        m_lasterror = get_last_error();
        return NOGET_PROCESS_TOKEN;
    }
    GetTokenInformation(h_token, TokenUser, nullptr, 0, &length);

    std::vector<BYTE> buffer(length);
    PTOKEN_USER p_token_user = reinterpret_cast<PTOKEN_USER>(buffer.data());
    if (!GetTokenInformation(h_token, TokenUser, p_token_user, length, &length)) {
        m_lasterror = get_last_error();
        CloseHandle(h_token);
        return NOGET_USER_TOKEN;
    }

    mp_user_sid = p_token_user->User.Sid;
    if (!mp_user_sid) {
        m_lasterror = get_last_error();
        return NOGET_USER_PSID;
    }

    return SUCCESS;
}

uint8_t SimpleLog::eventlog_writer::log(const uint16_t &event_type_id, const uint16_t &category_id, const uint32_t &event_id, const std::string &message) {

    WORD string_count = 1;
    DWORD data_size = 0;
    LPCSTR strings[1];
    strings[0] = message.c_str();
    void* data_blob = nullptr;

    // report event
    if (!ReportEvent(mp_eventloghandle, event_type_id, category_id, event_id, mp_user_sid, string_count, data_size, strings, data_blob)) {
        m_lasterror = get_last_error();
        return LOG_FAILURE;
    }

    return SUCCESS;
}

void SimpleLog::eventlog_writer::stop() {
    DeregisterEventSource(mp_eventloghandle);
    CloseHandle(mp_user_sid);
    CloseHandle(mp_eventloghandle);
}

const std::string SimpleLog::eventlog_writer::return_code_def(const uint8_t &return_code) {

    auto iterator = cm_return_codes.find(return_code);
    if (iterator != cm_return_codes.end())
        return iterator->second;
    
    return std::string();
}

const std::string SimpleLog::eventlog_writer::get_last_error() {
    LPSTR message_buffer = nullptr;
    DWORD lasterror = GetLastError();
    DWORD chars = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        nullptr,
        lasterror,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&message_buffer),
        0,
        nullptr
    );

    if (chars == 0 || !message_buffer)
        return "unknown error: " + std::to_string(lasterror);

    std::string output(message_buffer);
    LocalFree(message_buffer);

    return output;
    /*
    size_t message_length = wcslen(message_buffer);
    while (message_length > 0) {
        wchar_t c = message_buffer[message_length - 1];
        if (c == L'\r' || c == L'\n' || c == L' ' || c == L'.') {
            --message_length;
        } else {
            break;
        }
    }
    std::wstring_view msg{message_buffer, message_length};
    
    int needed = WideCharToMultiByte(
        CP_UTF8,
        0,
        msg.data(),
        static_cast<int>(msg.size()),
        nullptr,
        0,
        nullptr,
        nullptr
    );
    
    std::string output;
    if (needed > 0) {
        output.resize(static_cast<size_t>(needed));
        WideCharToMultiByte(
            CP_UTF8,
            0,
            msg.data(),
            static_cast<int>(msg.size()),
            output.data(),
            needed,
            nullptr,
            nullptr
        );
    } else {
        output = "unknown error: " + std::to_string(lasterror);
    }
    LocalFree(message_buffer);
    return output;
    */
}
