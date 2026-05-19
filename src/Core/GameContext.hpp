#pragma once

#include "Map.hpp"
#include "UnitStorage.hpp"
#include <IO/System/EventLog.hpp>

#include <cstdint>
#include <random>

namespace sw
{
	// Holds all game services. Passed to IAction::execute so actions can read/modify game state.
	struct GameContext
	{
		Map&         map;
		UnitStorage& units;
		EventLog&    eventLog;
		uint64_t     tick{0};

		// Shared RNG for all actions — avoids hidden static state in action classes.
		std::mt19937 rng{std::random_device{}()};
	};
}
