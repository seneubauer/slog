#include <eventlog_writer.h>
#include <cstdint>
#include <string>
#include <unordered_map>

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

void SimpleLog::eventlog_writer::define(const std::wstring &application) {
    m_defined = true;
    m_application = application;
}

uint8_t SimpleLog::eventlog_writer::start() {
    if (!m_defined)
        return PARAMETERS_NOT_DEFINED;

    DWORD types_supported = EVENTLOG_SUCCESS | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE | EVENTLOG_AUDIT_SUCCESS | EVENTLOG_AUDIT_FAILURE;
    std::wstring key_path = L"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" + m_application;
    HKEY h_key;
    LSTATUS return_status;

    // make sure the registry path is defined
    return_status = RegCreateKeyExW(HKEY_LOCAL_MACHINE, key_path.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &h_key, nullptr);
    if (return_status != ERROR_SUCCESS)
        return COULD_NOT_SET_KEYPATH;

    // register the key
    return_status = RegSetValueExW(h_key, L"TypesSupported", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&types_supported), sizeof(types_supported));
    if (return_status != ERROR_SUCCESS)
        return COULD_NOT_SET_KEY;

    RegCloseKey(h_key);

    // register the source

    return SUCCESS;
}

uint8_t SimpleLog::eventlog_writer::log() {
    
    // report event
    
    return 0;
}

void SimpleLog::eventlog_writer::stop() {
    
    // deregister the source
}

const std::string SimpleLog::eventlog_writer::return_code_def(const uint8_t &return_code) {
    
    auto iterator = cm_return_codes.find(return_code);
    if (iterator != cm_return_codes.end())
        return iterator->second;
    
    return std::string();
}

const std::string SimpleLog::eventlog_writer::get_last_error() {
    
}