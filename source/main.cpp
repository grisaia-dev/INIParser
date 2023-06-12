#include "../lib/ini_parser.hpp"

int main(void) {
	INIP::Parser parser("../config.ini");
	parser.Print();
	auto a = parser.get_value<std::string>("database", "OS");
	std::cout << a;
	
	return 0;
}