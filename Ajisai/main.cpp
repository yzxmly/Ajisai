#include "Viewer.h"

int main() {
	Viewer app;

	try {
		app.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	int n;
	std::cin >> n;
	return EXIT_SUCCESS;
}

