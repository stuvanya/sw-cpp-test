#include "TurnManager.hpp"
#include "IAction.hpp"

#include <memory>
#include <vector>

namespace sw
{
	TurnManager::TurnManager(GameContext& ctx)
		: _ctx(ctx)
	{}

	bool TurnManager::isFinished() const
	{
		// Stop when one or zero alive units remain
		return _ctx.units.aliveCount() <= 1;
	}

	// Returns true if at least one alive unit reports it can act next turn.
	// This is the correct implementation of the README condition:
	// "no units capable of acting in the next turn".
	bool TurnManager::anyUnitCanAct() const
	{
		for (auto& unit : _ctx.units.all())
		{
			if (!unit->isAlive())
				continue;

			if (auto action = unit->components.template get<IAction>())
			{
				if (action->canAct(*unit, _ctx))
					return true;
			}
		}
		return false;
	}

	void TurnManager::run()
	{
		while (!isFinished())
		{
			// Check before the tick: if nobody can act, stop now.
			if (!anyUnitCanAct())
				break;

			++_ctx.tick;

			// Copy the unit list so removals mid-turn don't affect iteration
			std::vector<std::shared_ptr<Unit>> unitsCopy(_ctx.units.all());

			for (auto& unit : unitsCopy)
			{
				if (!unit->isAlive())
					continue;

				if (auto action = unit->components.template get<IAction>())
				{
					action->execute(*unit, _ctx);
				}
			}

			// Remove dead units before the next turn starts
			_ctx.units.removeDeadUnits();
		}
	}
}
