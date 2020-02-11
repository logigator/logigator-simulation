#include <thread>
#include <chrono>
#include <algorithm>
#include "board.h"
#include "component.h"
#include "link.h"
#include "spinlock_barrier.h"
#include "events.h"
#include "output.h"

Board::Board() = default;

Board::~Board()
{
	stop();

	for (unsigned int i = 0; i < componentCount; i++) {
		delete components[i];
	}

	if (componentCount > 0)
		delete[] components;

	if (linkCount > 0)
		delete[] links;

    #ifndef __EMSCRIPTEN__

	for (unsigned int i = 0; i < threadCount; i++) {
		delete threads[i];
	}
	delete[] threads;
	delete barrier;

	#endif

	delete[] linkStates;
	delete[] readBuffer;
	delete[] writeBuffer;
}

#ifdef __EMSCRIPTEN__

void Board::init(Component** components, Link* links, const unsigned int componentCount, const unsigned int linkCount)
{
	if (currentState != Board::Uninitialized)
		return;

	this->components = components;
	this->links = links;
	this->componentCount = componentCount;
	this->linkCount = linkCount;

	if (linkCount > 0)
		this->linkStates = new bool[linkCount] { false };
	else
		this->linkStates = new bool[0];
	    
	for (unsigned int i = 0; i < linkCount; i++) {
		links[i].powered = &this->linkStates[i];
	}

	if (componentCount > 0) {
		buffer1 = new bool[componentCount] { false };
		buffer2 = new bool[componentCount] { false };
		buffer3 = new bool[componentCount] { false };
		std::fill_n(buffer1, componentCount, 1);
	}
	else {
		buffer1 = new bool[0];
		buffer2 = new bool[0];
		buffer3 = new bool[0];
	}

	readBuffer = Board::buffer1;
	writeBuffer = Board::buffer2;
	wipeBuffer = Board::buffer3;

	currentState = Board::Stopped;

	lastCapture = std::chrono::high_resolution_clock::now();
}

#else

void Board::init(Component** components, Link* links, const unsigned int componentCount, const unsigned int linkCount)
{
	if (currentState != Board::Uninitialized)
		return;

	this->components = components;
	this->links = links;
	this->componentCount = componentCount;
	this->linkCount = linkCount;

	if (linkCount > 0)
		this->linkStates = new bool[linkCount] { false };
	else
		this->linkStates = new bool[0];

	for (unsigned int i = 0; i < linkCount; i++)
	{
		links[i].powered = &this->linkStates[i];
	}

	if (componentCount > 0)
	{
		readBuffer = new Component*[componentCount + 1] { nullptr };
		writeBuffer = new Component*[componentCount + 1] { nullptr };
		for (unsigned int i = 0; i < componentCount; i++)
		{
			readBuffer[i] = components[i];
		}
	}
	else
	{
		readBuffer = new Component*[0];
		writeBuffer = new Component*[0];
	}
	
	currentState = Board::Stopped;

	lastCapture = std::chrono::high_resolution_clock::now();

	barrier = new SpinlockBarrier(0, [this]() {
		auto* readBuffer = this->readBuffer;
		this->readBuffer = this->writeBuffer;
		this->writeBuffer = readBuffer;
		for (auto i = 0; this->writeBuffer[i]; i++)
		{
			this->writeBuffer[i] = nullptr;
		}
		std::atomic_store_explicit(&this->bufferCount, 0, std::memory_order_relaxed);
		
		tickEvent.emit(nullptr, Events::EventArgs());
		tick++;

		const auto timestamp = std::chrono::high_resolution_clock::now();

		const auto diff = (timestamp - lastCapture).count();
		if (diff > 10e8) {
			currentSpeed = ((tick - lastCaptureTick) * (unsigned long)10e8) / diff;
			lastCapture = timestamp;
			lastCaptureTick = tick;
		}

		if ((unsigned long long)(timestamp - started).count() > this->timeout) {
			currentState = Board::Stopped;
			return;
		}

		if (!--cyclesLeft || currentState == Board::Stopping) {
			currentState = Board::Stopped;
			return;
		}
	}, 2);
}

