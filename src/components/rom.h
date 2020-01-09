#pragma once
#include <cmath>
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
	ROM(Board* board,
		Input* inputs,
		Output* outputs,
		const unsigned int inputCount,
		const unsigned int outputCount)
	: Component(board, inputs, outputs, inputCount, outputCount)
	{
		this->data = new unsigned char[(long)ceil(outputCount * pow(2, inputCount) / 8)] { 0 };
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
		this->data = new unsigned char[(long)ceil(outputCount * pow(2, inputCount) / 8)] { 0 };

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
		this->data = new unsigned char[(long)ceil(outputCount * pow(2, inputCount) / 8)] { 0 };
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
		this->data = new unsigned char[(long)ceil(outputCount * pow(2, inputCount) / 8)] { 0 };
		
		for (unsigned int i = 0; i < wordCount; i++)
		{
			this->data[i] = data[i];
		}

		auto linksString = std::string("[");
		for (auto i = 0; i < ceil(outputCount * pow(2, inputCount) / 8) - 1; i++) {
			linksString += std::to_string(this->data[i]) + std::string(", ");
		}
		linksString += std::to_string(this->data[(long)ceil(outputCount * pow(2, inputCount) / 8) - 1]);
		printf("%s", (linksString + std::string("]\n")).c_str());
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
			this->outputs[i].setPowered((unsigned char)(data[(position + i) / 8] << (7 - (position + i) % 8)) >> 7);
		}
	}
private:
	unsigned char* data;
};
