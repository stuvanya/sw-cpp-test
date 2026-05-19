#pragma once

#include <Core/IAction.hpp>
#include <Core/MarchState.hpp>
#include <memory>

namespace sw
{
	// Moves the unit one step toward its march target each turn.
	// Reads march state from a shared MarchState instance.
	class MoveAction : public IAction
	{
	public:
		explicit MoveAction(std::shared_ptr<MarchState> marchState);

		void execute(Unit& self, GameContext& ctx) override;
		bool canAct(const Unit& self, const GameContext& ctx) const override;

	private:
		std::shared_ptr<MarchState> _movable;
	};
}
