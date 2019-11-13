#pragma once
#include "component.h"
#include "link.h"

class DELAY :
	public Component
{
public:
	DELAY(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) { }

	void compute() override {
		//if (outputs[0]->poweredNext)
		//	return;
#pragma optimize( "", off )
		if (*inputs[0]->poweredCurrent)
			*outputs[0]->poweredNext = true;
#pragma optimize( "", on )
	}
};

