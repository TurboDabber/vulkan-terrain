#include "app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

void compileShaders()
{
	system("compile.bat");
}

int main()
{
	compileShaders();
	app::DefaultApp application{};
	try {
		application.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
		return EXIT_FAILURE;
	}
	return 0;
}
