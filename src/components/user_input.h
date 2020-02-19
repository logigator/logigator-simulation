#pragma once

#include "component.h"
#include "output.h"
#include "input.h"
#include "events.h"
#include "board.h"

class UserInput :
	public Component
{
public:
	UserInput(Board* board, Link** outputs, const unsigned int outputCount) : Component(board, nullptr, outputs, 0, outputCount) { }
	UserInput(Board* board, Output* outputs, const unsigned int outputCount) : Component(board, nullptr, outputs, 0, outputCount) { }

	enum InputEvent { Cont, Pulse, Max };
	
	void compute() override { }

	void triggerUserInput(bool* state, const InputEvent inputEvent) {
		delete[] pending;
		pending = new bool[outputCount];
		memcpy(pending, state, outputCount * sizeof(bool));
		pendingInput = inputEvent;
		
		if (!subscribed) {
			board->tickEvent += tickEvent;
			subscribed = true;
		}
	}
private:
	bool subscribed = false;
	bool* pending = nullptr;
	InputEvent pendingInput = InputEvent::Max;

	Events::EventHandler<>* tickEvent = new Events::EventHandler<>([this](Events::Emitter* e, Events::EventArgs& a) {
		if (pendingInput == InputEvent::Cont)
		{
			for (unsigned int i = 0; i < outputCount; i++) {
				this->outputs[i].setPowered(pending[i]);
			}
			delete[] pending;
			pending = nullptr;
			board->tickEvent -= tickEvent;
			subscribed = false;
		}
		else if (pendingInput == InputEvent::Pulse)
		{
			if (pending)
			{
				for (unsigned int i = 0; i < outputCount; i++) {
					this->outputs[i].setPowered(pending[i]);
				}
				delete[] pending;
				pending = nullptr;
			}
			else
			{
				for (unsigned int i = 0; i < outputCount; i++) {
					this->outputs[i].setPowered(false);
				}
				board->tickEvent -= tickEvent;
				subscribed = false;
			}
		}
	});
};
