#pragma once
#include "component.h"
#include "events.h"
#include "board.h"

class UserInput :
	public Component
{
public:
	UserInput(Board* board, Link** outputs, const unsigned int outputCount) : Component(board, nullptr, outputs, 0, outputCount) { }

	enum InputEvent { Cont, Pulse, Max };
	
	void compute() override {

	}

	void triggerUserInput(bool* state, const InputEvent inputEvent) {
		for (unsigned int i = 0; i < outputCount; i++) {
			*this->outputs[i]->poweredNext = state[i];
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
		for (unsigned int i = 0; i < this->outputCount; i++) {
#pragma optimize( "", off )
			if (!*this->outputs[0]->poweredNext)
				*this->outputs[i]->poweredNext = true;
#pragma optimize( "", on )
		}
		this->board->tickEvent -= this->tickEvent;
		this->subscribed = false;
	});
};
