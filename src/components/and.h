#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class AND :
	public Component
{
public:
	AND(Board* board, Input* inputs, Output* outputs, unsigned int inputCount) : Component(board, inputs, outputs, inputCount, getOutputCount()) {
		functionSelect(inputCount);
	}
	AND(Board* board, Link** inputs, Link** outputs, unsigned int inputCount) : Component(board, inputs, outputs, inputCount, getOutputCount()) {
		functionSelect(inputCount);
	}

	int getInputCount() {
		return 2;
	}

	int getOutputCount() {
		return 1;
	}

	void compute() {
		computeInternal();
	}
private:
	std::function<void()> computeInternal = [this]() {};

	void functionSelect(unsigned int inputCount) {
		switch (inputCount) {
			case 2:
				computeInternal = [&]() {
					outputs[0].setPowered(inputs[0].getPowered() && inputs[1].getPowered());
				};
				break;
			case 3:
				computeInternal = [&]() {
					outputs[0].setPowered(inputs[0].getPowered() && inputs[1].getPowered() && inputs[2].getPowered());
				};
				break;
			default:
				computeInternal = [&]() {
					for (int i = 0; i < getInputCount(); i++) {
						if (!inputs[i].getPowered()) {
							outputs[0].setPowered(false);
							return;
						}
					}
					outputs[0].setPowered(true);
				};
		}
	}
};

