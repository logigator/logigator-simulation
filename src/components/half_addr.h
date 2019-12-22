#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class HalfAddr :
	public Component
{
public:
	HalfAddr(Board* board, Input* inputs, Output* outputs) : Component(board, inputs, outputs, 2, 2) { }
	HalfAddr(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 2, 2) { }

	void compute() override {
		outputs[0].setPowered(inputs[0].getPowered() ^ inputs[1].getPowered());
		outputs[1].setPowered(inputs[0].getPowered() && inputs[1].getPowered());
	}
};

