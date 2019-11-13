#pragma once
#include "component.h"
#include "link.h"

class XOR :
	public Component
{
public:
	XOR(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }

	void compute() override {
		if (*outputs[0]->poweredNext)
			return;
#pragma optimize( "", off )
		unsigned int c = 0;
		for (unsigned int i = 0; i < inputCount; i++) {
			c += *inputs[i]->poweredCurrent;
		}
		if (c % 2)
			*outputs[0]->poweredNext = true;
#pragma optimize( "", on )
	}
};

