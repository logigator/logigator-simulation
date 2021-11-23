#pragma once
#include <cmath>
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class DEMUX :
	public Component
{
public:
    DEMUX(Board* board, Input* inputs, Output* outputs, const size_t inputCount) : Component(board, inputs, outputs, inputCount, pow(2, inputCount - 1)) { }
    DEMUX(Board* board, Link** inputs, Link** outputs, const size_t inputCount) : Component(board, inputs, outputs, inputCount, pow(2, inputCount - 1)) { }

	void compute() override
	{
		size_t index = 0;
		for (size_t i = 1; i < inputCount; i++) {
			index |= static_cast<size_t>(inputs[i].getPowered()) << (i - 1);
		}
        outputs[prev].setPowered(false);
        outputs[index].setPowered(inputs[0].getPowered());
		prev = index;
	}
private:
	size_t prev = 0;
};

