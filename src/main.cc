#include "app/app.hh"

using namespace spoon;

int main(int argc, char* argv[]) {
	app* application = new app();
	application->run();
	delete application;

	return 0;
}
