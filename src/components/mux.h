#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"
#include <stdio.h>

class MUX :
	public Component
{
public:
    MUX(Board* board,
        Input* inputs,
        Output* outputs,
        const size_t inputCount,
        const size_t addressSize)
    : Component(board, inputs, outputs, inputCount, 1)
    {
        this->addressSize = addressSize;
    }

    MUX(Board* board,
        Link** inputs,
        Link** outputs,
        const size_t inputCount,
        const size_t addressSize)
    : Component(board, inputs, outputs, inputCount, 1)
    {
        this->addressSize = addressSize;
    }

	void compute() override
	{
        size_t index = 0;
        for (size_t i = 0; i < this->addressSize; i++) {
            index |= static_cast<size_t>(inputs[i].getPowered()) << i;
        }
        outputs[0].setPowered(inputs[this->addressSize + index].getPowered());
    }
private:
    size_t addressSize;
};

