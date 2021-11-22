#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <ctime>
#include <nan.h>
#include "node.h"
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
#include "ram.h"
#include "dec.h"
#include "mux.h"

Board* board = new Board();
Component** components = nullptr;
Link* links = nullptr;

void init(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 1 || !args[0]->IsObject()) {
		Nan::ThrowSyntaxError("Usage: initBoard([object]board)");
		return;
	}

	if (components != nullptr || links != nullptr)
	{
		Nan::ThrowError("Board is already initialized. Destroy the old one first.");
		return;
	}

	const auto obj = args[0]->ToObject(Nan::GetCurrentContext()).ToLocalChecked();
	if (!Nan::Get(obj, Nan::New("links").ToLocalChecked()).ToLocalChecked()->IsNumber()) {
		Nan::ThrowError("Invalid number of links.");
		return;
	}

	std::srand(std::time(nullptr));

	const uint_fast32_t linkCount = Nan::Get(obj, Nan::New("links").ToLocalChecked()).ToLocalChecked()->Uint32Value(Nan::GetCurrentContext()).FromJust();;
	links = new Link[linkCount];
	for (uint_fast32_t i = 0; i < linkCount; i++) {
		new (&links[i]) Link(board);
	}
	uint_fast32_t componentCount = 0;

	if (Nan::Get(obj, Nan::New("components").ToLocalChecked()).ToLocalChecked()->IsArray()) {
		const auto v8Components = v8::Local<v8::Array>::Cast(Nan::Get(obj, Nan::New("components").ToLocalChecked()).ToLocalChecked());

		componentCount = v8Components->Length();
		components = new Component*[componentCount] { nullptr };
		for (uint_fast32_t i = 0; i < componentCount; i++) {
			const auto v8Component = Nan::Get(v8Components, i).ToLocalChecked()->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

			const auto componentType = Nan::To<uint32_t>(Nan::Get(v8Component, Nan::New("type").ToLocalChecked()).ToLocalChecked()).FromJust();
			const auto v8ComponentInputs = v8::Local<v8::Array>::Cast(Nan::Get(v8Component, Nan::New("inputs").ToLocalChecked()).ToLocalChecked());
			auto** componentInputs = new Link*[v8ComponentInputs->Length()];
			for (uint_fast32_t j = 0; j < v8ComponentInputs->Length(); j++)
				componentInputs[j] = &links[Nan::To<uint32_t>(Nan::Get(v8ComponentInputs, j).ToLocalChecked()).FromJust()];

			const auto v8ComponentOutputs = v8::Local<v8::Array>::Cast(Nan::Get(v8Component, Nan::New("outputs").ToLocalChecked()).ToLocalChecked());
			auto** componentOutputs = new Link*[v8ComponentOutputs->Length()];
			for (uint_fast32_t j = 0; j < v8ComponentOutputs->Length(); j++)
				componentOutputs[j] = &links[Nan::To<uint32_t>(Nan::Get(v8ComponentOutputs, j).ToLocalChecked()).FromJust()];

			v8::Local<v8::Array> ops = v8::Local<v8::Array>();
			if (Nan::Get(v8Component, Nan::New("ops").ToLocalChecked()).ToLocalChecked()->IsArray())
			{
				ops = v8::Local<v8::Array>::Cast(Nan::Get(v8Component, Nan::New("ops").ToLocalChecked()).ToLocalChecked());
			}

			unsigned char* data;
			switch (componentType)
			{
				case 1:
					components[i] = new NOT(board, componentInputs, componentOutputs);
					break;
				case 2:
					components[i] = new AND(board, componentInputs, componentOutputs, v8ComponentInputs->Length());
					break;
				case 3:
					components[i] = new OR(board, componentInputs, componentOutputs, v8ComponentInputs->Length());
					break;
				case 4:
					components[i] = new XOR(board, componentInputs, componentOutputs, v8ComponentInputs->Length());
					break;
				case 5:
					components[i] = new DELAY(board, componentInputs, componentOutputs);
					break;
				case 6:
					if (ops->Length() > 0)
						components[i] = new CLK(board, componentInputs, componentOutputs, Nan::To<int32_t>(Nan::Get(ops, 0).ToLocalChecked()).FromJust());
					break;
				case 10:
					components[i] = new HalfAddr(board, componentInputs, componentOutputs);
					break;
				case 11:
					components[i] = new FullAddr(board, componentInputs, componentOutputs);
					break;
				case 12:
					data = new unsigned char[ops->Length()];
					for (size_t j = 0; j < ops->Length(); j++)
					{
						data[j] = static_cast<unsigned char>(Nan::To<uint32_t>(Nan::Get(ops, j).ToLocalChecked()).FromJust());
					}
					components[i] = new ROM(board, componentInputs, componentOutputs, v8ComponentInputs->Length(), v8ComponentOutputs->Length(), ops->Length(), data);
					delete[] data;
					break;
				case 13:
					components[i] = new D_FF(board, componentInputs, componentOutputs);
					break;
				case 14:
					components[i] = new JK_FF(board, componentInputs, componentOutputs);
					break;
				case 15:
					components[i] = new SR_FF(board, componentInputs, componentOutputs);
					break;
				case 16:
					components[i] = new RNG(board, componentInputs, componentOutputs, v8ComponentOutputs->Length());
					break;
				case 17:
					if (v8ComponentInputs->Length() - 2 >= v8ComponentOutputs->Length())
						components[i] = new RAM(board, componentInputs, componentOutputs, v8ComponentInputs->Length() - v8ComponentOutputs->Length() - 2, v8ComponentOutputs->Length());
					break;
				case 18:
					if (v8ComponentInputs->Length() > 0)
						components[i] = new DEC(board, componentInputs, componentOutputs, v8ComponentInputs->Length());
					break;
                case 20:
                    if (v8ComponentInputs->Length() > 0)
                        components[i] = new MUX(board, componentInputs, componentOutputs, v8ComponentInputs->Length(), Nan::To<int32_t>(Nan::Get(ops, 0).ToLocalChecked()).FromJust());
                    break;
				case 204:
					if (ops->Length() > 0)
						components[i] = new LEDMatrix(board, componentInputs, componentOutputs, Nan::To<int32_t>(Nan::Get(ops, 0).ToLocalChecked()).FromJust() > 4 ? 8 : 4, v8ComponentOutputs->Length());
					break;
				default:
					if (componentType >= 200 && componentType < 300) {
						components[i] = new UserInput(board, componentOutputs, v8ComponentOutputs->Length());
						break;
					}
					Nan::ThrowTypeError((std::string("Error: Component '") + std::to_string(componentType) + std::string("' (") + std::to_string(i) + std::string(") is of no valid type!")).c_str());
					return;
			}
		}
	}
	else {
		components = new Component*[0];
	}

	board->init(components, links, componentCount, linkCount);
}

void destroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	delete board;

	components = nullptr;
	links = nullptr;

	board = new Board();
}

void start(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() > 4) {
		Nan::ThrowSyntaxError("Usage: startBoard({[number]threads, [number]ticks, [number]ms, [bool]synchronized})");
		return;
	}
	if (components == nullptr || links == nullptr)
	{
		Nan::ThrowError("Board is not initialized yet.");
		return;
	}

	auto threadCount = 1;
	auto ticks = UINT64_MAX;
	auto timeout = UINT64_MAX;

	if (args.Length() > 0) {
		if (!args[0]->IsNumber() || Nan::To<uint32_t>(args[0]).FromJust() <= 0) {
			Nan::ThrowTypeError("Thread count is invalid.");
			return;
		}
		threadCount = Nan::To<uint32_t>(args[0]).FromJust();
	}
	if (args.Length() > 1) {
		if (!args[1]->IsNumber()) {
			Nan::ThrowTypeError("Tick count is invalid.");
			return;
		}
		ticks = Nan::To<int64_t>(args[1]).FromJust();
	}
	if (args.Length() > 2) {
		if (!args[2]->IsNumber()) {
			Nan::ThrowTypeError("Timeout is invalid.");
			return;
		}
		timeout = Nan::To<int64_t>(args[2]).FromJust();
	}

	if (args.Length() > 3 && args[3]->IsBoolean() && Nan::To<bool>(args[3]).FromJust())
	{
		board->start(ticks, timeout * static_cast<int64_t>(10e5), threadCount, true);
	}
	else
	{
		board->start(ticks, timeout * static_cast<int64_t>(10e5), threadCount);
	}
}

void stop(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (components == nullptr || links == nullptr)
	{
		Nan::ThrowError("Board is not initialized yet.");
		return;
	}

	board->stop();
}

