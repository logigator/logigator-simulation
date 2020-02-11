#pragma once
#include "input.h"
#include "output.h"
#include "link.h"
#include "board.h"
#include <cstring>

class Component
{
public:
	virtual ~Component()
	{
		delete[] inputs;
		delete[] outputs;
	}

	virtual void compute() = 0;
	
	Input* getInputs() const
	{
		return inputs;
	}

	Output* getOutputs() const
	{
		return outputs;
	}

	unsigned int getInputCount() const
	{
		return inputCount;
	};

	unsigned int getOutputCount() const
	{
		return outputCount;
	};
	
protected:
	Board* board;
	Input* inputs;
	Output* outputs;
	unsigned int inputCount;
	unsigned int outputCount;

	Component(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount, const unsigned int outputCount) :
		board(board),
		inputCount(inputCount),
		outputCount(outputCount)
	{
		this->inputs = new Input[inputCount];
		for (unsigned int i = 0; i < inputCount; i++) {
			new (&this->inputs[i]) Input(this, inputs[i]);
		}

		this->outputs = new Output[outputCount];
		for (unsigned int i = 0; i < outputCount; i++) {
			new (&this->outputs[i]) Output(this, outputs[i]);
		}


		for (unsigned int i = 0; i < inputCount; i++) {
			auto** newInputs = new Input* [inputs[i]->inputCount + 1u];
			std::memcpy(newInputs, inputs[i]->inputs, inputs[i]->inputCount * sizeof(Input*));
			newInputs[inputs[i]->inputCount] = &this->inputs[i];
			delete[] inputs[i]->inputs;
			inputs[i]->inputs = newInputs;
			inputs[i]->inputCount++;
		}

		for (unsigned int i = 0; i < outputCount; i++) {
			auto** newOutputs = new Output * [outputs[i]->outputCount + 1];
			std::memcpy(newOutputs, outputs[i]->outputs, outputs[i]->outputCount * sizeof(Output*));
			newOutputs[outputs[i]->outputCount] = &this->outputs[i];
			delete[] outputs[i]->outputs;
			outputs[i]->outputs = newOutputs;
			outputs[i]->outputCount++;
		}
	}

	Component(Board* board, Input* inputs, Output* outputs, const unsigned int inputCount, const unsigned int outputCount) :
		board(board),
		inputs(inputs),
		outputs(outputs),
		inputCount(inputCount),
		outputCount(outputCount)
	{
	}
};
