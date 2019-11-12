#pragma once

class Input;
class Output;
class Board;

class Link
{
public:
	bool* powered;
	Input** inputs;
	Output** outputs;
	Board* board;
	unsigned int inputCount;
	unsigned int outputCount;

	Link()
	{
	}

	Link(Board* board) :
		board(board),
		inputCount(0),
		outputCount(0),
		inputs(nullptr),
		outputs(nullptr),
		powered(nullptr)
	{
	}

	Link(Board* board, Input** inputs, Output** outputs, int inputCount, int outputCount) :
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

