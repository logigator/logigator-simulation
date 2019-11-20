#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class FullAddr :
	public Component
{
public:
	FullAddr(Board* board, Input* inputs, Output* outputs) : Component(board, inputs, outputs, 3, 2) { }
	FullAddr(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 3, 2) { }

	void compute() override {
		outputs[0].setPowered(inputs[0].getPowered() ^ inputs[1].getPowered() ^ inputs[2].getPowered());
		outputs[1].setPowered(inputs[0].getPowered() && inputs[1].getPowered() || inputs[0].getPowered() ^ inputs[1].getPowered() && inputs[2].getPowered());
	}
};

