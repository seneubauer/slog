#include <slog_types.h>
#include <slog.h>
#include <thread>
#include <iostream>
#include <cstdint>

void generator(SimpleLog::slog *p_s, const SimpleLog::Severity &severity, const std::string &source, const uint32_t &pid, const std::string &message) {
	SimpleLog::Event event;

    event.severity = severity;
    event.source = source;
    event.pid = pid;
	event.message = message;
    
	p_s->log(event);
}

int main() {
    std::string error;
    SimpleLog::LogTarget targets = SimpleLog::LogTarget::file;
    
	SimpleLog::slog s(targets, SimpleLog::OperatingSystem::windows);
    s.set_file_parameters("app_log", 64);
    
    if (!s.start(error))
        return 1;
	
    uint32_t pid_0 = 0;
    uint32_t pid_1 = 1;
    
	std::thread t0(generator, &s, SimpleLog::Severity::warning, "testapp", pid_0, "message 0");
	std::thread t1(generator, &s, SimpleLog::Severity::critical, "testapp", pid_1, "message 1");
    
    t0.join();
	t1.join();
	
	return 0;
}