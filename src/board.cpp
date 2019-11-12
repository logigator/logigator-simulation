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
	delete[] components;

	delete[] links;

    #ifndef __EMSCRIPTEN__

	for (unsigned int i = 0; i < threadCount; i++) {
		delete threads[i];
	}
	delete[] threads;
	delete barrier;

	#endif

	delete[] linkStates;
	delete[] buffer1;
	delete[] buffer2;
	delete[] buffer3;
}

#ifdef __EMSCRIPTEN__

void Board::init(Component** components, Link* links, const unsigned int componentCount, const unsigned int linkCount, const unsigned int threadCount)
{
	Board::init(components, links, componentCount, linkCount);
}

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
	Board::init(components, links, componentCount, linkCount, 1);
}

void Board::init(Component** components, Link* links, const unsigned int componentCount, const unsigned int linkCount, const unsigned int threadCount)
{
	if (currentState != Board::Uninitialized)
		return;

	this->components = components;
	this->links = links;
	this->threadCount = threadCount;
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

	threads = new std::thread*[threadCount] { nullptr };
	lastCapture = std::chrono::high_resolution_clock::now();

	barrier = new SpinlockBarrier(threadCount, [this]() {
		auto* readPointer(readBuffer);

		readBuffer = writeBuffer;
		writeBuffer = wipeBuffer;
		wipeBuffer = readPointer;

		tickEvent.emit(nullptr, Events::EventArgs());
		tick++;

		const auto timestamp = std::chrono::high_resolution_clock::now();

		if ((unsigned long long)(timestamp - started).count() > this->timeout) {
			currentState = Board::Stopped;
			return;
		}

		const auto diff = (timestamp - lastCapture).count();
		if (diff > 10e8) {
			currentSpeed = ((tick - lastCaptureTick) * (unsigned long)10e8) / diff;
			lastCapture = timestamp;
			lastCaptureTick = tick;
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

void Board::start()
{
	Board::startInternal(UINT64_MAX, UINT64_MAX);
}

void Board::startManual(const unsigned long long cyclesLeft)
{
	Board::startInternal(cyclesLeft, UINT64_MAX);
}

void Board::startTimeout(const unsigned int ms)
{
	Board::startInternal(UINT64_MAX, (unsigned long long)ms * 10e5);
}

#ifdef __EMSCRIPTEN__

void Board::startInternal(unsigned long long cyclesLeft, unsigned long long ns)
{
	if (currentState != Board::Stopped)
		return;

	currentState = Board::Running;
	this->started = std::chrono::high_resolution_clock::now();

	while (true) {
		for (unsigned int i = 0; i < componentCount; i++) {
			if (readBuffer[i])
				components[i]->compute();
			wipeBuffer[i] = false;
		}

		for (unsigned int i = 0; i < linkCount; i++) {
			for(unsigned int j = 0; j < links[i].outputCount; j++) {
				if (readBuffer[links[i].outputs[j]->getComponent()->componentIndex]) {
					*links[i].powered = std::any_of(links[i].outputs, links[i].outputs + links[i].outputCount, [](Output* x) { return x->getPowered(); });
					break;
				}
			}
		}

		auto* readPointer(readBuffer);

		readBuffer = writeBuffer;
		writeBuffer = wipeBuffer;
		wipeBuffer = readPointer;

        tickEvent.emit(nullptr, Events::EventArgs());
		tick++;

		const auto timestamp = std::chrono::high_resolution_clock::now();

		if ((unsigned long long)(timestamp - started).count() > ns) {
			currentState = Board::Stopped;
			return;
		}

		const auto diff = (timestamp - lastCapture).count();
		if (diff > 10e8) {
			currentSpeed = ((tick - lastCaptureTick) * (unsigned long)10e8) / diff;
			lastCapture = timestamp;
			lastCaptureTick = tick;
		}

		if (!--cyclesLeft || currentState == Board::Stopping) {
			currentState = Board::Stopped;
			return;
		}
	}
}

#else

void Board::startInternal(unsigned long long cyclesLeft, unsigned long long ns)
{
	if (currentState != Board::Stopped)
		return;

	this->cyclesLeft = cyclesLeft;
	this->timeout = ns;

	for (unsigned int i = 0; i < threadCount; i++) {
		if (threads[i] != nullptr)
			delete threads[i];

		currentState = Board::Running;
		threads[i] = new std::thread([this](int id) {
			while (true) {
				if (currentState == Board::Stopped)
					return;

				for (unsigned int i = id; i < componentCount; i += threadCount) {
					if (readBuffer[i])
						components[i]->compute();
					wipeBuffer[i] = false;
				}
				barrier->wait();

				for (unsigned int i = id; i < linkCount; i += threadCount) {
					for (unsigned int j = 0; j < links[i].outputCount; j++) {
						if (readBuffer[links[i].outputs[j]->getComponent()->componentIndex]) {
							*links[i].powered = std::any_of(links[i].outputs, links[i].outputs + links[i].outputCount, [](Output* x) { return x->getPowered(); });
							break;
						}
					}
				}

				barrier->wait();
			}
		}, i);
	}
}

#endif
