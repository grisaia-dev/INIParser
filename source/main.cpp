#include "../lib/ini_parser.hpp"

int main(void) {

	INIP::Parser parser("../confsig.ini");
	parser.Print();
	std::cout << "-------------- in main.cpp ---------------" << std::endl;
	int a = parser.get_value<int>("owner.age");
	std::cout << " --> " << a << std::endl;

	return 0;
}