#pragma once
#include "component.h"
#include "link.h"

class DELAY :
	public Component
{
public:
	DELAY(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) { }

#ifndef __EMSCRIPTEN__
#pragma optimize( "", off )
#endif
	void compute() override {
//		if (*outputs[0]->poweredNext)
//			return;
		if (*inputs[0]->poweredCurrent)
		{
			outputs[0]->setPowered(true);
		}
		outputs[0]->update();
	}
#pragma optimize( "", on )
};

