#include "link.h"
#include "board.h"
#include "component.h"

void Output::setPowered(bool state)
{
	if (state != powered) {
		powered = state;

		for (unsigned int i = 0; i < link->inputCount; i++)
			link->board->writeBuffer[link->inputs[i]->getComponent()->componentIndex] = true;
	}
}