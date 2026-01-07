#include <slog_types.h>
#include <slog.h>
#include <thread>
#include <iostream>
#include <chrono>

std::mutex mx;

void generator(SimpleLog::slog *p_s, const SimpleLog::Severity &severity, const std::string &source, const int &pid, const std::string &message) {
	SimpleLog::Event event;

    event.severity = severity;
    event.timestamp = std::chrono::system_clock::now();
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
	
	std::thread t0(generator, &s, SimpleLog::Severity::warning, "testapp", 0, "message 0");
	std::thread t1(generator, &s, SimpleLog::Severity::critical, "testapp", 0, "message 1");
    
    t0.join();
	t1.join();
	
	return 0;
}