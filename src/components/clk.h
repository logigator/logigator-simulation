#pragma once
#include "component.h"
#include "board.h"
#include "events.h"
#include "link.h"

class CLK : 
	public Component
{
public:
	int speed = 1;

	CLK(Board* board, Link** inputs, Link** outputs) : Component(board, inputs, outputs, 1, 1) {
		board->tickEvent += tickEvent;
        subscribed = true;
	}

	CLK(Board* board, Link** inputs, Link** outputs, const int speed) : Component(board, inputs, outputs, 1, 1) {
		this->speed = speed;
		board->tickEvent += tickEvent;
        subscribed = true;
	}

	~CLK() {
		delete tickEvent;
	}

	void compute() override {
		outputChange();
	}
private:
	bool subscribed = false;
	int tickCount = 0;
	Events::EventHandler<>* tickEvent = new Events::EventHandler<>([this](Events::Emitter* e, Events::EventArgs& a) {
		this->tickCount++;

		if (this->tickCount >= this->speed) {
#pragma optimize( "", off )
			if (!*this->outputs[0]->poweredNext && !*this->outputs[0]->poweredCurrent)
				*this->outputs[0]->poweredNext = true;
#pragma optimize( "", on )

			this->tickCount = 0;
		}
	});
	
	void outputChange() {
		if (*inputs[0]->poweredCurrent) {
			if (subscribed) {
				board->tickEvent -= tickEvent;
				subscribed = false;
			}
		}
		else {
			if (!subscribed) {
				board->tickEvent += tickEvent;
				subscribed = true;
			}
		}
	}
};
