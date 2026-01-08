add_executable(test test.cpp)

target_link_libraries(test PRIVATE SimpleLog)

if (MSVC)
	target_compile_options(test PRIVATE /W4 /EHsc)
else()
	target_compile_options(test PRIVATE -Wall -Wextra -Wpedantic -02)
endif()