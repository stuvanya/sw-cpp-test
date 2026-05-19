#include "TestHelpers.hpp"
#include "TestRunner.hpp"

#include <Core/MarchState.hpp>
#include <Core/Unit.hpp>
#include <Features/Actions/RangedAttackAction.hpp>

using namespace sw;
using namespace sw::test;

// agility=8, strength=5, range=5
static RangedAttackAction makeHunterAction(std::shared_ptr<MarchState> ms)
{
	return RangedAttackAction(8, 5, 5, std::move(ms));
}

TEST(RangedAttack_ranged_shot_when_no_adjacent_enemies_and_target_in_range)
{
	GameFixture f;
	auto hunter = std::make_shared<Unit>(1, 0, 0, 100);
	auto target = std::make_shared<Unit>(2, 3, 0, 50);
	f.units.add(hunter);
	f.units.add(target);
	f.map.occupy(0, 0);
	f.map.occupy(3, 0);

	auto action = makeHunterAction(std::make_shared<MarchState>());
	action.execute(*hunter, f.ctx);

	CHECK_EQ(target->hp, int32_t(42));	// 50 - 8 (agility)
	CHECK(f.eventLog.hasEvent("UNIT_ATTACKED"));
}

TEST(RangedAttack_melee_strike_when_adjacent_enemy_present)
{
	GameFixture f;
	auto hunter = std::make_shared<Unit>(1, 0, 0, 100);
	auto adj = std::make_shared<Unit>(2, 1, 0, 50);
	f.units.add(hunter);
	f.units.add(adj);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);

	auto action = makeHunterAction(std::make_shared<MarchState>());
	action.execute(*hunter, f.ctx);

	CHECK_EQ(adj->hp, int32_t(45));	 // 50 - 5 (strength)
	CHECK(f.eventLog.hasEvent("UNIT_ATTACKED"));
}

TEST(RangedAttack_adjacent_enemy_takes_priority_over_ranged_target)
{
	GameFixture f;
	auto hunter = std::make_shared<Unit>(1, 0, 0, 100);
	auto adj = std::make_shared<Unit>(2, 1, 0, 50);
	auto faraway = std::make_shared<Unit>(3, 4, 0, 50);
	f.units.add(hunter);
	f.units.add(adj);
	f.units.add(faraway);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);
	f.map.occupy(4, 0);

	auto action = makeHunterAction(std::make_shared<MarchState>());
	action.execute(*hunter, f.ctx);

	CHECK_EQ(adj->hp, int32_t(45));		 // hit with strength
	CHECK_EQ(faraway->hp, int32_t(50));	 // untouched
}

TEST(RangedAttack_target_beyond_range_is_ignored)
{
	GameFixture f(20, 20);
	auto hunter = std::make_shared<Unit>(1, 0, 0, 100);
	auto target = std::make_shared<Unit>(2, 9, 0, 50);	// distance 9 > range 5
	f.units.add(hunter);
	f.units.add(target);
	f.map.occupy(0, 0);
	f.map.occupy(9, 0);

	auto action = makeHunterAction(std::make_shared<MarchState>());
	action.execute(*hunter, f.ctx);

	CHECK_EQ(target->hp, int32_t(50));
	CHECK(!f.eventLog.hasEvent("UNIT_ATTACKED"));
}

TEST(RangedAttack_moves_when_no_targets_in_range_and_march_set)
{
	GameFixture f;
	auto hunter = std::make_shared<Unit>(1, 0, 0, 100);
	f.units.add(hunter);
	f.map.occupy(0, 0);

	auto ms = std::make_shared<MarchState>();
	ms->hasTarget = true;
	ms->targetX = 5;
	ms->targetY = 0;
	auto action = makeHunterAction(ms);
	action.execute(*hunter, f.ctx);

	CHECK(f.eventLog.hasEvent("UNIT_MOVED"));
	CHECK(!f.eventLog.hasEvent("UNIT_ATTACKED"));
}

TEST(RangedAttack_canAct_true_when_enemy_in_range)
{
	GameFixture f;
	auto hunter = std::make_shared<Unit>(1, 0, 0, 100);
	auto target = std::make_shared<Unit>(2, 3, 0, 50);
	f.units.add(hunter);
	f.units.add(target);

	auto action = makeHunterAction(std::make_shared<MarchState>());
	CHECK(action.canAct(*hunter, f.ctx));
}

TEST(RangedAttack_canAct_false_when_no_enemies_and_no_march)
{
	GameFixture f;
	auto hunter = std::make_shared<Unit>(1, 0, 0, 100);
	f.units.add(hunter);

	auto action = makeHunterAction(std::make_shared<MarchState>());
	CHECK(!action.canAct(*hunter, f.ctx));
}

TEST(RangedAttack_logs_UNIT_DIED_when_ranged_shot_kills_target)
{
	GameFixture f;
	auto hunter = std::make_shared<Unit>(1, 0, 0, 100);
	auto target = std::make_shared<Unit>(2, 3, 0, 5);  // 5 HP, agility=8 kills it
	f.units.add(hunter);
	f.units.add(target);
	f.map.occupy(0, 0);
	f.map.occupy(3, 0);

	auto action = makeHunterAction(std::make_shared<MarchState>());
	action.execute(*hunter, f.ctx);

	CHECK(!target->isAlive());
	CHECK(f.eventLog.hasEvent("UNIT_DIED"));
}
