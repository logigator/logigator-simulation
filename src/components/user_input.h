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
		mutex.lock();
		memcpy(pendingData, state, outputCount * sizeof(bool));
		pendingInput = inputEvent;
		pending = true;

		if (!subscribed) {
			board->tickEvent += tickEvent;
			subscribed = true;
		}
		mutex.unlock();
	}
private:
	bool subscribed = false;
	bool* pendingData = new bool[outputCount];
	bool pending = false;
	InputEvent pendingInput = InputEvent::Max;
	std::mutex mutex;

	Events::EventHandler<>* tickEvent = new Events::EventHandler<>([this](Events::Emitter* e, Events::EventArgs& a) {
		mutex.lock();
		if (pendingInput == InputEvent::Cont)
		{
			for (unsigned int i = 0; i < outputCount; i++) {
				this->outputs[i].setPowered(pendingData[i]);
			}
			board->tickEvent -= tickEvent;
			pending = false;
			subscribed = false;
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
				subscribed = false;
			}
		}
		mutex.unlock();
	});
};
