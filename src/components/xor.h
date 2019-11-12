#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class XOR :
	public Component
{
public:
	XOR(Board* board, Input* inputs, Output* outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }
	XOR(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }

	void compute() override {
		unsigned int c = 0;
		for (unsigned int i = 0; i < inputCount; i++) {
			c += inputs[i].getPowered();
		}
		outputs[0].setPowered(c % 2);
	}
};

