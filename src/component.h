#pragma once
#include "link.h"
#include "board.h"

class Component
{
public:
	virtual ~Component()
	{
		delete[] inputs;
		delete[] outputs;
	}

	virtual void compute() = 0;

	unsigned int componentIndex;
	
	Link** getInputs() const
	{
		return inputs;
	}

	Link** getOutputs() const
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
	Link** inputs;
	Link** outputs;
	unsigned int inputCount;
	unsigned int outputCount;

	
	Component(Board* board, Link** inputs, Link** outputs, const unsigned int inputCount, const unsigned int outputCount) :
		board(board),
		inputs(inputs),
		outputs(outputs),
		inputCount(inputCount),
		outputCount(outputCount)
	{
		for (unsigned int i = 0; i < outputCount; i++) {
			Component** newComponents = new Component*[outputs[i]->inputCount + 1u];
			std::memcpy(newComponents, outputs[i]->inputs, outputs[i]->inputCount * sizeof(Component*));
			newComponents[outputs[i]->inputCount] = this;
			delete[] outputs[i]->inputs;
			outputs[i]->inputs = newComponents;
			outputs[i]->inputCount++;
		}

		for (unsigned int i = 0; i < inputCount; i++) {
			Component** newComponents = new Component*[inputs[i]->outputCount + 1u];
			std::memcpy(newComponents, inputs[i]->outputs, inputs[i]->outputCount * sizeof(Component*));
			newComponents[inputs[i]->outputCount] = this;
			delete[] inputs[i]->outputs;
			inputs[i]->outputs = newComponents;
			inputs[i]->outputCount++;
		}

		componentIndex = board->getNextComponentIndex();
	}
};
