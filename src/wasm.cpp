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
Link** links = nullptr;

unsigned int componentCount = 0;
unsigned int linkCount = 0;

int test() {
	printf("%s", ("componentCount: " + std::to_string(board->componentCount) + "\n").c_str());
	printf("%s", ("LinkCount: " + std::to_string(board->linkCount) + "\n").c_str());
	printf("%s", ("Tick: " + std::to_string(board->getCurrentTick()) + "\n").c_str());

	std::string linksString = std::string("Links:");
	for (unsigned int i = 0; i < board->linkCount; i++) {
		linksString += std::string(" ") + std::to_string(board->getLinks()[i]->powered);
	}
	printf("%s", (linksString + std::string("\n")).c_str());

	std::string str = std::string("Components:");
	for (unsigned int i = 0; i < board->componentCount; i++) {
		str += std::string("\n [") + std::to_string(i) + std::string("]\n  Inputs: ");
		for (unsigned int j = 0; j < board->getComponents()[i]->getInputCount(); j++) {
			str += std::string(" ") + std::to_string(board->getComponents()[i]->inputs[j]->getPowered());
		}

		str += std::string("\n  Outputs:");
		for (unsigned int j = 0; j < board->getComponents()[i]->getOutputCount(); j++) {
			str += std::string(" ") + std::to_string(board->getComponents()[i]->outputs[j]->getPowered());
		}
	}
	printf("%s", (str + std::string("\n")).c_str());

	return 0;
}

int initBoard() {
	board->init(components, links, componentCount, linkCount);
	return 0;
}

int initLinks(unsigned int count) {
	links = new Link*[count] { 0 };
	
	for (unsigned int i = 0; i < count; i++) {
		links[i] = new Link(board);
	}

	linkCount = count;

	return 0;
}

int initComponents(unsigned int count) {
	components = new Component*[count] { 0 };
	componentCount = count;
	return 0;
}

int initComponent(unsigned int index, std::string typeStr, uintptr_t inputsPtr, uintptr_t outputsPtr, unsigned int inputCount, unsigned int outputCount) {
		const char* type = typeStr.c_str();
		unsigned int* inputs = reinterpret_cast<unsigned int*>(inputsPtr);
		unsigned int* outputs = reinterpret_cast<unsigned int*>(outputsPtr);

		Link** componentInputs = new Link*[inputCount];

		for (unsigned int j = 0; j < inputCount; j++) {
			componentInputs[j] = links[inputs[j]];
		}

		Link** componentOutputs = new Link*[outputCount];

		for (unsigned int j = 0; j < outputCount; j++) {
			componentOutputs[j] = links[outputs[j]];
		}

		if (!strcmp(type, "AND"))
			components[index] = new AND(board, componentInputs, componentOutputs);
		else if (!strcmp(type, "BUTTON"))
			components[index] = new BUTTON(board, componentInputs, componentOutputs);
		else if (!strcmp(type, "CLK"))
			components[index] = new CLK(board, componentInputs, componentOutputs, 1);
		else if (!strcmp(type, "DELAY"))
			components[index] = new DELAY(board, componentInputs, componentOutputs);
		else if (!strcmp(type, "NOT"))
			components[index] = new NOT(board, componentInputs, componentOutputs);
		else if (!strcmp(type, "OR"))
			components[index] = new OR(board, componentInputs, componentOutputs);
		else if (!strcmp(type, "SWITCH"))
			components[index] = new SWITCH(board, componentInputs, componentOutputs);
		else if (!strcmp(type, "XOR"))
			components[index] = new XOR(board, componentInputs, componentOutputs);
		else {
			return 1;
		}

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

unsigned long getCurrentSpeed() {
	return board->currentSpeed;
}

uintptr_t getLinks() {
	bool* links = new bool[linkCount];

	for (unsigned int i = 0; i < linkCount; i++) {
		links[i] = board->getLinks()[i]->powered;
	}

	return (uintptr_t)links;
}

EMSCRIPTEN_BINDINGS(module)
{
	emscripten::function("test", &test);
	emscripten::function("initBoard", &initBoard);
	emscripten::function("initLinks", &initLinks);
	emscripten::function("initComponents", &initComponents, emscripten::allow_raw_pointers());
	emscripten::function("initComponent", &initComponent, emscripten::allow_raw_pointers());
	emscripten::function("start", &start);
	emscripten::function("startTimeout", &startTimeout);
	emscripten::function("startManual", &startManual);
	emscripten::function("stop", &stop);
	emscripten::function("getCurrentSpeed", &getCurrentSpeed);
	emscripten::function("getLinks", &getLinks, emscripten::allow_raw_pointers());
}
