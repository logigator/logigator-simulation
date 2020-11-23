#include <emscripten/bind.h>
#include <string>
#include <ctime>

#include "board.h"
#include "component.h"
#include "link.h"

#include "and.h"
#include "clk.h"
#include "delay.h"
#include "not.h"
#include "or.h"
#include "user_input.h"
#include "xor.h"
#include "half_addr.h"
#include "full_addr.h"
#include "rom.h"
#include "d_ff.h"
#include "jk_ff.h"
#include "sr_ff.h"
#include "led_matrix.h"
#include "rng.h"

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
			str += std::string(" ") + std::to_string(board->getComponents()[i]->getInputs()[j].getPowered());
		}

		str += std::string("\n  Outputs:");
		for (unsigned int j = 0; j < board->getComponents()[i]->getOutputCount(); j++) {
			str += std::string(" ") + std::to_string(board->getComponents()[i]->getOutputs()[j].getPowered());
		}
	}
	printf("%s", (str + std::string("\n")).c_str());

	return 0;
}

int start(double ticks, unsigned long ms) {
	board->start((unsigned long long)ticks, ms, 1);
	return 0;
}

int stop() {
	board->stop();
	return 0;
}

int initBoard() {
	std::srand(std::time(nullptr));
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

int initComponents(const unsigned int count) {
	components = new Component*[count];
	componentCount = count;
	return 0;
}

int initComponent(const unsigned int index, const unsigned int type, const uintptr_t inputsPtr, const uintptr_t outputsPtr, const unsigned int inputCount, const unsigned int outputCount, const unsigned int opCount, const uintptr_t opsPtr) {
	auto* inputs = reinterpret_cast<uint32_t*>(inputsPtr);
	auto* outputs = reinterpret_cast<uint32_t*>(outputsPtr);
	auto* ops = reinterpret_cast<uint32_t*>(opsPtr);

	auto** componentInputs = new Link*[inputCount];

	for (unsigned int j = 0; j < inputCount; j++) {
		componentInputs[j] = &links[(unsigned int)inputs[j]];
	}

	auto** componentOutputs = new Link*[outputCount];

	for (unsigned int j = 0; j < outputCount; j++) {
		componentOutputs[j] = &links[(unsigned int)outputs[j]];
	}

	unsigned char* data;
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
			if (opCount > 0)
				components[index] = new CLK(board, componentInputs, componentOutputs, ops[0]);
			break;
		case 10:
			components[index] = new HalfAddr(board, componentInputs, componentOutputs);
			break;
		case 11:
			components[index] = new FullAddr(board, componentInputs, componentOutputs);
			break;
		case 12:
			data = new unsigned char[opCount];
			for (unsigned int i = 0; i < opCount; i++)
			{
				data[i] = static_cast<unsigned char>(ops[i]);
			}
			components[index] = new ROM(board, componentInputs, componentOutputs, inputCount, outputCount, opCount, data);
			delete[] data;
			break;
		case 13:
			components[index] = new D_FF(board, componentInputs, componentOutputs);
			break;
		case 14:
			components[index] = new JK_FF(board, componentInputs, componentOutputs);
			break;
		case 15:
			components[index] = new SR_FF(board, componentInputs, componentOutputs);
			break;
		case 16:
			components[index] = new RNG(board, componentInputs, componentOutputs, outputCount);
			break;
		case 204:
			if (opCount > 0)
				components[index] = new LEDMatrix(board, componentInputs, componentOutputs, ops[0] > 4 ? 8 : 4, outputCount);
			break;
		default:
			if (type >= 200 && type < 300) {
            	components[index] = new UserInput(board, componentOutputs, outputCount);
            	break;
            }
            return 1;
	}

	return 0;
}

int triggerInput(unsigned int index, int event, uintptr_t state) {
	if (index > board->componentCount) {
		return 1;
	}
	if (event < 0 || event >= UserInput::InputEvent::Max) {
    	return 2;
    }

	const auto inputEvent = static_cast<UserInput::InputEvent>(event);
	auto* userInput = (UserInput*)(board->getComponents()[index]);

	userInput->triggerUserInput(reinterpret_cast<bool*>(state), inputEvent);
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

	auto* states = new uint8_t[arraySize];

	uint_fast32_t stateIndex = 0;
	for (size_t i = 0; i < board->componentCount; i++) {
		Component* component = board->getComponents()[i];

		for (int j = 0; j < component->getInputCount(); j++) {
			states[stateIndex++] = (uint8_t)component->getInputs()[j].getPowered();
		}

		for (int j = 0; j < component->getOutputCount(); j++) {
			states[stateIndex++] = (uint8_t)component->getOutputs()[j].getPowered();
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
	emscripten::function("triggerInput", &triggerInput, emscripten::allow_raw_pointers());

	emscripten::function("start", &start);
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
