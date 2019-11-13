#pragma once
#include "component.h"
#include "link.h"

class AND :
	public Component
{
public:
	AND(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }

	void compute() override {
		/*if (outputs[0]->poweredNext)
			return;

		if (outputs[0].getLink()->lastUpdateTick != this->board->getCurrentTick())
		{
			for (unsigned int i = 0; i < inputCount; i++) {
				if (!inputs[i].getPowered()) {
					outputs[0].setPowered(false);
					return;
				}
			}
			outputs[0].setPowered(true);
			outputs[0].getLink()->lastUpdateTick = this->board->getCurrentTick();
		}*/
		if (outputs[0]->poweredNext)
			return;
#pragma optimize( "", off )
		for (unsigned int i = 0; i < inputCount; i++) {
			if (!*inputs[0]->poweredCurrent) {
				return;
			}
		}
		*outputs[0]->poweredNext = true;
#pragma optimize( "", on )
	}

	static AND* generateOptimized(Board* board, Link** inputs, Link** outputs, unsigned int inputCount);
private:

};

class AND_2 :
	public AND
{
public:
	AND_2(Board* board, Link** inputs, Link** outputs) : AND(board, inputs, outputs, 2) { }

	void compute() override {
		if (outputs[0]->poweredNext)
			return;
#pragma optimize( "", off )
		if (*inputs[0]->poweredCurrent && *inputs[1]->poweredCurrent)
			*outputs[0]->poweredNext = true;
#pragma optimize( "", on )
	}
private:
};

class AND_3 :
	public AND
{
public:
	AND_3(Board* board, Link** inputs, Link** outputs) : AND(board, inputs, outputs, 3) { }

	void compute() override {
		if (outputs[0]->poweredNext)
			return;
#pragma optimize( "", off )
		if (*inputs[0]->poweredCurrent && *inputs[1]->poweredCurrent && *inputs[2]->poweredCurrent)
			*outputs[0]->poweredNext = true;
#pragma optimize( "", on )
	}
private:
};

inline AND* AND::generateOptimized(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) {
	switch (inputCount) {
	case 2:
		return new AND_2(board, inputs, outputs);
	case 3:
		return new AND_3(board, inputs, outputs);
	default:
		return new AND(board, inputs, outputs, inputCount);
	}
}