#pragma once
#include <cmath>
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class DEC :
	public Component
{
public:
	DEC(Board* board, Input* inputs, Output* outputs, const size_t inputCount) : Component(board, inputs, outputs, inputCount, pow(2, inputCount)) { }
	DEC(Board* board, Link** inputs, Link** outputs, const size_t inputCount) : Component(board, inputs, outputs, inputCount, pow(2, inputCount)) { }

	void init() override
	{
		outputs[0].setPowered(true);
	}

	void compute() override
	{
		size_t index = 0;
		for (size_t i = 0; i < inputCount; i++) {
			index |= static_cast<size_t>(inputs[i].getPowered()) << i;
		}
		outputs[index].setPowered(true);
		outputs[prev].setPowered(false);
		prev = index;
	}
private:
	size_t prev = 0;
};

