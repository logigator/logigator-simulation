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
		const unsigned int inputCount,
		const unsigned int outputCount)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		this->data = new unsigned char[(long)ceil(outputCount * pow(2, inputCount) / CHAR_BIT)] { 0 };
	}

	ROM(Board* board,
		Input* inputs,
		Output* outputs,
		const unsigned int inputCount,
		const unsigned int outputCount,
		const unsigned int wordCount,
		const unsigned char* data)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		this->data = new unsigned char[(long)ceil(outputCount * pow(2, inputCount) / CHAR_BIT)] { 0 };

		for (unsigned int i = 0; i < wordCount; i++)
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
		this->data = new unsigned char[(long)ceil(outputCount * pow(2, inputCount) / CHAR_BIT)] { 0 };
	}

	ROM(Board* board,
		Link** inputs,
		Link** outputs,
		const unsigned int inputCount,
		const unsigned int outputCount,
		const unsigned int wordCount,
		const unsigned char* data)
		: Component(board, inputs, outputs, inputCount, outputCount)
	{
		this->data = new unsigned char[(long)ceil(outputCount * pow(2, inputCount) / CHAR_BIT)] { 0 };
		
		for (unsigned int i = 0; i < wordCount; i++)
		{
			this->data[i] = data[i];
		}
	}

	~ROM() {
		delete[] data;
	}

	void compute() override {
		unsigned long position = 0;

		for (unsigned int i = 0; i < this->inputCount; i++) {
			position |= this->inputs[i].getPowered() << i;
		}
		position *= outputCount;
		
		for (unsigned int i = 0; i < this->outputCount; i++)
		{
			this->outputs[i].setPowered((unsigned char)(data[(position + i) / CHAR_BIT] << (CHAR_BIT - 1 - (position + i) % 8)) >> (CHAR_BIT - 1));
		}
	}
private:
	unsigned char* data;
};
