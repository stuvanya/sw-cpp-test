#include "RangedAttackAction.hpp"
#include "MoveAction.hpp"

#include <Core/GameContext.hpp>
#include <Core/Map.hpp>
#include <Core/Unit.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>

#include <vector>

namespace sw
{
	RangedAttackAction::RangedAttackAction(uint32_t agility, uint32_t strength, uint32_t range,
	                                       std::shared_ptr<MarchState> marchState)
		: _agility(agility), _strength(strength), _range(range), _movable(std::move(marchState))
	{}

	bool RangedAttackAction::canAct(const Unit& self, const GameContext& ctx) const
	{
		// Can act if there is any targetable enemy within melee or ranged distance
		for (auto& unit : ctx.units.all())
		{
			if (unit->id == self.id || !unit->isAlive() || !unit->targetable)
				continue;

			uint32_t d = Map::distance(self.x, self.y, unit->x, unit->y);
			if (d >= 1 && d <= _range)
				return true;
		}
		// Or if there is an active march target not yet reached
		return _movable->hasTarget
		    && (self.x != _movable->targetX || self.y != _movable->targetY);
	}

	void RangedAttackAction::execute(Unit& self, GameContext& ctx)
	{
		std::vector<std::shared_ptr<Unit>> adjacent;
		std::vector<std::shared_ptr<Unit>> ranged;

		for (auto& unit : ctx.units.all())
		{
			if (unit->id == self.id || !unit->isAlive() || !unit->targetable)
				continue;

			uint32_t d = Map::distance(self.x, self.y, unit->x, unit->y);

			if (d == 1)
				adjacent.push_back(unit);
			else if (d >= 2 && d <= _range)
				ranged.push_back(unit);
		}

		// Use shared RNG from GameContext — avoids hidden static state
		if (adjacent.empty() && !ranged.empty())
		{
			// Swift shot: pick a random unit at distance 2..range
			std::uniform_int_distribution<std::size_t> dist(0, ranged.size() - 1);
			auto& target = ranged[dist(ctx.rng)];

			target->hp -= static_cast<int32_t>(_agility);

			ctx.eventLog.log(ctx.tick, io::UnitAttacked{
				self.id,
				target->id,
				_agility,
				static_cast<uint32_t>(std::max(0, target->hp))
			});

			if (!target->isAlive())
				ctx.eventLog.log(ctx.tick, io::UnitDied{target->id});
		}
		else if (!adjacent.empty())
		{
			// Shadow strike: pick a random adjacent unit
			std::uniform_int_distribution<std::size_t> dist(0, adjacent.size() - 1);
			auto& target = adjacent[dist(ctx.rng)];

			target->hp -= static_cast<int32_t>(_strength);

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
			// No targets — move toward march target
			MoveAction move(_movable);
			move.execute(self, ctx);
		}
	}
}
