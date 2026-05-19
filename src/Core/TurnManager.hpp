#pragma once

#include "GameContext.hpp"

namespace sw
{
	// Runs the simulation loop turn by turn until the end condition is met.
	class TurnManager
	{
	public:
		explicit TurnManager(GameContext& ctx);

		// Runs the simulation until no unit can act or only one unit remains.
		void run();

	private:
		GameContext& _ctx;

		// Returns true when only one (or zero) alive units remain.
		bool isFinished() const;

		// Returns true if at least one alive unit can act this turn.
		// Implements the README condition: "no units capable of acting in the next turn".
		bool anyUnitCanAct() const;
	};
}
