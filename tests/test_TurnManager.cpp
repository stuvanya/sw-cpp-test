#include "TestRunner.hpp"
#include "TestHelpers.hpp"

#include <Core/TurnManager.hpp>
#include <Features/Units/SwordsmanUnit.hpp>

using namespace sw;
using namespace sw::test;

TEST(TurnManager_does_not_run_when_zero_units)
{
	GameFixture f;
	TurnManager tm(f.ctx);
	tm.run();
	CHECK_EQ(f.ctx.tick, uint64_t(0));
}

TEST(TurnManager_does_not_run_when_only_one_unit)
{
	GameFixture f;
	auto unit = makeSwordsman(1, 0, 0, 100, 10);
	f.units.add(unit);
	f.map.occupy(0, 0);

	TurnManager tm(f.ctx);
	tm.run();

	CHECK_EQ(f.ctx.tick, uint64_t(0));
}

TEST(TurnManager_stops_when_no_unit_can_act)
{
	GameFixture f(20, 20);
	// Two swordsmen far apart with no march targets — neither can act
	auto s1 = makeSwordsman(1, 0,  0,  100, 10);
	auto s2 = makeSwordsman(2, 19, 19, 100, 10);
	f.units.add(s1);
	f.units.add(s2);
	f.map.occupy(0, 0);
	f.map.occupy(19, 19);

	TurnManager tm(f.ctx);
	tm.run();

	CHECK_EQ(f.ctx.tick, uint64_t(0));
}

TEST(TurnManager_advances_tick_when_units_can_act)
{
	GameFixture f;
	auto s1 = makeSwordsman(1, 0, 0, 2, 1);
	auto s2 = makeSwordsman(2, 1, 0, 2, 1);
	f.units.add(s1);
	f.units.add(s2);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);

	TurnManager tm(f.ctx);
	tm.run();

	CHECK(f.ctx.tick >= uint64_t(1));
	CHECK(f.eventLog.hasEvent("UNIT_ATTACKED"));
}

TEST(TurnManager_removes_dead_units_between_turns)
{
	GameFixture f;
	auto s1 = makeSwordsman(1, 0, 0, 100, 50);
	auto s2 = makeSwordsman(2, 1, 0, 10,  50);
	f.units.add(s1);
	f.units.add(s2);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);

	TurnManager tm(f.ctx);
	tm.run();

	CHECK_EQ(f.units.aliveCount(), std::size_t(1));
	CHECK(f.eventLog.hasEvent("UNIT_DIED"));
}
