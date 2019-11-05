#pragma once
#include "component.h"

class UserInputComponent :
	public Component
{
public:
	enum InputEvent { Down, Up };
	virtual void triggerUserInput(int index, InputEvent inputEvent) = 0;
	unsigned int getUserInputCount() {
		return userInputCount;
	}
protected:
	UserInputComponent(Board* board, Link** inputs, Link** outputs, unsigned int inputCount, unsigned int outputCount, unsigned int userInputCount) : Component(board, inputs, outputs, inputCount, outputCount) {
		this->userInputCount = userInputCount;
	}
	UserInputComponent(Board* board, Input* inputs, Output* outputs, unsigned int inputCount, unsigned int outputCount, unsigned int userInputCount) : Component(board, inputs, outputs, inputCount, outputCount) {
		this->userInputCount = userInputCount;
	}
	unsigned int userInputCount;
};
