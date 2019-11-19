#pragma once
#include "component.h"
#include "link.h"
#include <string>

class NOT :
	public Component
{
public:
	NOT(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) { }

#ifndef __EMSCRIPTEN__
#pragma optimize( "", off )
#endif
	void compute() override {
		if (/**outputs[0]->poweredNext || */*inputs[0]->poweredCurrent) {
			outputs[0]->update();
			return;
		}
		outputs[0]->setPowered(true);
		outputs[0]->update();
	}
#pragma optimize( "", on )
};

