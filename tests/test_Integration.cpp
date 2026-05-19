#include "TestHelpers.hpp"
#include "TestRunner.hpp"

#include <Core/MarchState.hpp>
#include <Core/TurnManager.hpp>
#include <Features/Units/HunterUnit.hpp>
#include <Features/Units/SwordsmanUnit.hpp>

using namespace sw;
using namespace sw::test;

// ---------------------------------------------------------------------------
// Scenario A: swordsman kills swordsman in one hit
// ---------------------------------------------------------------------------
TEST(Integration_swordsman_kills_adjacent_swordsman_in_one_hit)
{
	// s1 has strength 100 — kills s2 (hp=10) in a single blow
	GameFixture f;
	auto s1 = makeSwordsman(1, 0, 0, 100, 100);
	auto s2 = makeSwordsman(2, 1, 0, 10, 10);
	f.units.add(s1);
	f.units.add(s2);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);

	TurnManager(f.ctx).run();

	CHECK_EQ(f.units.aliveCount(), std::size_t(1));
	CHECK(f.eventLog.hasEvent("UNIT_ATTACKED"));
	CHECK(f.eventLog.hasEvent("UNIT_DIED"));
	CHECK(f.ctx.tick >= uint64_t(1));
}

// ---------------------------------------------------------------------------
// Scenario B: hunter shoots from range, no adjacent enemies
// ---------------------------------------------------------------------------
TEST(Integration_hunter_fires_ranged_shot_at_distant_enemy)
{
	// Hunter at (0,0), enemy swordsman at (4,0) — distance 4, within range 5.
	// Hunter agility=20 kills swordsman (hp=10) in one shot.
	GameFixture f(20, 20);

	// Hunter at (0,0), enemy swordsman at (4,0) — distance 4, within range 5
	// Hunter agility=20 kills swordsman (hp=10) in one shot
	auto hunter = makeHunter(1, 0, 0, 100, 20, 5, 5);
	auto swordsman = makeSwordsman(2, 4, 0, 10, 5);
	f.units.add(hunter);
	f.units.add(swordsman);
	f.map.occupy(0, 0);
	f.map.occupy(4, 0);

	TurnManager(f.ctx).run();

	CHECK_EQ(f.units.aliveCount(), std::size_t(1));
	CHECK(f.eventLog.hasEvent("UNIT_ATTACKED"));
	CHECK(f.eventLog.hasEvent("UNIT_DIED"));
}

// ---------------------------------------------------------------------------
// Scenario C: swordsman marches toward enemy and then attacks
// ---------------------------------------------------------------------------
TEST(Integration_swordsman_marches_to_enemy_and_attacks)
{
	// s1 at (0,0), s2 at (5,0) — not adjacent initially.
	// Give s1 a march target at (5,0) so it walks toward s2.
	GameFixture f(20, 20);

	// s1 at (0,0), s2 at (5,0) — not adjacent initially
	// Give s1 a march target at (5,0) so it walks toward s2
	auto s1 = makeSwordsman(1, 0, 0, 100, 50);
	auto s2 = makeSwordsman(2, 5, 0, 10, 5);
	f.units.add(s1);
	f.units.add(s2);
	f.map.occupy(0, 0);
	f.map.occupy(5, 0);

	// Set march target for s1
	auto marchState = s1->components.get<MarchState>();
	marchState->hasTarget = true;
	marchState->targetX = 5;
	marchState->targetY = 0;

	TurnManager(f.ctx).run();

	// s1 should have moved and eventually killed s2
	CHECK(f.eventLog.hasEvent("UNIT_MOVED"));
	CHECK(f.eventLog.hasEvent("UNIT_ATTACKED"));
	CHECK_EQ(f.units.aliveCount(), std::size_t(1));
}

// ---------------------------------------------------------------------------
// Scenario D: hunter switches from ranged to melee when enemy closes in
// ---------------------------------------------------------------------------
TEST(Integration_hunter_uses_melee_when_enemy_is_adjacent)
{
	// Hunter at (0,0), enemy at (1,0) — adjacent, so hunter uses melee (strength).
	// strength=5, enemy hp=3 → dies in one hit.
	GameFixture f;

	// Hunter at (0,0), enemy at (1,0) — adjacent, so hunter uses melee (strength)
	// strength=5, enemy hp=3 → dies in one hit
	auto hunter = makeHunter(1, 0, 0, 100, 20, 5, 5);
	auto enemy = makeSwordsman(2, 1, 0, 3, 1);
	f.units.add(hunter);
	f.units.add(enemy);
	f.map.occupy(0, 0);
	f.map.occupy(1, 0);

	TurnManager(f.ctx).run();

	CHECK_EQ(f.units.aliveCount(), std::size_t(1));
	CHECK(f.eventLog.hasEvent("UNIT_ATTACKED"));
	CHECK(f.eventLog.hasEvent("UNIT_DIED"));
}
