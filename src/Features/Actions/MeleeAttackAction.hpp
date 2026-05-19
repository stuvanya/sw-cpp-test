#pragma once

#include <Core/IAction.hpp>
#include <Core/MarchState.hpp>

#include <cstdint>
#include <memory>

namespace sw
{
	// Melee attack: hits a random adjacent unit each turn.
	// If no adjacent targets, moves toward march target instead.
	class MeleeAttackAction : public IAction
	{
	public:
		MeleeAttackAction(uint32_t strength, std::shared_ptr<MarchState> marchState);

		void execute(Unit& self, GameContext& ctx) override;
		bool canAct(const Unit& self, const GameContext& ctx) const override;

	private:
		uint32_t                    _strength;
		std::shared_ptr<MarchState> _movable;
	};
}
