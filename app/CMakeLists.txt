add_executable(main main.cpp)

target_link_libraries(main PRIVATE SimpleLog)

if (MSVC)
	target_compile_options(main PRIVATE /W4 /EHsc)
else()
	target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic -02)
endif()