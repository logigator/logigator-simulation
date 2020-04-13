#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "events.h"
#include "board.h"
#include <mutex>

class UserInput :
	public Component
{
public:
	UserInput(Board* board, Link** outputs, const size_t outputCount) : Component(board, nullptr, outputs, 0, outputCount) { }
	UserInput(Board* board, Output* outputs, const size_t outputCount) : Component(board, nullptr, outputs, 0, outputCount) { }

	~UserInput()
	{
		delete[] pendingData;
		delete tickEvent;
	}

	enum InputEvent { Cont, Pulse, Max };

	void compute() override { }

	void triggerUserInput(bool* state, const InputEvent inputEvent) {
		memcpy(pendingData, state, outputCount * sizeof(bool));
		pendingInput = inputEvent;
		pending = true;

		if (!subscribed || subscribed < this->board->getCurrentTick() - 1) {
			board->tickEvent += tickEvent;
			subscribed = this->board->getCurrentTick();
		}
	}
private:
	uint_fast64_t subscribed = 0;
	bool* pendingData = new bool[outputCount];
	bool pending = false;
	InputEvent pendingInput = InputEvent::Max;

	Events::EventHandler<>* tickEvent = new Events::EventHandler<>([this](Events::Emitter* e, Events::EventArgs& a) {
		if (pendingInput == InputEvent::Cont && pending)
		{
			for (unsigned int i = 0; i < outputCount; i++) {
				this->outputs[i].setPowered(pendingData[i]);
			}
			board->tickEvent -= tickEvent;
			pending = false;
			subscribed = 0;
		}
		else if (pendingInput == InputEvent::Pulse)
		{
			if (pending)
			{
				for (unsigned int i = 0; i < outputCount; i++) {
					this->outputs[i].setPowered(pendingData[i]);
				}
				pending = false;
			}
			else
			{
				for (unsigned int i = 0; i < outputCount; i++) {
					this->outputs[i].setPowered(false);
				}
				board->tickEvent -= tickEvent;
				subscribed = 0;
			}
		}
	});
};
