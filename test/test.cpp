#include <slog.h>
#include <slog_types.h>
#include <thread>
#include <iostream>

void generator(SimpleLog::slog *p_s, const SimpleLogTypes::Severity &severity, const std::string &source, const std::string &message) {
	SimpleLogTypes::Event event;

    event.severity = severity;
    event.source = source;
	event.message = message;

	p_s->log(event);
}

int main() {
    std::string error;
    SimpleLogTypes::LoggingTarget targets = SimpleLogTypes::LoggingTarget::file | SimpleLogTypes::LoggingTarget::console;

	SimpleLog::slog s(targets, SimpleLogTypes::OperatingSystem::windows);
    if (!s.set_parameters_file("test_log", 64, "||", ".slog", error))
        return 1;

    if (!s.start(error))
        return 1;

	std::thread t0(generator, &s, SimpleLogTypes::Severity::warning, "testapp", "message 0");
	std::thread t1(generator, &s, SimpleLogTypes::Severity::critical, "testapp", "message 1");

    t0.join();
	t1.join();

	return 0;
}