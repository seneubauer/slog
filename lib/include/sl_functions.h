#ifndef sl_functions_header_h
#define sl_functions_header_h

#define _AMD64_

#include <processthreadsapi.h>
#include <cstdint>

namespace SimpleLog {
    uint32_t current_process_id_windows();
}

#endif