add_library(
	SimpleLog STATIC
	src/slog.cpp
    src/logfile_writer.cpp
    src/sl_functions.cpp
)

target_include_directories(
	SimpleLog
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
	target_compile_options(SimpleLog PRIVATE /W4 /EHsc)
else()
	target_compile_options(SimpleLog PRIVATE -Wall -Wextra -Wpedantic -02)
endif()