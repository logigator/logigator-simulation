#include <emscripten/bind.h>
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
	Board* board = new Board();
	Link** links = new Link*[3]
	{
		new Link(board),
		new Link(board),
		new Link(board)
	};
	Component** components = new Component*[2]
	{
		new NOT(board, new Link*[1] { links[0] }, new Link*[1] { links[1] }),
		new AND(board, new Link*[2] { links[0], links[1] }, new Link*[1] { links[2] })
	};
	board->init(components, links, 2, 3, 1);
	//board->start(100);

	return 1;
}

void initBoard() {
	board->init(components, links, componentCount, linkCount);
}

void initLinks(unsigned int count) {
	links = new Link*[count] { 0 };
	
	for (unsigned int i = 0; i < count; i++) {
		links[i] = new Link(board);
	}

	linkCount = count;
}

void initComponents(unsigned int count) {
	components = new Component*[count] { 0 };
	componentCount = count;
}

int initComponent(unsigned int index, char* type, unsigned int* inputs, unsigned int* outputs, unsigned int inputCount, unsigned int outputCount) {
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

EMSCRIPTEN_BINDINGS(module)
{
	emscripten::function("test", &test);
	emscripten::function("initBoard", &initBoard);
	emscripten::function("initLinks", &initLinks);
	emscripten::function("initComponents", &initComponents, emscripten::allow_raw_pointers());
	emscripten::function("initComponent", &initComponent, emscripten::allow_raw_pointers());
}
