#include <slog_types.h>
#include <slog.h>
#include <thread>
#include <iostream>

void generator(SimpleLog::slog *s, const int &value) {
	SimpleLog::Event event;
	event.message = std::string("value: ") + std::to_string(value);
	s->log(event);
}

int main() {
	
	std::cout << std::string("started...") << std::endl;
	
	SimpleLog::slog s(SimpleLog::LogTarget::csv, true);
	
	s.start();
	
	std::thread t0(generator, &s, 0);
	
	std::thread t1(generator, &s, 1);
	
	s.stop();
	
	t0.join();
	t1.join();
	
	std::cout << std::string("finished...") << std::endl;
	
	std::string exit_clause;
	std::getline(std::cin, exit_clause);
	
	return 0;
}