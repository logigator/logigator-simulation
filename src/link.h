#pragma once
#include "board.h"

class Input;
class Output;
class Board;

class Link
{
public:
	Board* board;
	Component** inputs;
	unsigned int inputCount;
	Component** outputs;
	unsigned int outputCount;
	bool* poweredNext;
	bool* poweredCurrent;
	unsigned int linkIndex;

	Link() :
		board(nullptr),
		inputs(nullptr),
		inputCount(0),
		outputs(nullptr),
		outputCount(0),
		poweredNext(nullptr),
		poweredCurrent(nullptr),
		linkIndex(0)
	{
	}

	explicit Link(Board* board) :
		board(board),
		inputs(nullptr),
		inputCount(0),
		outputs(nullptr),
		outputCount(0),
		poweredNext(nullptr),
		poweredCurrent(nullptr)
	{
		linkIndex = board->getNextLinkIndex();
	}

	Link(Board* board, Component** inputs, Component** outputs, const unsigned int inputCount, const unsigned int outputCount) :
		board(board),
		inputs(inputs),
		inputCount(inputCount),
		outputs(outputs),
		outputCount(outputCount),
		poweredNext(nullptr),
		poweredCurrent(nullptr)
	{
		linkIndex = board->getNextLinkIndex();
	}

	~Link()
	{
		delete[] inputs;
		delete[] outputs;
	}

	void setPowered(const bool val) const
	{
		*poweredNext = val;
	}

	void update() const
	{
		if (*poweredNext != *poweredCurrent)
			board->writeBuffer[linkIndex] = true;
	}
};

