#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <map>
#include <nan.h>
#include "node.h"
#include "board.h"
#include "component.h"
#include "link.h"
#include "user_input_component.h"

#include "and.h"
#include "button.h"
#include "clk.h"
#include "delay.h"
#include "not.h"
#include "or.h"
#include "switch.h"
#include "xor.h"

std::map<std::string, Board*> boards;

void newBoard(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 2 || !args[0]->IsName() || !args[1]->IsObject()) {
		Nan::ThrowSyntaxError("Usage: newBoards([string]identifier, [object]data");
		return;
	}

	unsigned int threadCount = 1;
	unsigned int componentCount = 0;
	unsigned int linkCount = 0;
	Component** components;
	Link* links;

	std::string identifier(*Nan::Utf8String(args[0]));
	v8::Local<v8::Object> obj = args[1]->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

	if (boards.count(identifier)) {
		Nan::ThrowError("Identifier already used!");
		return;
	}

	if (!Nan::Get(obj, Nan::New("links").ToLocalChecked()).ToLocalChecked()->IsNumber()) {
		Nan::ThrowError("Invalid number of links!");
		return;
	}

	Board* board = new Board();

	if (Nan::Get(obj, Nan::New("threads").ToLocalChecked()).ToLocalChecked()->IsNumber())
		threadCount = Nan::Get(obj, Nan::New("threads").ToLocalChecked()).ToLocalChecked()->Int32Value(Nan::GetCurrentContext()).FromJust();

	linkCount = Nan::Get(obj, Nan::New("links").ToLocalChecked()).ToLocalChecked()->Int32Value(Nan::GetCurrentContext()).FromJust();
	links = (Link*)malloc(sizeof(Link) * linkCount);
	for (unsigned int i = 0; i < linkCount; i++) {
		new (&links[i]) Link(board);
	}

	if (Nan::Get(obj, Nan::New("components").ToLocalChecked()).ToLocalChecked()->IsArray()) {
		v8::Local<v8::Array> v8Components = v8::Local<v8::Array>::Cast(Nan::Get(obj, Nan::New("components").ToLocalChecked()).ToLocalChecked());

		componentCount = v8Components->Length();
		components = new Component*[componentCount] { 0 };
		for (unsigned int i = 0; i < componentCount; i++) {
			v8::Local<v8::Object> v8Component = Nan::Get(v8Components, i).ToLocalChecked()->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

			const char* componentType = *Nan::Utf8String(Nan::Get(v8Component, Nan::New("type").ToLocalChecked()).ToLocalChecked());
			v8::Local<v8::Array> v8ComponentInputs = v8::Local<v8::Array>::Cast(Nan::Get(v8Component, Nan::New("inputs").ToLocalChecked()).ToLocalChecked());
			Link** componentInputs = new Link*[v8ComponentInputs->Length()];
			for (unsigned int j = 0; j < v8ComponentInputs->Length(); j++)
				componentInputs[j] = &links[Nan::Get(v8ComponentInputs, j).ToLocalChecked()->Int32Value(Nan::GetCurrentContext()).FromJust()];

			v8::Local<v8::Array> v8ComponentOutputs = v8::Local<v8::Array>::Cast(Nan::Get(v8Component, Nan::New("outputs").ToLocalChecked()).ToLocalChecked());
			Link** componentOutputs = new Link*[v8ComponentOutputs->Length()];
			for (unsigned int j = 0; j < v8ComponentOutputs->Length(); j++)
				componentOutputs[j] = &links[Nan::Get(v8ComponentOutputs, j).ToLocalChecked()->Int32Value(Nan::GetCurrentContext()).FromJust()];
			
			if (!strcmp(componentType, "AND"))
				components[i] = new AND(board, componentInputs, componentOutputs);
			else if (!strcmp(componentType, "BUTTON"))
				components[i] = new BUTTON(board, componentInputs, componentOutputs);
			else if (!strcmp(componentType, "CLK"))
				components[i] = new CLK(board, componentInputs, componentOutputs, Nan::Get(v8Component, Nan::New("CLK_Speed").ToLocalChecked()).ToLocalChecked()->Int32Value(Nan::GetCurrentContext()).FromJust());
			else if (!strcmp(componentType, "DELAY"))
				components[i] = new DELAY(board, componentInputs, componentOutputs);
			else if (!strcmp(componentType, "NOT"))
				components[i] = new NOT(board, componentInputs, componentOutputs);
			else if (!strcmp(componentType, "OR"))
				components[i] = new OR(board, componentInputs, componentOutputs);
			else if (!strcmp(componentType, "SWITCH"))
				components[i] = new SWITCH(board, componentInputs, componentOutputs);
			else if (!strcmp(componentType, "XOR"))
				components[i] = new XOR(board, componentInputs, componentOutputs);
			else {
				Nan::ThrowTypeError((std::string("Error: Component '") + std::string(componentType) + std::string("' (") + std::to_string(i) + std::string(") is of no valid type!")).c_str());
				return;
			}
		}
	}
	else {
		components = new Component*[0];
	}

	boards[identifier] = board;
	board->init(components, links, componentCount, linkCount, (int)threadCount);
}

void removeBoard(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1 || !args[0]->IsName()) {
		Nan::ThrowSyntaxError("No identifier specified!");
		return;
	}
	std::string identifier(*Nan::Utf8String(args[0]));

	if (!boards.count(identifier)) {
		Nan::ThrowTypeError("Board not found!");
		return;
	}

	delete boards[identifier];
	boards.erase(identifier);
}

