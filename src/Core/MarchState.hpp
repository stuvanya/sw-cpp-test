#pragma once

#include <cstdint>

namespace sw
{
	// Holds the march target state for a movable unit.
	// Registered as a separate component in TypeRegistry.
	// Static units (e.g. Tower) do not have this component — MARCH commands
	// for such units are silently ignored by main.cpp.
	struct MarchState
	{
		uint32_t targetX{0};
		uint32_t targetY{0};
		bool hasTarget{false};
	};
}
