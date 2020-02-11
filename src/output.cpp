#include "link.h"
#include "board.h"
#include "component.h"

void Output::setPowered(const bool state)
{
	if (state != powered) {
		powered = state;

		for (unsigned int i = 0; i < link->inputCount; i++)
			link->board->writeBuffer[std::atomic_fetch_add_explicit(&link->board->bufferCount, 1, std::memory_order_relaxed)] = component;
	}
}