#pragma once
#include <cmath>
#include "component.h"
#include "output.h"
#include "input.h"
#include "link.h"

class LEDMatrix :
	public Component
{
public:
	LEDMatrix(Board* board, Input* inputs, Output* outputs, const size_t dataBusLength, const size_t ledCount)
		: Component(board, inputs, outputs, static_cast<size_t>(ceil(log2(ledCount / dataBusLength))) + dataBusLength + 1, ledCount)
	{
		this->dataBusLength = dataBusLength;
		this->addrBusLength = static_cast<size_t>(ceil(log2(ledCount / dataBusLength)));
	}
	
	LEDMatrix(Board* board, Link** inputs, Link** outputs, const size_t dataBusLength, const size_t ledCount)
		: Component(board, inputs, outputs, static_cast<size_t>(ceil(log2(ledCount / dataBusLength))) + dataBusLength + 1, ledCount)
	{
		this->dataBusLength = dataBusLength;
		this->addrBusLength = static_cast<size_t>(ceil(log2(ledCount / dataBusLength)));
	}

	void init() override
	{
		
	}
	
	void compute() override {
		if (inputs[inputCount - 1].getPowered() && !prevClk)
		{
			size_t position = 0;
			for (size_t i = 0; i < addrBusLength; i++)
			{
				position |= static_cast<size_t>(inputs[i].getPowered()) << i;
			}
			position *= dataBusLength;
			for (size_t i = 0; i < dataBusLength; i++)
			{
				outputs[position + i].setPowered(inputs[addrBusLength + i].getPowered());
			}
		}
		prevClk = inputs[inputCount - 1].getPowered();
	}
private:
	bool prevClk = false;
	size_t dataBusLength;
	size_t addrBusLength;
};

