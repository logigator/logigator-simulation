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
	UserInput(Board* board, Link** outputs, unsigned int outputCount) : Component(board, nullptr, outputs, 0, outputCount) { }
	UserInput(Board* board, Output* outputs, unsigned int outputCount) : Component(board, nullptr, outputs, 0, outputCount) { }

	enum InputEvent { Cont, Pulse, Max };
	void compute() {

	}

	void triggerUserInput(bool* state, InputEvent inputEvent) {
		for (unsigned int i = 0; i < outputCount; i++) {
			this->outputs[i].setPowered(state[i]);
		}

		if (inputEvent == InputEvent::Pulse && !subscribed) {
			board->tickEvent += tickEvent;
			subscribed = true;
		}
		else if (inputEvent != InputEvent::Pulse && subscribed) {
			board->tickEvent -= tickEvent;
			subscribed = false;
		}
	}
private:
	bool subscribed = false;

	Events::EventHandler<>* tickEvent = new Events::EventHandler<>([this](Events::Emitter* e, Events::EventArgs& a) {
		for (unsigned int i = 0; i < outputCount; i++) {
			this->outputs[i].setPowered(false);
		}
		board->tickEvent -= tickEvent;
		subscribed = false;
	});
};
