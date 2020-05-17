#include <server/server.h>

#include <shared/console/console.h>

int main()
{
	console::init_basic();

	Server* sv = new Server();

	sv->init();

	while (!GetAsyncKeyState(VK_F8))
	{
		sv->dispatch_packets();

		std::this_thread::sleep_for(std::chrono::microseconds(7812));
	}

	delete sv;

	return 0;
}