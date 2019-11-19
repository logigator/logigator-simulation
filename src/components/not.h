#pragma once
#include "component.h"
#include "link.h"
#include <string>

class NOT :
	public Component
{
public:
	NOT(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) { }

#pragma optimize( "", off )
	void compute() override {
		outputs[0]->update();
		if (/**outputs[0]->poweredNext || */*inputs[0]->poweredCurrent) {
			printf("%s", "not NICHT\n");
			return;
		}
		outputs[0]->setPowered(true);
		printf("%s", "notiges not macht an\n");
	}
#pragma optimize( "", on )
};

