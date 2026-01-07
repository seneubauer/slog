add_executable(app main.cpp)

target_link_libraries(app PRIVATE SimpleLog)

if (MSVC)
	target_compile_options(app PRIVATE /W4 /EHsc)
else()
	target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic -02)
endif()