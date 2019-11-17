#pragma once
#include "component.h"
#include "link.h"

class DELAY :
	public Component
{
public:
	DELAY(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) { }

#pragma optimize( "", off )
	void compute() override {
		//if (*inputs[0]->poweredNext)
		//	return;
		if (*inputs[0]->poweredCurrent)
			outputs[0]->setPowered(true);
	}
#pragma optimize( "", on )
};

