#include "TestHelpers.hpp"
#include "TestRunner.hpp"

#include <Core/MarchState.hpp>
#include <Core/Unit.hpp>
#include <Features/Actions/MoveAction.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/UnitMoved.hpp>

using namespace sw;
using namespace sw::test;

TEST(MoveAction_canAct_false_when_no_march_target)
{
	GameFixture f;
	auto marchState = std::make_shared<MarchState>();
	MoveAction action(marchState);

	auto unit = std::make_shared<Unit>(1, 0, 0, 10);
	f.units.add(unit);
	f.map.occupy(0, 0);

	CHECK(!action.canAct(*unit, f.ctx));
}

TEST(MoveAction_canAct_true_when_target_set_and_not_reached)
{
	GameFixture f;
	auto marchState = std::make_shared<MarchState>();
	marchState->hasTarget = true;
	marchState->targetX = 5;
	marchState->targetY = 5;
	MoveAction action(marchState);

	auto unit = std::make_shared<Unit>(1, 0, 0, 10);
	f.units.add(unit);
	f.map.occupy(0, 0);

	CHECK(action.canAct(*unit, f.ctx));
}

TEST(MoveAction_canAct_false_when_already_at_target)
{
	GameFixture f;
	auto marchState = std::make_shared<MarchState>();
	marchState->hasTarget = true;
	marchState->targetX = 3;
	marchState->targetY = 3;
	MoveAction action(marchState);

	auto unit = std::make_shared<Unit>(1, 3, 3, 10);
	f.units.add(unit);
	f.map.occupy(3, 3);

	CHECK(!action.canAct(*unit, f.ctx));
}

TEST(MoveAction_unit_moves_one_step_closer_to_target)
{
	GameFixture f;
	auto marchState = std::make_shared<MarchState>();
	marchState->hasTarget = true;
	marchState->targetX = 5;
	marchState->targetY = 0;
	MoveAction action(marchState);

	auto unit = std::make_shared<Unit>(1, 0, 0, 10);
	f.units.add(unit);
	f.map.occupy(0, 0);

	action.execute(*unit, f.ctx);

	CHECK_EQ(unit->x, uint32_t(1));
	CHECK_EQ(unit->y, uint32_t(0));
	CHECK(f.eventLog.hasEvent<io::UnitMoved>());
}

TEST(MoveAction_reaching_target_logs_MARCH_ENDED_and_clears_hasTarget)
{
	GameFixture f;
	auto marchState = std::make_shared<MarchState>();
	marchState->hasTarget = true;
	marchState->targetX = 1;
	marchState->targetY = 0;
	MoveAction action(marchState);

	auto unit = std::make_shared<Unit>(1, 0, 0, 10);
	f.units.add(unit);
	f.map.occupy(0, 0);

	action.execute(*unit, f.ctx);

	CHECK_EQ(unit->x, uint32_t(1));
	CHECK_EQ(unit->y, uint32_t(0));
	CHECK(!marchState->hasTarget);
	CHECK(f.eventLog.hasEvent<io::MarchEnded>());
}

TEST(MoveAction_blocked_unit_does_not_move_and_keeps_target)
{
	GameFixture f;
	for (int x = 0; x <= 2; ++x)
	{
		for (int y = 0; y <= 2; ++y)
		{
			if (!(x == 1 && y == 1))
			{
				f.map.occupy(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
			}
		}
	}

	auto marchState = std::make_shared<MarchState>();
	marchState->hasTarget = true;
	marchState->targetX = 5;
	marchState->targetY = 5;
	MoveAction action(marchState);

	auto unit = std::make_shared<Unit>(1, 1, 1, 10);
	f.units.add(unit);
	f.map.occupy(1, 1);

	action.execute(*unit, f.ctx);

	CHECK_EQ(unit->x, uint32_t(1));
	CHECK_EQ(unit->y, uint32_t(1));
	CHECK(marchState->hasTarget);
	CHECK(!f.eventLog.hasEvent<io::UnitMoved>());
}
