#pragma once

class Input;
class Output;
class Board;

class Link
{
public:
	Link();
	Link(Board* board);
	Link(Board* board, Input** inputs, Output** outputs, int inputCount, int outputCount);
	~Link();
	bool* powered;
	Input** inputs;
	Output** outputs;
	Board* board;
	unsigned int inputCount;
	unsigned int outputCount;
};

