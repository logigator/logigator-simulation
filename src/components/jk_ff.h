#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class JK_FF :
	public Component
{
public:
	JK_FF(Board* board, Input* inputs, Output* outputs) : Component(board, inputs, outputs, 3, 2) { }
	JK_FF(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 3, 2) { }

	void init() override
	{
		this->outputs[1].setPowered(true);
	}

	void compute() override {
		if (inputs[1].getPowered() && !prevClk)
		{
			if(inputs[0].getPowered() && inputs[2].getPowered())
			{
				outputs[0].setPowered(!outputs[0].getPowered());
				outputs[1].setPowered(!outputs[1].getPowered());
			} else if(inputs[0].getPowered())
			{
				outputs[0].setPowered(true);
				outputs[1].setPowered(false);
			} else if(inputs[2].getPowered())
			{
				outputs[0].setPowered(false);
				outputs[1].setPowered(true);
			}
		}
		prevClk = inputs[1].getPowered();
	}

private:
	bool prevClk = false;
};
