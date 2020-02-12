#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class NOT :
	public Component
{
public:
	NOT(Board* board, Input* inputs, Output* outputs) : Component(board, inputs, outputs, 1, 1) { }
	NOT(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) { }

	void init() override
	{
		outputs[0].setPowered(true);
	}
	
	void compute() override {
		outputs[0].setPowered(!inputs[0].getPowered());
	}
};

