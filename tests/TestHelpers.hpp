#pragma once

// Convenience header for test files.
// Provides a pre-built GameContext backed by a silent EventLog
// (no stdout output) that still captures events for assertions.

#include <Core/GameContext.hpp>
#include <Core/Map.hpp>
#include <Core/UnitStorage.hpp>
#include <IO/System/EventLog.hpp>
#include <memory>

namespace sw::test
{
	// Owns all game-state objects and wires them into a GameContext.
	// EventLog is constructed in silent mode (nullptr ostream) so tests
	// produce no stdout noise, but events are still captured in eventLog.captured().
	struct GameFixture
	{
		Map map;
		UnitStorage units;
		EventLog eventLog{nullptr};	 // silent: no stdout, captures events
		GameContext ctx;

		explicit GameFixture(uint32_t w = 10, uint32_t h = 10) :
				map(w, h),
				ctx{map, units, eventLog}
		{
			ctx.rng.seed(0);  // deterministic RNG for reproducible tests
		}
	};
}  // namespace sw::test
