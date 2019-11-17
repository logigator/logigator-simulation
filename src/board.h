#pragma once
#include <thread>
#include <chrono>
#include <atomic>
#include "events.h"

class Component;
class SpinlockBarrier;
class Link;

class Board
{
public:
	Board();
	~Board();
	void init(Component** components, Link* links, unsigned int componentCount, unsigned int linkCount);
	void init(Component** components, Link* links, unsigned int componentCount, unsigned int linkCount, unsigned int threadCount);
	unsigned int getNextComponentIndex();
	unsigned int getNextLinkIndex();
	bool* readBuffer = nullptr;
	bool* writeBuffer = nullptr;
	bool* wipeBuffer = nullptr;
	bool* linkDefaults = nullptr;
	size_t componentCount = 0;
	size_t linkCount = 0;
	bool* linkStates = nullptr;
	enum State { Uninitialized, Stopped, Running, Stopping };
	unsigned int getThreadCount() const;
	Component** getComponents() const;
	Link* getLinks() const;
	State getCurrentState() const;
	unsigned long long int getCurrentTick() const;
	unsigned long long currentSpeed = 0;
	Events::Event<> tickEvent;
	void stop();
	void start();
	void startManual(unsigned long long cyclesLeft);
	void startTimeout(unsigned int ms);
	
private:
	void startInternal(const unsigned long long cyclesLeft, const unsigned long long ns);
	bool* buffer1 = nullptr;
	bool* buffer2 = nullptr;
	bool* buffer3 = nullptr;
	unsigned int threadCount = 1;
	Component** components = nullptr;
	Link* links = nullptr;
	State currentState = Board::Uninitialized;
	unsigned long long int tick = 0;
	std::thread** threads = nullptr;
	SpinlockBarrier* barrier = nullptr;
	std::chrono::high_resolution_clock::time_point lastCapture;
	std::chrono::high_resolution_clock::time_point started;
	unsigned long long int lastCaptureTick = 0;
	unsigned int componentIndex = 0;
	unsigned int linkIndex = 0;
	unsigned long long cyclesLeft = 0;
	unsigned long long timeout = 0;
};

