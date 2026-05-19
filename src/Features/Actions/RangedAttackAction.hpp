#pragma once

#include <Core/IAction.hpp>
#include <Core/MarchState.hpp>
#include <cstdint>
#include <memory>

namespace sw
{
	// Hunter attack logic:
	// 1. No adjacent enemies AND ranged targets exist (distance 2..range) -> ranged shot (agility damage)
	// 2. Adjacent enemies exist -> melee strike (strength damage)
	// 3. Otherwise -> move toward march target
	class RangedAttackAction : public IAction
	{
	public:
		RangedAttackAction(uint32_t agility, uint32_t strength, uint32_t range, std::shared_ptr<MarchState> marchState);

		void execute(Unit& self, GameContext& ctx) override;
		bool canAct(const Unit& self, const GameContext& ctx) const override;

	private:
		uint32_t _agility;
		uint32_t _strength;
		uint32_t _range;
		std::shared_ptr<MarchState> _movable;
	};
}
