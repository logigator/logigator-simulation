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
	Component** outputs;
	unsigned int inputCount;
	unsigned int outputCount;
	bool* poweredNext;
	bool* poweredCurrent;
	unsigned long long lastUpdateTick;

	Link() :
		board(nullptr),
		inputs(nullptr),
		outputs(nullptr),
		inputCount(0),
		outputCount(0),
		poweredNext(nullptr),
		poweredCurrent(nullptr),
		lastUpdateTick(0)
	{
	}

	explicit Link(Board* board) :
		board(board),
		inputs(nullptr),
		outputs(nullptr),
		inputCount(0),
		outputCount(0),
		poweredNext(nullptr),
		poweredCurrent(nullptr),
		lastUpdateTick(board->getCurrentTick())
	{
	}

	Link(Board* board, Component** inputs, Component** outputs, const unsigned int inputCount, const unsigned int outputCount) :
		board(board),
		inputs(inputs),
		outputs(outputs),
		inputCount(inputCount),
		outputCount(outputCount),
		poweredNext(nullptr),
		poweredCurrent(nullptr),
		lastUpdateTick(board->getCurrentTick())
	{
	}

	~Link()
	{
		delete[] inputs;
		delete[] outputs;
	}
};

