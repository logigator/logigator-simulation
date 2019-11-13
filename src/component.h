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
		componentIndex = board->getNextComponentIndex();
	}
};
