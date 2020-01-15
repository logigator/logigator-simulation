#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class SR_FF :
	public Component
{
public:
	SR_FF(Board* board, Input* inputs, Output* outputs) : Component(board, inputs, outputs, 3, 2)
	{
		this->outputs[1].setPowered(true);
	}
	
	SR_FF(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 3, 2)
	{
		this->outputs[1].setPowered(true);
	}

	void compute() override {
		if (inputs[1].getPowered())
		{
			if (inputs[0].getPowered())
			{
				outputs[0].setPowered(true);
				outputs[1].setPowered(false);
			} else if(inputs[2].getPowered())
			{
				outputs[0].setPowered(false);
				outputs[1].setPowered(true);
			}
		}
	}

private:

};
