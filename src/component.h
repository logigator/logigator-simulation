#pragma once

class Input;
class Output;
class Link;
class Board;

class Component
{
public:
	virtual ~Component();
	virtual void compute() = 0;
	unsigned int componentIndex;
	Input* inputs;
	Output* outputs;
	unsigned int getInputCount();
	unsigned int getOutputCount();
	
protected:
	Board* board;
	Component(Board* board, Link** inputs, Link** outputs, int inputCount, int outputCount);
	Component(Board* board, Input* inputs, Output* outputs, int inputCount, int outputCount);
	unsigned int inputCount;
	unsigned int outputCount;
};
