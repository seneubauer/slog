#include <sl_functions.h>

uint32_t SimpleLog::current_process_id_windows() {
    return GetCurrentProcessId();
}