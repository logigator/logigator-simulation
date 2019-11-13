#pragma once
#include "component.h"
#include "link.h"

class NOT :
	public Component
{
public:
	NOT(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) { }

	void compute() override {
		if (outputs[0]->poweredNext)
			return;
		*outputs[0]->poweredNext = !*inputs[0]->poweredCurrent;
	}
};

