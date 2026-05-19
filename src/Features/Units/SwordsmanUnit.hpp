#pragma once

#include <Features/Actions/MeleeAttackAction.hpp>
#include <Core/IAction.hpp>
#include <Core/MarchState.hpp>
#include <Core/Unit.hpp>

#include <cstdint>
#include <memory>

namespace sw
{
	// Creates a Swordsman unit.
	// MarchState is a separate shared object so MARCH commands and execute() share the same state.
	inline std::shared_ptr<Unit> makeSwordsman(
		uint32_t id, uint32_t x, uint32_t y, uint32_t hp, uint32_t strength)
	{
		auto unit       = std::make_shared<Unit>(id, x, y, static_cast<int32_t>(hp));
		auto marchState = std::make_shared<MarchState>();
		auto action     = std::make_shared<MeleeAttackAction>(strength, marchState);

		unit->components.add<MarchState>(marchState);
		unit->components.add<MeleeAttackAction, IAction>(action);
		return unit;
	}
}
