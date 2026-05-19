#include "MoveAction.hpp"

#include <Core/GameContext.hpp>
#include <Core/Map.hpp>
#include <Core/Unit.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/UnitMoved.hpp>

namespace sw
{
	MoveAction::MoveAction(std::shared_ptr<MarchState> marchState) :
			_movable(std::move(marchState))
	{}

	bool MoveAction::canAct(const Unit& self, const GameContext& /*ctx*/) const
	{
		// Can act if there is an active march target and we haven't reached it yet
		return _movable->hasTarget && (self.x != _movable->targetX || self.y != _movable->targetY);
	}

	void MoveAction::execute(Unit& self, GameContext& ctx)
	{
		if (!_movable->hasTarget)
		{
			return;
		}

		// Already at target
		if (self.x == _movable->targetX && self.y == _movable->targetY)
		{
			_movable->hasTarget = false;
			ctx.eventLog.log(ctx.tick, io::MarchEnded{self.id, self.x, self.y});
			return;
		}

		auto neighbors = ctx.map.getFreeNeighbors(self.x, self.y);

		if (neighbors.empty())
		{
			// Completely surrounded — wait, do not cancel the march.
			// Another unit may move away next turn and free a path.
			return;
		}

		// Find the neighbor closest to the target
		std::pair<uint32_t, uint32_t> best = neighbors.front();
		uint32_t bestDist = Map::distance(best.first, best.second, _movable->targetX, _movable->targetY);

		for (auto& [nx, ny] : neighbors)
		{
			uint32_t dist = Map::distance(nx, ny, _movable->targetX, _movable->targetY);
			if (dist < bestDist)
			{
				bestDist = dist;
				best = {nx, ny};
			}
		}

		// If no free neighbor brings us closer — temporarily blocked, keep the target
		// and retry next turn (another unit may vacate the blocking cell).
		uint32_t currentDist = Map::distance(self.x, self.y, _movable->targetX, _movable->targetY);
		if (bestDist >= currentDist)
		{
			return;
		}

		// Move to the chosen cell
		if (self.occupiesCell)
		{
			ctx.map.free(self.x, self.y);
		}
		self.x = best.first;
		self.y = best.second;
		if (self.occupiesCell)
		{
			ctx.map.occupy(self.x, self.y);
		}

		ctx.eventLog.log(ctx.tick, io::UnitMoved{self.id, self.x, self.y});

		// Check if we reached the target
		if (self.x == _movable->targetX && self.y == _movable->targetY)
		{
			_movable->hasTarget = false;
			ctx.eventLog.log(ctx.tick, io::MarchEnded{self.id, self.x, self.y});
		}
	}
}
