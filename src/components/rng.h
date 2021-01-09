#pragma once
#include "component.h"
#include <cmath>
#include "output.h"
#include "input.h"
#include "link.h"

class RNG :
	public Component
{
public:
	RNG(Board* board, Input* inputs, Output* outputs, const size_t outputCount) : Component(board, inputs, outputs, 1, outputCount) { }
	RNG(Board* board, Link** inputs, Link** outputs, const size_t outputCount) : Component(board, inputs, outputs, 1, outputCount) { }

	void compute() override
	{
		if (this->inputs[0].getPowered()){
			for (size_t i = 0; i < this->outputCount;)
			{
				const auto random = rand();
				for (auto j = 0; j < this->rand_bits; j++)
				{
					this->outputs[i++].setPowered((random >> j) % 2);
					if (i >= this->outputCount)
						break;
				}
			}
		}
	}
private:
	const int_fast32_t rand_bits = static_cast<int_fast32_t>(std::log2(RAND_MAX));
};
