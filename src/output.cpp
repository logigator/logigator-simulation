#include "link.h"
#include "board.h"
#include "component.h"

void Output::setPowered(const bool state)
{
	if (state != powered) {
		powered = state;
		link->board->writeBuffer->push(link);
	}
}