#include "TestRunner.hpp"
#include <Core/Unit.hpp>
#include <Core/UnitStorage.hpp>

using namespace sw;

static std::shared_ptr<Unit> makeUnit(uint32_t id, int32_t hp = 10)
{
	return std::make_shared<Unit>(id, 0, 0, hp);
}

TEST(UnitStorage_add_and_findById_returns_unit)
{
	UnitStorage s;
	auto u = makeUnit(42);
	s.add(u);
	CHECK(s.findById(42) != nullptr);
	CHECK_EQ(s.findById(42)->id, uint32_t(42));
}

TEST(UnitStorage_findById_returns_nullptr_for_unknown_id)
{
	UnitStorage s;
	CHECK(s.findById(99) == nullptr);
}

TEST(UnitStorage_size_counts_all_units_including_dead)
{
	UnitStorage s;
	s.add(makeUnit(1, 10));
	s.add(makeUnit(2, 0));
	CHECK_EQ(s.size(), std::size_t(2));
}

TEST(UnitStorage_aliveCount_counts_only_alive_units)
{
	UnitStorage s;
	s.add(makeUnit(1, 10));
	s.add(makeUnit(2, 0));
	s.add(makeUnit(3, -5));
	CHECK_EQ(s.aliveCount(), std::size_t(1));
}

TEST(UnitStorage_removeDeadUnits_removes_units_with_hp_le_zero)
{
	UnitStorage s;
	s.add(makeUnit(1, 10));
	s.add(makeUnit(2, 0));
	s.add(makeUnit(3, -1));
	s.removeDeadUnits();
	CHECK_EQ(s.size(), std::size_t(1));
	CHECK(s.findById(1) != nullptr);
	CHECK(s.findById(2) == nullptr);
	CHECK(s.findById(3) == nullptr);
}

TEST(UnitStorage_all_preserves_insertion_order)
{
	UnitStorage s;
	s.add(makeUnit(10));
	s.add(makeUnit(20));
	s.add(makeUnit(30));
	auto& all = s.all();
	CHECK_EQ(all[0]->id, uint32_t(10));
	CHECK_EQ(all[1]->id, uint32_t(20));
	CHECK_EQ(all[2]->id, uint32_t(30));
}
