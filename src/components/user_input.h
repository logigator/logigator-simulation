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

	void triggerUserInput(const bool* state, const InputEvent inputEvent) {
		for (unsigned int i = 0; i < outputCount; i++) {
			this->board->linkDefaults[this->componentIndex] = state[i];
			this->outputs[i]->setPowered(state[i]);
			this->outputs[i]->update();
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

#ifndef __EMSCRIPTEN__
#pragma optimize( "", off )
#endif
	Events::EventHandler<>* tickEvent = new Events::EventHandler<>([this](Events::Emitter* e, Events::EventArgs& a) {
		for (unsigned int i = 0; i < this->outputCount; i++) {
			this->board->linkDefaults[this->componentIndex] = false;
			this->outputs[i]->update();
		}
		this->board->tickEvent -= this->tickEvent;
		this->subscribed = false;
	});
#pragma optimize( "", on )
};
