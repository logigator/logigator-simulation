#pragma once
#include "component.h"
#include "link.h"

class OR :
	public Component
{
public:
	OR(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }

#pragma optimize( "", off )
	void compute() override {
		//if (*inputs[0]->poweredNext)
		//	return;
		bool isOn = false;
		for (unsigned int i = 0; i < inputCount; i++) {
			if (*inputs[0]->poweredCurrent) {
				isOn = true;
				break;
			}
		}
		if (isOn)
			outputs[0]->setPowered(true);
	}
#pragma optimize( "", on )
};

