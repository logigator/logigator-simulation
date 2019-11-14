#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class OR :
	public Component
{
public:
	OR(Board* board, Input* inputs, Output* outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }
	OR(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }

	void compute() override {
		for (unsigned int i = 0; i < inputCount; i++) {
			if (inputs[i].getPowered() == true) {
				outputs[0].setPowered(true);
				return;
			}
		}
		outputs[0].setPowered(false);
	}
};

