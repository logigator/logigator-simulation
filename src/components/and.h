#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class AND :
	public Component
{
public:
	AND(Board* board, Input* inputs, Output* outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }
	AND(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) : Component(board, inputs, outputs, inputCount, 1) { }

	void compute() override {
		for (unsigned int i = 0; i < inputCount; i++) {
			if (!inputs[i].getPowered()) {
				outputs[0].setPowered(false);
				return;
			}
		}
		outputs[0].setPowered(true);
	}

	static AND* generateOptimized(Board* board, Link** inputs, Link** outputs, unsigned int inputCount);
private:

};

class AND_2 :
	public AND
{
public:
	AND_2(Board* board, Input* inputs, Output* outputs) : AND(board, inputs, outputs, 2) { }
	AND_2(Board* board, Link** inputs, Link** outputs) : AND(board, inputs, outputs, 2) { }

	void compute() override {
		outputs[0].setPowered(inputs[0].getPowered() && inputs[1].getPowered());
	}
private:
};

class AND_3 :
	public AND
{
public:
	AND_3(Board* board, Input* inputs, Output* outputs) : AND(board, inputs, outputs, 2) { }
	AND_3(Board* board, Link** inputs, Link** outputs) : AND(board, inputs, outputs, 2) { }

	void compute() override {
		outputs[0].setPowered(inputs[0].getPowered() && inputs[1].getPowered() && inputs[2].getPowered());
	}
private:
};

inline AND* AND::generateOptimized(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount) {
	switch (inputCount) {
	case 2:
		return new AND_2(board, inputs, outputs);
	case 3:
		return new AND_3(board, inputs, outputs);
	default:
		return new AND(board, inputs, outputs, inputCount);
	}
}