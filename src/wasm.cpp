#include <emscripten/bind.h>
#include <string>

#include "board.h"
#include "component.h"
#include "link.h"

#include "and.h"
#include "button.h"
#include "clk.h"
#include "delay.h"
#include "not.h"
#include "or.h"
#include "switch.h"
#include "xor.h"

Board* board = new Board();
Component** components = nullptr;
Link* links = nullptr;

unsigned int componentCount = 0;
unsigned int linkCount = 0;

struct BoardStatus {
	double tick;
	unsigned int speed;
	unsigned char state;
	unsigned int componentCount;
	unsigned int linkCount;
};

int test() {
	printf("%s", ("componentCount: " + std::to_string(board->componentCount) + "\n").c_str());
	printf("%s", ("LinkCount: " + std::to_string(board->linkCount) + "\n").c_str());
	printf("%s", ("Tick: " + std::to_string(board->getCurrentTick()) + "\n").c_str());

	std::string linksString = std::string("Links:");
	for (unsigned int i = 0; i < board->linkCount; i++) {
		linksString += std::string(" ") + std::to_string(*board->getLinks()[i].powered);
	}
	printf("%s", (linksString + std::string("\n")).c_str());

	std::string str = std::string("Components:");
	for (unsigned int i = 0; i < board->componentCount; i++) {
		str += std::string("\n [") + std::to_string(i) + std::string("]\n  Inputs: ");
		for (unsigned int j = 0; j < board->getComponents()[i]->getInputCount(); j++) {
			str += std::string(" ") + std::to_string(board->getComponents()[i]->inputs[j].getPowered());
		}

		str += std::string("\n  Outputs:");
		for (unsigned int j = 0; j < board->getComponents()[i]->getOutputCount(); j++) {
			str += std::string(" ") + std::to_string(board->getComponents()[i]->outputs[j].getPowered());
		}
	}
	printf("%s", (str + std::string("\n")).c_str());

	return 0;
}

int start() {
	board->start();
	return 0;
}

int startManual(unsigned long ticks) {
	board->startManual(ticks);
	return 0;
}

int startTimeout(unsigned int ms) {
	board->startTimeout(ms);
	return 0;
}

int stop() {
	board->stop();
	return 0;
}

int initBoard() {
	board->init(components, links, componentCount, linkCount);
	return 0;
}

int initLinks(unsigned int count) {
	links = new Link[count];

	for (unsigned int i = 0; i < count; i++) {
		new(&links[i]) Link(board);
	}

	linkCount = count;
	return 0;
}

int initComponents(unsigned int count) {
	components = new Component*[count];
	componentCount = count;
	return 0;
}

int initComponent(unsigned int index, unsigned int type, uintptr_t inputsPtr, uintptr_t outputsPtr, unsigned int inputCount, unsigned int outputCount, int op1, int op2) {
	uint32_t* inputs = reinterpret_cast<uint32_t*>(inputsPtr);
	uint32_t* outputs = reinterpret_cast<uint32_t*>(outputsPtr);

	Link** componentInputs = new Link*[inputCount];

	for (unsigned int j = 0; j < inputCount; j++) {
		componentInputs[j] = &links[(unsigned int)inputs[j]];
	}

	Link** componentOutputs = new Link*[outputCount];

	for (unsigned int j = 0; j < outputCount; j++) {
		componentOutputs[j] = &links[(unsigned int)outputs[j]];
	}

	switch (type)
	{
		case 1:
			components[index] = new NOT(board, componentInputs, componentOutputs);
			break;
		case 2:
			components[index] = new AND(board, componentInputs, componentOutputs, inputCount);
			break;
		case 3:
			components[index] = new OR(board, componentInputs, componentOutputs, inputCount);
			break;
		case 4:
			components[index] = new XOR(board, componentInputs, componentOutputs, inputCount);
			break;
		case 5:
			components[index] = new DELAY(board, componentInputs, componentOutputs);
			break;
		case 6:
			components[index] = new CLK(board, componentInputs, componentOutputs, op1);
			break;
		case 20:
			components[index] = new BUTTON(board, componentInputs, componentOutputs);
			break;
		case 21:
			components[index] = new SWITCH(board, componentInputs, componentOutputs);
			break;
		default:
			return 1;
	}

	return 0;
}

BoardStatus getStatus() {
	return BoardStatus {
		(double) board->getCurrentTick(),
		(unsigned int) board->currentSpeed,
		(unsigned char) board->getCurrentState(),
		(unsigned int) board->componentCount,
		(unsigned int) board->linkCount
	};
}

uintptr_t getLinks() {
	return (uintptr_t)board->linkStates;
}

uintptr_t getComponents() {
	uint_fast32_t arraySize = 0;

	for (size_t i = 0; i < board->componentCount; i++) {
		arraySize += board->getComponents()[i]->getInputCount();
		arraySize += board->getComponents()[i]->getOutputCount();
	}

	uint8_t* states = new uint8_t[arraySize];

	uint_fast32_t stateIndex = 0;
	for (size_t i = 0; i < board->componentCount; i++) {
		Component* component = board->getComponents()[i];
		
		for (int j = 0; j < component->getInputCount(); j++) {
			states[stateIndex++] = (uint8_t)component->inputs[j].getPowered();
		}

		for (int j = 0; j < component->getOutputCount(); j++) {
			states[stateIndex++] = (uint8_t)component->outputs[j].getPowered();
		}
	}

	return (uintptr_t)states;
}

int destroy() {
	delete board;

    components = nullptr;
    links = nullptr;
	componentCount = 0;
	linkCount = 0;

	board = new Board();

	return 0;
}

EMSCRIPTEN_BINDINGS(module)
{
	emscripten::function("test", &test);
	emscripten::function("initBoard", &initBoard);
	emscripten::function("initLinks", &initLinks);
	emscripten::function("initComponents", &initComponents);
	emscripten::function("initComponent", &initComponent, emscripten::allow_raw_pointers());

	emscripten::function("start", &start);
	emscripten::function("startTimeout", &startTimeout);
	emscripten::function("startManual", &startManual);
	emscripten::function("stop", &stop);

	emscripten::function("getStatus", &getStatus);
	emscripten::function("getLinks", &getLinks, emscripten::allow_raw_pointers());
	emscripten::function("getComponents", &getComponents, emscripten::allow_raw_pointers());

	emscripten::function("destroy", &destroy);

	emscripten::value_object<BoardStatus>("BoardStatus")
		.field("tick", &BoardStatus::tick)
		.field("speed", &BoardStatus::speed)
		.field("state", &BoardStatus::state)
		.field("componentCount", &BoardStatus::componentCount)
		.field("linkCount", &BoardStatus::linkCount);
}
