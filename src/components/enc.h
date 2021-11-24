#pragma once
#include <cmath>
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class ENC :
	public Component
{
public:
	ENC(Board* board, Input* inputs, Output* outputs, const size_t outputCount) : Component(board, inputs, outputs, static_cast<size_t>(pow(2, outputCount)), outputCount) { }
	ENC(Board* board, Link** inputs, Link** outputs, const size_t outputCount) : Component(board, inputs, outputs, static_cast<size_t>(pow(2, outputCount)), outputCount) { }

	void compute() override
	{
		size_t value = 0;
		for (size_t i = inputCount - 1; i > 0; i--) {
			if (inputs[i].getPowered()) {
				value = i;
				break;
			}
		}
		for (size_t i = 0; i < outputCount; i++) {
			outputs[i].setPowered((value & (1ULL << i)) >> i);
		}
	}
private:
};