void getStatus(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (components == nullptr || links == nullptr)
	{
		Nan::ThrowError("Board is not initialized yet.");
		return;
	}

	const auto obj = Nan::New<v8::Object>();

	Nan::Set(obj, Nan::New("currentSpeed").ToLocalChecked(), Nan::New(static_cast<double>(board->currentSpeed)));
	Nan::Set(obj, Nan::New("currentState").ToLocalChecked(), Nan::New(board->getCurrentState()));
	Nan::Set(obj, Nan::New("threadCount").ToLocalChecked(), Nan::New(static_cast<uint32_t>(board->getThreadCount())));
	Nan::Set(obj, Nan::New("componentCount").ToLocalChecked(), Nan::New(static_cast<uint32_t>(board->componentCount)));
	Nan::Set(obj, Nan::New("linkCount").ToLocalChecked(), Nan::New(static_cast<uint32_t>(board->linkCount)));
	Nan::Set(obj, Nan::New("tick").ToLocalChecked(), Nan::New(static_cast<double>(board->getCurrentTick())));

	args.GetReturnValue().Set(obj);
}

void getBoard(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (components == nullptr || links == nullptr)
	{
		Nan::ThrowError("Board is not initialized yet.");
		return;
	}

	const auto v8Components = Nan::New<v8::Array>();

	for (size_t i = 0; i < board->componentCount; i++) {
		auto* component = board->getComponents()[i];
		const auto v8Component = Nan::New<v8::Array>();

		for (size_t j = 0; j < component->getOutputCount(); j++)
			Nan::Set(v8Component, j, Nan::New(component->getOutputs()[j].getPowered()));

		Nan::Set(v8Components, i, v8Component);
	}

	const auto v8Links = Nan::New<v8::Array>();
	for (size_t i = 0; i < board->linkCount; i++) {
		Nan::Set(v8Links, i, Nan::New(board->linkStates[i]));
	}

	const auto v8Board = Nan::New<v8::Object>();
	Nan::Set(v8Board, Nan::New("components").ToLocalChecked(), v8Components);
	Nan::Set(v8Board, Nan::New("links").ToLocalChecked(), v8Links);
	args.GetReturnValue().Set(v8Board);
}

void getLinks(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (components == nullptr || links == nullptr)
	{
		Nan::ThrowError("Board is not initialized yet.");
		return;
	}

	const auto v8Links = Nan::New<v8::Array>(board->linkCount);
	for (size_t i = 0; i < board->linkCount; i++) {
		Nan::Set(v8Links, i, Nan::New(board->linkStates[i]));
	}
	args.GetReturnValue().Set(v8Links);
}

void triggerInput(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() != 3 || !args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsArray()) {
		Nan::ThrowSyntaxError("Usage: newBoards([String]componentIndex, [Number]event, [Array(bool)]outputs)");
		return;
	}

	const uint_fast32_t componentIndex = Nan::To<uint32_t>(args[0]).FromJust();
	if (componentIndex > board->componentCount) {
		Nan::ThrowTypeError("Component not found!");
		return;
	}

	const auto inputEvent = static_cast<UserInput::InputEvent>(Nan::To<int32_t>(args[1]).FromJust());
	if (inputEvent < 0 || inputEvent >= UserInput::InputEvent::Max) {
		Nan::ThrowTypeError("Unrecognized input event.");
		return;
	}

	auto* userInput = reinterpret_cast<UserInput*>(board->getComponents()[componentIndex]);
	const auto stateArray = v8::Local<v8::Array>::Cast(args[2]);

	if (userInput->getOutputCount() <= 0)
		return;

	auto* state = new bool[userInput->getOutputCount()] { false };

	for (uint_fast32_t i = 0; i < stateArray->Length(); i++) {
		state[i] = Nan::To<bool>(Nan::Get(stateArray, i).ToLocalChecked()).FromMaybe(false);
	}

	userInput->triggerUserInput(state, inputEvent);
	delete[] state;
}

NAN_MODULE_INIT(Init) {
	Nan::Set(target, Nan::New("init").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(init)).ToLocalChecked());
	Nan::Set(target, Nan::New("destroy").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(destroy)).ToLocalChecked());
	Nan::Set(target, Nan::New("start").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(start)).ToLocalChecked());
	Nan::Set(target, Nan::New("stop").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(stop)).ToLocalChecked());
	Nan::Set(target, Nan::New("getStatus").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(getStatus)).ToLocalChecked());
	Nan::Set(target, Nan::New("getBoard").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(getBoard)).ToLocalChecked());
	Nan::Set(target, Nan::New("getLinks").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(getLinks)).ToLocalChecked());
	Nan::Set(target, Nan::New("triggerInput").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(triggerInput)).ToLocalChecked());
}

#if NODE_MAJOR_VERSION >= 10
NAN_MODULE_WORKER_ENABLED(keytar, Init)
#else
NODE_MODULE(keytar, Init)
#endif
