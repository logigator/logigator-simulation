#pragma once
#include <cmath>
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class MUX :
	public Component
{
public:
	MUX(Board* board,
		Input* inputs,
		Output* outputs,
		const size_t selectBits)
		: Component(board, inputs, outputs, static_cast<size_t>(pow(2, selectBits)) + selectBits, 1)
	{
		this->selectBits = selectBits;
	}

	MUX(Board* board,
		Link** inputs,
		Link** outputs,
		const size_t selectBits)
		: Component(board, inputs, outputs, static_cast<size_t>(pow(2, selectBits)) + selectBits, 1)
	{
		this->selectBits = selectBits;
	}

	void compute() override
	{
		size_t index = 0;
		for (size_t i = 0; i < this->selectBits; i++) {
			index |= static_cast<size_t>(inputs[i].getPowered()) << i;
		}
		outputs[0].setPowered(inputs[this->selectBits + index].getPowered());
	}
private:
	size_t selectBits;
};

