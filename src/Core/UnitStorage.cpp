#include "UnitStorage.hpp"

#include <algorithm>

namespace sw
{
	void UnitStorage::add(std::shared_ptr<Unit> unit)
	{
		_units.push_back(std::move(unit));
	}

	std::shared_ptr<Unit> UnitStorage::findById(uint32_t id) const
	{
		for (auto& unit : _units)
		{
			if (unit->id == id)
			{
				return unit;
			}
		}
		return nullptr;
	}

	const std::vector<std::shared_ptr<Unit>>& UnitStorage::all() const
	{
		return _units;
	}

	void UnitStorage::removeDeadUnits()
	{
		_units.erase(
			std::remove_if(_units.begin(), _units.end(), [](const std::shared_ptr<Unit>& u) { return !u->isAlive(); }),
			_units.end());
	}

	std::size_t UnitStorage::size() const
	{
		return _units.size();
	}

	std::size_t UnitStorage::aliveCount() const
	{
		return static_cast<std::size_t>(
			std::count_if(_units.begin(), _units.end(), [](const std::shared_ptr<Unit>& u) { return u->isAlive(); }));
	}
}