#endif

unsigned int Board::getNextComponentIndex()
{
	return componentIndex++;
}

unsigned int Board::getThreadCount() const
{
	return threadCount;
}

Component** Board::getComponents() const
{
	return components;
}

Link* Board::getLinks() const
{
	return links;
}

Board::State Board::getCurrentState() const
{
	return currentState;
}

unsigned long long int Board::getCurrentTick() const
{
	return tick;
}

void Board::stop()
{
	if (currentState != Board::Running)
		return;

	currentState = Board::Stopping;
	for (unsigned i = 0; i < threadCount; i++) {
		threads[i]->join();
	}
}

#ifdef __EMSCRIPTEN__

void Board::start(unsigned long long cyclesLeft, unsigned long ms, unsigned int threadCount, const bool synchronized)
{
	if (this->currentState != Board::Stopped)
		return;

	this->currentState = Board::Running;
	this->started = std::chrono::high_resolution_clock::now();
	this->timeout = (unsigned long long)ms * (unsigned long long)10e5;
	this->cyclesLeft = cyclesLeft;

	if (this->cyclesLeft <= 0 || this->timeout <= 0)
	{
		currentState = Board::Stopped;
		return;
	}

	while (true) {
		for (unsigned int i = 0; i < componentCount; i++) {
			if (readBuffer[i])
				components[i]->compute();
			wipeBuffer[i] = false;
		}

		for (unsigned int i = 0; i < linkCount; i++) {
			*links[i].powered = std::any_of(links[i].outputs, links[i].outputs + links[i].outputCount, [](Output* x) { return x->getPowered(); });
		}

		auto* readPointer(readBuffer);

		readBuffer = writeBuffer;
		writeBuffer = wipeBuffer;
		wipeBuffer = readPointer;

        tickEvent.emit(nullptr, Events::EventArgs());
		tick++;

		const auto timestamp = std::chrono::high_resolution_clock::now();

		const auto diff = (timestamp - lastCapture).count();
		if (diff > 10e8) {
			currentSpeed = ((tick - lastCaptureTick) * (unsigned long)10e8) / diff;
			lastCapture = timestamp;
			lastCaptureTick = tick;
		}

		if ((unsigned long long)(timestamp - started).count() > this->timeout) {
			currentState = Board::Stopped;
			return;
		}

		if (!--this->cyclesLeft || currentState == Board::Stopping) {
			currentState = Board::Stopped;
			return;
		}
	}
}

#else

void Board::start(const unsigned long long cyclesLeft, const unsigned long ms, const unsigned int threadCount, const bool synchronized)
{
	if (currentState != Board::Stopped)
		return;

	this->started = std::chrono::high_resolution_clock::now();
	this->cyclesLeft = cyclesLeft;
	this->timeout = (unsigned long long)ms * (unsigned long long)10e5;
	this->currentState = Board::Running;

	if (this->cyclesLeft <= 0 || this->timeout <= 0)
	{
		currentState = Board::Stopped;
		return;
	}

	for (unsigned int i = 0; i < this->threadCount; i++)
	{
		if (threads[i] != nullptr)
			delete threads[i];
	}
	delete[] this->threads;
	this->threads = new std::thread*[threadCount] { nullptr };
	this->threadCount = threadCount;
	this->barrier->setStageCount(threadCount);

	for (unsigned int i = 0; i < threadCount; i++) {
		threads[i] = new std::thread([this](const int id) {
			while (true) {
				if (currentState == Board::Stopped)
					return;

				for (unsigned long i = id; this->readBuffer[i] && i < this->componentCount; i += this->threadCount) {
					this->readBuffer[i]->compute();
				}
				barrier->wait();

				for (unsigned int i = id; i < linkCount; i += this->threadCount) {
					*links[i].powered = std::any_of(links[i].outputs, links[i].outputs + links[i].outputCount, [](Output* x) { return x->getPowered(); });
				}
				barrier->wait();
			}
		}, i);
	}

	if (synchronized)
	{
		for (unsigned i = 0; i < threadCount; i++)
		{
			threads[i]->join();
		}
	}
}

#endif
