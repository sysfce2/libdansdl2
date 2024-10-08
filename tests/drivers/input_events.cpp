#include "../../include/ldv/screen.h"
#include "../../include/ldi/sdl_input.h"
#include "../../include/ldt/sdl_tools.h"
#include "../class/input_events.h"

int main(int /*argc*/, char ** /*argv*/) {

	ldt::sdl_init();

	//Basic skeleton: video and input...
	ldv::screen screen(64, 64);
	ldi::sdl_input input;

	ldtest::input_events test;
	test.run(screen, input);

	ldt::sdl_shutdown();

	return 0;
}
