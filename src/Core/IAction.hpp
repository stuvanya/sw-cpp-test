#pragma once

namespace sw
{
	struct Unit;
	struct GameContext;

	// Interface for unit actions. Each unit type implements this.
	struct IAction
	{
		virtual void execute(Unit& self, GameContext& ctx) = 0;

		// Returns true if the unit has something to do this turn:
		// enemies in range, or an active march target.
		// Used by TurnManager to detect a fully idle simulation.
		virtual bool canAct(const Unit& self, const GameContext& ctx) const = 0;

		virtual ~IAction() = default;
	};
}
