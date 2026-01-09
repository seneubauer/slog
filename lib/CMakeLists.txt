add_library(
	SimpleLog STATIC
	src/slog.cpp
    src/file_writer.cpp
    src/eventlog_writer.cpp
)

target_link_libraries(SimpleLog PRIVATE AdvAPI32.Lib)

target_include_directories(
	SimpleLog
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
	target_compile_options(SimpleLog PRIVATE /W4 /EHsc)
else()
	target_compile_options(SimpleLog PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()