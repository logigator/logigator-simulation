#pragma once
#include <cmath>
#include "component.h"
#include "output.h"
#include "input.h"
#include "board.h"
#include "link.h"

class ROM :
	public Component
{
public:
	ROM(Board* board,
		Input* inputs,
		Output* outputs,
		const size_t inputCount,
		const size_t outputCount)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		this->data = new unsigned char[static_cast<size_t>(ceil(outputCount * pow(2, inputCount) / CHAR_BIT))] { 0 };
	}

	ROM(Board* board,
		Input* inputs,
		Output* outputs,
		const size_t inputCount,
		const size_t outputCount,
		const size_t wordCount,
		const unsigned char* data)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		size_t size = static_cast<size_t>(ceil(outputCount * pow(2, inputCount) / CHAR_BIT));
		this->data = new unsigned char[size] { 0 };

		for (size_t i = 0; i < wordCount && i < size; i++)
		{
			this->data[i] = data[i];
		}
	}

	ROM(Board* board,
		Link** inputs,
		Link** outputs,
		const size_t inputCount,
		const size_t outputCount)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		this->data = new unsigned char[static_cast<size_t>(ceil(outputCount * pow(2, inputCount) / CHAR_BIT))] { 0 };
	}

	ROM(Board* board,
		Link** inputs,
		Link** outputs,
		const size_t inputCount,
		const size_t outputCount,
		const size_t wordCount,
		const unsigned char* data)
		: Component(board, inputs, outputs, inputCount, outputCount)
	{
		size_t size = static_cast<size_t>(ceil(outputCount * pow(2, inputCount) / CHAR_BIT));
		this->data = new unsigned char[size] { 0 };
		
		for (size_t i = 0; i < wordCount && i < size; i++)
		{
			this->data[i] = data[i];
		}
	}

	~ROM()
	{
		delete[] data;
	}

	void compute() override
	{
		size_t position = 0;

		for (size_t i = 0; i < this->inputCount; i++) {
			position |= static_cast<size_t>(this->inputs[i].getPowered()) << i;
		}
		position *= outputCount;
		
		for (size_t i = 0; i < this->outputCount; i++)
		{
			this->outputs[i].setPowered(static_cast<unsigned char>(data[(position + i) / CHAR_BIT] << (CHAR_BIT - 1 - (position + i) % 8)) >> (CHAR_BIT - 1));
		}
	}

	void init() override
	{
		this->compute();
	}

private:
	unsigned char* data;
};
