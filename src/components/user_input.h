#pragma once
#include "component.h"
#include "output.h"
#include "input.h"
#include "events.h"
#include "board.h"
#include <mutex>
#include <thread>

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
		const auto prevState = this->board->getCurrentState();

		if (prevState == Board::Uninitialized)
			return;

		if (prevState == Board::Stopping)
		{
			while(prevState != Board::Stopped)
				std::this_thread::yield();
		}

		if (prevState == Board::Running)
			this->board->stop();

		if (inputEvent == InputEvent::Cont)
		{
			for (unsigned int i = 0; i < outputCount; i++) {
				this->outputs[i].setPowered(pendingData[i]);
			}
		}
		else if (inputEvent == InputEvent::Pulse)
		{
			memcpy(pendingData, state, outputCount * sizeof(bool));
			pending = true;

			if (!subscribed) {
				board->tickEvent += tickEvent;
				subscribed = true;
			}
		}

		if (prevState == Board::Running)
			this->board->resume();
	}
private:
	bool subscribed = false;
	bool* pendingData = new bool[outputCount];
	bool pending = false;

	Events::EventHandler<>* tickEvent = new Events::EventHandler<>([this](Events::Emitter* e, Events::EventArgs& a) {
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
	});
};
