#pragma once
#include <math.h>
#include "component.h"
#include "output.h"
#include "input.h"
#include "board.h"
#include "events.h"
#include "link.h"

class ROM :
	public Component
{
public:
	int speed = 1;

	ROM(Board* board,
		Input* inputs,
		Output* outputs,
		const unsigned int inputCount,
		const unsigned int outputCount)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		if (inputCount <= 2 || outputCount <= 0)
		{
			return;
		}
		this->data = new bool[outputCount * (unsigned int)pow(2, inputCount - 2)] { false };
	}

	ROM(Board* board,
		Input* inputs,
		Output* outputs,
		const unsigned int inputCount,
		const unsigned int outputCount,
		const unsigned int wordCount,
		const bool* data)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		if (inputCount <= 2 || outputCount <= 0)
		{
			return;
		}
		this->data = new bool[outputCount * (unsigned int)pow(2, inputCount - 2)]{ false };

		for (unsigned int i = 0; i < wordCount * outputCount; i++)
		{
			this->data[i] = data[i];
		}
	}

	ROM(Board* board,
		Link** inputs,
		Link** outputs,
		unsigned int inputCount,
		unsigned int outputCount)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		if (inputCount <= 2 || outputCount <= 0)
		{
			return;
		}
		this->data = new bool[outputCount * (unsigned int)pow(2, inputCount - 2)]{ false };
	}

	ROM(Board* board,
		Link** inputs,
		Link** outputs,
		const unsigned int inputCount,
		const unsigned int outputCount,
		const unsigned int wordCount,
		const bool* data)
		: Component(board, inputs, outputs, inputCount, outputCount)
	{
		if (inputCount <= 2 || outputCount <= 0)
		{
			return;
		}
		this->data = new bool[outputCount * (unsigned int)pow(2, inputCount)]{ false };
		
		for (unsigned int i = 0; i < wordCount * outputCount; i++)
		{
			this->data[i] = data[i];
		}
	}

	~ROM() {
		delete[] data;
	}

	void compute() override {
		unsigned int position = 0;

		for (auto i = this->inputCount - 1; i <= 0; i++)
		{
			position |= this->inputs[i].getPowered() << i;
		}
		
		for (unsigned int i = 0; i < this->outputCount; i++)
		{
			this->outputs[i].setPowered(data[position + i]);
		}
	}
private:
	bool* data;
};
