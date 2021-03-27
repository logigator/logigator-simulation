#pragma once
#include <thread>
#include <chrono>
#include "events.h"
#include "fast_stack_atomic.h"

class Component;
class SpinlockBarrier;
class Link;

class Board
{
public:
	Board();
	~Board();
	void init(Component** components, Link* links, size_t componentCount, size_t linkCount);
	size_t getNextComponentIndex();
	size_t componentCount = 0;
	size_t linkCount = 0;
	bool* linkStates = nullptr;
	enum State { Uninitialized, Stopped, Running, Stopping };
	size_t getThreadCount() const;
	Component** getComponents() const;
	Link* getLinks() const;
	State getCurrentState() const;
	uint_fast64_t getCurrentTick() const;
	uint_fast64_t currentSpeed = 0;
	Events::Event<> tickEvent;
	void stop();
	void start(uint_fast64_t cyclesLeft, uint_fast64_t timeout, size_t threadCount, bool synchronized = false);
	void resume();
	FastStackAtomic<Link*>* readBuffer = new FastStackAtomic<Link*>();
	FastStackAtomic<Link*>* writeBuffer = new FastStackAtomic<Link*>();

private:
	size_t threadCount = 0;
	Component** components = nullptr;
	Link* links = nullptr;
	State currentState = Board::Uninitialized;
	uint_fast64_t tick = 0;
	std::thread** threads = nullptr;
	SpinlockBarrier* barrier = nullptr;
	std::chrono::high_resolution_clock::time_point lastCapture;
	std::chrono::high_resolution_clock::time_point started;
	uint_fast64_t lastCaptureTick = 0;
	size_t componentIndex = 0;
	uint_fast64_t cyclesLeft = 0;
	uint_fast64_t timeout = 0;
};

