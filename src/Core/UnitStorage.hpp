#pragma once

#include "Unit.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace sw
{
	// Stores all units in creation order (which is also turn order).
	class UnitStorage
	{
	public:
		void add(std::shared_ptr<Unit> unit);

		// Returns nullptr if not found.
		std::shared_ptr<Unit> findById(uint32_t id) const;

		// All units including dead ones (use isAlive() to filter).
		// Returns a const reference — callers must not modify the container directly.
		// To iterate safely during a turn (with possible removals), copy the result first.
		const std::vector<std::shared_ptr<Unit>>& all() const;

		// Remove units with hp <= 0. Call at the end of each turn.
		void removeDeadUnits();

		// Returns total count including dead units. Use aliveCount() for game-logic checks.
		std::size_t size() const;

		// Returns the number of units with hp > 0.
		std::size_t aliveCount() const;

	private:
		std::vector<std::shared_ptr<Unit>> _units;
	};
}
