#pragma once

#include "details/PrintFieldVisitor.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace sw
{
	// Logs game events to an ostream (default: stdout) and optionally captures
	// event names in an in-memory vector for testing.
	//
	// In production:   EventLog log;                  // writes to std::cout
	// In tests:        EventLog log(nullptr);          // silent, captures only
	//                  EventLog log(&myStringStream);  // writes + captures
	class EventLog
	{
	public:
		// out == nullptr  → silent mode (no output, only capture)
		// out == &std::cout (default) → production mode
		explicit EventLog(std::ostream* out = &std::cout)
			: _out(out)
		{}

		template <class TEvent>
		void log(uint64_t tick, TEvent&& event)
		{
			// Always capture the event name for inspection
			_captured.push_back({tick, std::string(TEvent::Name)});
			++_totalEvents;

			if (_out)
			{
				*_out << "[" << tick << "] " << TEvent::Name << " ";
				PrintFieldVisitor visitor(*_out);
				event.visit(visitor);
				*_out << "\n";
			}
		}

		uint64_t totalEvents() const { return _totalEvents; }

		// --- Test helpers ---

		struct Entry
		{
			uint64_t    tick;
			std::string name;
		};

		const std::vector<Entry>& captured() const { return _captured; }

		bool hasEvent(const std::string& name) const
		{
			return std::any_of(_captured.begin(), _captured.end(),
				[&](const Entry& e) { return e.name == name; });
		}

		std::size_t countEvents(const std::string& name) const
		{
			return static_cast<std::size_t>(
				std::count_if(_captured.begin(), _captured.end(),
					[&](const Entry& e) { return e.name == name; }));
		}

		std::string lastEventName() const
		{
			return _captured.empty() ? "" : _captured.back().name;
		}

		void clearCaptured() { _captured.clear(); }

	private:
		std::ostream*      _out;
		uint64_t           _totalEvents{0};
		std::vector<Entry> _captured;
	};
}
