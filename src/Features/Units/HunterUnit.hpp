#pragma once

#include <Core/IAction.hpp>
#include <Core/MarchState.hpp>
#include <Core/Unit.hpp>
#include <Features/Actions/RangedAttackAction.hpp>
#include <cstdint>
#include <memory>

namespace sw
{
	// Creates a Hunter unit.
	// MarchState is a separate shared object so MARCH commands and execute() share the same state.
	inline std::shared_ptr<Unit> makeHunter(
		uint32_t id, uint32_t x, uint32_t y, uint32_t hp, uint32_t agility, uint32_t strength, uint32_t range)
	{
		auto unit = std::make_shared<Unit>(id, x, y, static_cast<int32_t>(hp));
		auto marchState = std::make_shared<MarchState>();
		auto action = std::make_shared<RangedAttackAction>(agility, strength, range, marchState);

		unit->components.add<MarchState>(marchState);
		unit->components.add<RangedAttackAction, IAction>(action);
		return unit;
	}
}
