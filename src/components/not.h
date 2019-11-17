#pragma once
#include "component.h"
#include "link.h"

class NOT :
	public Component
{
public:
	NOT(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) { }

#pragma optimize( "", off )
	void compute() override {
		if (/**inputs[0]->poweredNext || */*inputs[0]->poweredCurrent)
			return;
		outputs[0]->setPowered(true);
	}
#pragma optimize( "", on )
};

