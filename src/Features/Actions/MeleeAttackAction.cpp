#include "MeleeAttackAction.hpp"
#include "MoveAction.hpp"

#include <Core/GameContext.hpp>
#include <Core/Map.hpp>
#include <Core/Unit.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>

#include <vector>

namespace sw
{
	MeleeAttackAction::MeleeAttackAction(uint32_t strength, std::shared_ptr<MarchState> marchState)
		: _strength(strength), _movable(std::move(marchState))
	{}

	bool MeleeAttackAction::canAct(const Unit& self, const GameContext& ctx) const
	{
		// Can act if there is an adjacent targetable enemy
		for (auto& unit : ctx.units.all())
		{
			if (unit->id != self.id && unit->isAlive() && unit->targetable
			        && Map::isNeighbor(self.x, self.y, unit->x, unit->y))
				return true;
		}
		// Or if there is an active march target not yet reached
		return _movable->hasTarget
		    && (self.x != _movable->targetX || self.y != _movable->targetY);
	}

	void MeleeAttackAction::execute(Unit& self, GameContext& ctx)
	{
		// Collect all alive adjacent units that can be targeted
		std::vector<std::shared_ptr<Unit>> targets;
		for (auto& unit : ctx.units.all())
		{
			if (unit->id != self.id && unit->isAlive() && unit->targetable
				    && Map::isNeighbor(self.x, self.y, unit->x, unit->y))
				targets.push_back(unit);
		}

		if (!targets.empty())
		{
			// Pick a random target using shared RNG from GameContext
			std::uniform_int_distribution<std::size_t> dist(0, targets.size() - 1);
			auto& target = targets[dist(ctx.rng)];

			target->hp -= _strength;

			ctx.eventLog.log(ctx.tick, io::UnitAttacked{
				self.id,
				target->id,
				_strength,
				static_cast<uint32_t>(std::max(0, target->hp))
			});

			if (!target->isAlive())
				ctx.eventLog.log(ctx.tick, io::UnitDied{target->id});
		}
		else
		{
			// No targets nearby — move toward march target
			MoveAction move(_movable);
			move.execute(self, ctx);
		}
	}
}
