#pragma once
#include <cmath>
#include "component.h"
#include "output.h"
#include "input.h"
#include "board.h"
#include "link.h"

class RAM:
	public Component
{
public:
	RAM(Board* board,
		Input* inputs,
		Output* outputs,
		const size_t addressSize,
		const size_t wordSize)
	: Component(board, inputs, outputs, wordSize + addressSize + 2, wordSize)
	{
		this->data = new unsigned char[static_cast<size_t>(ceil(wordSize * pow(2, addressSize) / CHAR_BIT))] { 0 };
		this->wordSize = wordSize;
		this->addressSize = addressSize;
	}

	RAM(Board* board,
		Link** inputs,
		Link** outputs,
		const size_t addressSize,
		const size_t wordSize)
	: Component(board, inputs, outputs, wordSize + addressSize + 2, wordSize)
	{
		this->data = new unsigned char[static_cast<size_t>(ceil(wordSize * pow(2, addressSize) / CHAR_BIT))] { 0 };
	}

	~RAM()
	{
		delete[] data;
	}

	void compute() override
	{
		if (this->inputs[this->inputCount - 1].getPowered() && !prevClk)
		{
			size_t position = 0;

			for (size_t i = 0; i < this->addressSize; i++) {
				position |= static_cast<size_t>(this->inputs[i].getPowered()) << i;
			}
			position *= outputCount;
			
			if (this->inputs[this->inputCount - 2].getPowered())
			{
				for (size_t i = 0; i < this->wordSize; i++)
				{
					this->outputs[i].setPowered((this->data[(position + i) / CHAR_BIT] >> (position + i) % CHAR_BIT) % 2);
				}
			}
			else
			{
				for (size_t i = 0; i < this->wordSize; i++)
				{
					const auto pos = position + i;
					const auto offset = pos % CHAR_BIT;
					this->data[pos / CHAR_BIT] = (this->data[pos / CHAR_BIT] & ~(1 << offset)) | this->inputs[i].getPowered() << offset;
				}
			}
		}
		prevClk = this->inputs[this->inputCount - 1].getPowered();
	}

	void init() override
	{
		this->compute();
	}

private:
	bool prevClk = false;
	size_t wordSize;
	size_t addressSize;
	unsigned char* data;
};
