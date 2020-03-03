#pragma once

#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <functional>
#include "fast_stack.h"

namespace Events
{
	// Base class for emitter.
	class Emitter
	{
	public:
		virtual ~Emitter() = default;
	};

	// Base class for arguments.
	class EventArgs
	{
	public:
		virtual ~EventArgs() = default;
	};

	template <typename TEventArgs = EventArgs>
	class EventHandler : private std::function < void(Emitter* emitter, TEventArgs&) >
	{
	public:
		template <typename F>
		explicit EventHandler(const F& f) : std::function<void(Emitter*, TEventArgs&)>(f) {}
		void operator ()(Emitter* emitter, TEventArgs& args) { return std::function<void(Emitter*, TEventArgs&)>::operator()(emitter, args); }
	};

	template <typename TEventArgs = EventArgs>
	class Event
	{
	public:
		// Subscribe to events.
		void operator +=(EventHandler<TEventArgs>* handler)
		{
			_PendingToAdd.push(handler);
		}

		// Unsubscribe from events.
		void operator -=(EventHandler<TEventArgs>* handler)
		{
			_PendingToRemove.push(handler);
		}

		// Emit an event.
		void emit(Emitter* emitter, TEventArgs&& args) // For passing an l or rvalue reference.
		{
			while(!_PendingToAdd.empty())
			{
				_Handlers.push_back(_PendingToAdd.pop());
			}
			while(!_PendingToRemove.empty())
			{
				// Find a handler that points to the same target and remove it.
				auto handler = _PendingToRemove.pop();
				auto it = std::find_if(_Handlers.begin(), _Handlers.end(), [handler](EventHandler<TEventArgs>* other) {
					return other == handler;
				});
				if (it != _Handlers.end())
					_Handlers.erase(it);
			}
			
			// Call each handler with args.
			std::for_each(_Handlers.begin(), _Handlers.end(), [emitter, &args](EventHandler<TEventArgs>* h) {
				(*h)(emitter, args);
			});
		}

	protected:
		FastStack<EventHandler<TEventArgs>*> _PendingToAdd;
		FastStack<EventHandler<TEventArgs>*> _PendingToRemove;
		std::vector<EventHandler<TEventArgs>*> _Handlers;
	};
}