void start(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 1 || args.Length() > 2 || !args[0]->IsName()) {
		Nan::ThrowSyntaxError("No identifier specified!");
		return;
	}
	if (args.Length() > 1 && !args[1]->IsNumber()) {
		Nan::ThrowTypeError("2. argument must be a number!");
		return;
	}

	std::string identifier(*Nan::Utf8String(args[0]));

	if (!boards.count(identifier)) {
		Nan::ThrowTypeError("Board not found!");
		return;
	}

	if (args.Length() > 1)
		boards[identifier]->startManual(args[1]->Int32Value(Nan::GetCurrentContext()).FromJust());
	else
		boards[identifier]->start();
}

void stop(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1 || !args[0]->IsName()) {
		Nan::ThrowSyntaxError("No identifier specified!");
		return;
	}
	std::string identifier(*Nan::Utf8String(args[0]));

	if (!boards.count(identifier)) {
		Nan::ThrowTypeError("Board not found!");
		return;
	}

	boards[identifier]->stop();
}

void getStatus(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1 || !args[0]->IsName()) {
		Nan::ThrowSyntaxError("No identifier specified!");
		return;
	}
	std::string identifier(*Nan::Utf8String(args[0]));

	if (!boards.count(identifier)) {
		Nan::ThrowTypeError("Board not found!");
		return;
	}

	Board* board = boards[identifier];
	v8::Local<v8::Object> obj = Nan::New<v8::Object>();
	
	Nan::Set(obj, Nan::New("currentSpeed").ToLocalChecked(), Nan::New((double)board->currentSpeed));
	Nan::Set(obj, Nan::New("currentState").ToLocalChecked(), Nan::New(board->getCurrentState()));
	Nan::Set(obj, Nan::New("threadCount").ToLocalChecked(), Nan::New(board->getThreadCount()));
	Nan::Set(obj, Nan::New("componentCount").ToLocalChecked(), Nan::New((unsigned int)board->componentCount));
	Nan::Set(obj, Nan::New("linkCount").ToLocalChecked(), Nan::New((unsigned int)board->linkCount));
	Nan::Set(obj, Nan::New("tick").ToLocalChecked(), Nan::New((double)board->getCurrentTick()));

	args.GetReturnValue().Set(obj);
}

void getBoard(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1 || !args[0]->IsName()) {
		Nan::ThrowSyntaxError("No identifier specified!");
		return;
	}
	std::string identifier(*Nan::Utf8String(args[0]));

	if (!boards.count(identifier)) {
		Nan::ThrowTypeError("Board not found!");
		return;
	}

	Board* board = boards[identifier];
	v8::Local<v8::Array> v8Components = Nan::New<v8::Array>();

	for (unsigned int i = 0; i < board->componentCount; i++) {
		Component* component = board->getComponents()[i];
		v8::Local<v8::Array> v8Component = Nan::New<v8::Array>();

		for (int j = 0; j < component->getOutputCount(); j++)
			Nan::Set(v8Component, j, Nan::New(component->outputs[j].getPowered()));

		Nan::Set(v8Components, i, v8Component);
	}

	v8::Local<v8::Array> v8Links = Nan::New<v8::Array>();
	for (unsigned int i = 0; i < board->linkCount; i++) {
		Nan::Set(v8Links, i, Nan::New(board->getLinks()[i].powered));
	}

	v8::Local<v8::Object> v8Board = Nan::New<v8::Object>();
	Nan::Set(v8Board, Nan::New("components").ToLocalChecked(), v8Components);
	Nan::Set(v8Board, Nan::New("links").ToLocalChecked(), v8Links);
	args.GetReturnValue().Set(v8Board);
}

void triggerInput(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 4 || !args[0]->IsName() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
		Nan::ThrowSyntaxError("Usage: newBoards([String]identifier, [Number]componentIndex, [Number]inputIndex, [Number]inputEvent");
		return;
	}
	std::string identifier(*Nan::Utf8String(args[0]));

	if (!boards.count(identifier)) {
		Nan::ThrowTypeError("Board not found!");
		return;
	}
	Board* board = boards[identifier];

	unsigned int componentIndex = args[1]->Int32Value(Nan::GetCurrentContext()).FromJust();
	if (componentIndex > board->componentCount) {
		Nan::ThrowTypeError("Component not found!");
		return;
	}
	
	UserInputComponent* userInputComponent = (UserInputComponent*)(board->getComponents()[componentIndex]);
	if (userInputComponent == nullptr) {
		Nan::ThrowTypeError("Component is not an user input!");
		return;
	}

	int inputIndex = args[2]->Int32Value(Nan::GetCurrentContext()).FromJust();
	if (inputIndex < 0 || inputIndex > userInputComponent->getUserInputCount()) {
		Nan::ThrowTypeError("InputIndex is out of range!");
		return;
	}

	UserInputComponent::InputEvent inputEvent = static_cast<UserInputComponent::InputEvent>(args[3]->Int32Value(Nan::GetCurrentContext()).FromJust());
	if (inputEvent < 0 || inputEvent > 1) {
		Nan::ThrowTypeError("InputEvent invalid!");
		return;
	}

	userInputComponent->triggerUserInput(inputIndex, inputEvent);
}

void Initialize(v8::Local<v8::Object> exports) {
	Nan::Set(exports, Nan::New("newBoard").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(newBoard)).ToLocalChecked());
	Nan::Set(exports, Nan::New("removeBoard").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(removeBoard)).ToLocalChecked());
	Nan::Set(exports, Nan::New("startBoard").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(start)).ToLocalChecked());
	Nan::Set(exports, Nan::New("stopBoard").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(stop)).ToLocalChecked());
	Nan::Set(exports, Nan::New("getBoardStatus").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(getStatus)).ToLocalChecked());
	Nan::Set(exports, Nan::New("getBoard").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(getBoard)).ToLocalChecked());
	Nan::Set(exports, Nan::New("triggerInput").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(triggerInput)).ToLocalChecked());
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
