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
		for (size_t i = inputCount - 1; i > 0; i--) {
			if (inputs[i].getPowered()) {
				for (size_t j = 0; j < outputCount; j++) {
					outputs[i].setPowered((i & (1ULL << j)) >> j);
				}
				return;
			}
		}
		
	}
private:
};

