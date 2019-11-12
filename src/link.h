#pragma once

class Input;
class Output;
class Board;

class Link
{
public:
	Board* board;
	Input** inputs;
	Output** outputs;
	unsigned int inputCount;
	unsigned int outputCount;
	bool* powered;

	Link() :
		board(nullptr),
		inputs(nullptr),
		outputs(nullptr),
		inputCount(0),
		outputCount(0),
		powered(nullptr)
	{
	}

	explicit Link(Board* board) :
		board(board),
		inputs(nullptr),
		outputs(nullptr),
		inputCount(0),
		outputCount(0),
		powered(nullptr)
	{
	}

	Link(Board* board, Input** inputs, Output** outputs, const unsigned int inputCount, const unsigned int outputCount) :
		board(board),
		inputs(inputs),
		outputs(outputs),
		inputCount(inputCount),
		outputCount(outputCount),
		powered(nullptr)
	{
	}

	~Link()
	{
		delete[] inputs;
		delete[] outputs;
	}
};

