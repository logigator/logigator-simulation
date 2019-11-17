#pragma once
#include "component.h"
#include "link.h"

class XOR :
	public Component
{
public:
	XOR(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }

#pragma optimize( "", off )
	void compute() override {
		//if (*inputs[0]->poweredNext)
		//	return;
		unsigned int c = 0;
		for (unsigned int i = 0; i < inputCount; i++) {
			c += *inputs[i]->poweredCurrent;
		}
		if (c % 2)
			outputs[0]->setPowered(true);
	}
#pragma optimize( "", on )
};

