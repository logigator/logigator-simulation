#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class D_FF :
	public Component
{
public:
	D_FF(Board* board, Input* inputs, Output* outputs) : Component(board, inputs, outputs, 2, 2) { }
	D_FF(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 2, 2) { }

	void init() override
	{
		this->outputs[1].setPowered(true);
	}

	void compute() override
	{
		if (inputs[1].getPowered() && !prevClk)
		{
			outputs[0].setPowered(inputs[0].getPowered());
			outputs[1].setPowered(!inputs[0].getPowered());
		}
		prevClk = inputs[1].getPowered();
	}

private:
	bool prevClk = false;
};
