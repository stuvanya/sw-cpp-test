#include "TestHelpers.hpp"
#include "TestRunner.hpp"

#include <Core/MarchState.hpp>
#include <Core/Unit.hpp>
#include <Features/Actions/MeleeAttackAction.hpp>

using namespace sw;
using namespace sw::test;

TEST(MeleeAttack_attacks_adjacent_enemy_and_reduces_HP)
{
	GameFixture f;
	auto attacker = std::make_shared<Unit>(1, 0, 0, 100);
	auto target = std::make_shared<Unit>(2, 1, 0, 50);
	f.units.add(attacker);
	f.units.add(target);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);

	MeleeAttackAction action(10, std::make_shared<MarchState>());
	action.execute(*attacker, f.ctx);

	CHECK_EQ(target->hp, int32_t(40));
	CHECK(f.eventLog.hasEvent("UNIT_ATTACKED"));
}

TEST(MeleeAttack_logs_UNIT_DIED_when_target_HP_drops_to_zero)
{
	GameFixture f;
	auto attacker = std::make_shared<Unit>(1, 0, 0, 100);
	auto target = std::make_shared<Unit>(2, 1, 0, 5);
	f.units.add(attacker);
	f.units.add(target);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);

	MeleeAttackAction action(10, std::make_shared<MarchState>());
	action.execute(*attacker, f.ctx);

	CHECK(!target->isAlive());
	CHECK(f.eventLog.hasEvent("UNIT_DIED"));
}

TEST(MeleeAttack_does_not_attack_non_targetable_unit)
{
	GameFixture f;
	auto attacker = std::make_shared<Unit>(1, 0, 0, 100);
	auto target = std::make_shared<Unit>(2, 1, 0, 50);
	target->targetable = false;
	f.units.add(attacker);
	f.units.add(target);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);

	MeleeAttackAction action(10, std::make_shared<MarchState>());
	action.execute(*attacker, f.ctx);

	CHECK_EQ(target->hp, int32_t(50));
	CHECK(!f.eventLog.hasEvent("UNIT_ATTACKED"));
}

TEST(MeleeAttack_moves_toward_march_target_when_no_adjacent_enemies)
{
	GameFixture f;
	auto attacker = std::make_shared<Unit>(1, 0, 0, 100);
	f.units.add(attacker);
	f.map.occupy(0, 0);

	auto ms = std::make_shared<MarchState>();
	ms->hasTarget = true;
	ms->targetX = 5;
	ms->targetY = 0;
	MeleeAttackAction action(10, ms);
	action.execute(*attacker, f.ctx);

	CHECK(f.eventLog.hasEvent("UNIT_MOVED"));
	CHECK(!f.eventLog.hasEvent("UNIT_ATTACKED"));
}

TEST(MeleeAttack_canAct_true_when_adjacent_enemy_exists)
{
	GameFixture f;
	auto attacker = std::make_shared<Unit>(1, 0, 0, 100);
	auto enemy = std::make_shared<Unit>(2, 1, 0, 50);
	f.units.add(attacker);
	f.units.add(enemy);

	MeleeAttackAction action(10, std::make_shared<MarchState>());
	CHECK(action.canAct(*attacker, f.ctx));
}

TEST(MeleeAttack_canAct_false_with_no_enemies_and_no_march)
{
	GameFixture f;
	auto attacker = std::make_shared<Unit>(1, 0, 0, 100);
	f.units.add(attacker);

	MeleeAttackAction action(10, std::make_shared<MarchState>());
	CHECK(!action.canAct(*attacker, f.ctx));
}

TEST(MeleeAttack_canAct_true_when_march_target_set)
{
	GameFixture f;
	auto attacker = std::make_shared<Unit>(1, 0, 0, 100);
	f.units.add(attacker);

	auto ms = std::make_shared<MarchState>();
	ms->hasTarget = true;
	ms->targetX = 5;
	ms->targetY = 5;
	MeleeAttackAction action(10, ms);
	CHECK(action.canAct(*attacker, f.ctx));
}
