#include <Core/GameContext.hpp>
#include <Core/Map.hpp>
#include <Core/MarchState.hpp>
#include <Core/TurnManager.hpp>
#include <Core/UnitStorage.hpp>
#include <Features/Units/HunterUnit.hpp>
#include <Features/Units/SwordsmanUnit.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv)
{
	using namespace sw;

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <scenario_file>\n";
		return 1;
	}

	std::ifstream file(argv[1]);
	if (!file)
	{
		std::cerr << "Error: File not found - " << argv[1] << "\n";
		return 1;
	}

	Map map;
	UnitStorage units;
	EventLog eventLog;
	GameContext ctx{map, units, eventLog};

	io::CommandParser parser;

	parser.add<io::CreateMap>(
		[&](auto cmd)
		{
			map = Map(cmd.width, cmd.height);
			eventLog.log(ctx.tick, io::MapCreated{cmd.width, cmd.height});
		});

	parser.add<io::SpawnSwordsman>(
		[&](auto cmd)
		{
			if (!map.isInBounds(cmd.x, cmd.y))
			{
				throw std::runtime_error(
					"SPAWN_SWORDSMAN: coordinates (" + std::to_string(cmd.x) + "," + std::to_string(cmd.y)
					+ ") are out of map bounds");
			}
			if (map.isOccupied(cmd.x, cmd.y))
			{
				throw std::runtime_error(
					"SPAWN_SWORDSMAN: cell (" + std::to_string(cmd.x) + "," + std::to_string(cmd.y)
					+ ") is already occupied");
			}

			auto unit = makeSwordsman(cmd.unitId, cmd.x, cmd.y, cmd.hp, cmd.strength);
			units.add(unit);
			if (unit->occupiesCell)
			{
				map.occupy(cmd.x, cmd.y);
			}
			eventLog.log(ctx.tick, io::UnitSpawned{cmd.unitId, "Swordsman", cmd.x, cmd.y});
		});

	parser.add<io::SpawnHunter>(
		[&](auto cmd)
		{
			if (!map.isInBounds(cmd.x, cmd.y))
			{
				throw std::runtime_error(
					"SPAWN_HUNTER: coordinates (" + std::to_string(cmd.x) + "," + std::to_string(cmd.y)
					+ ") are out of map bounds");
			}
			if (map.isOccupied(cmd.x, cmd.y))
			{
				throw std::runtime_error(
					"SPAWN_HUNTER: cell (" + std::to_string(cmd.x) + "," + std::to_string(cmd.y)
					+ ") is already occupied");
			}

			auto unit = makeHunter(cmd.unitId, cmd.x, cmd.y, cmd.hp, cmd.agility, cmd.strength, cmd.range);
			units.add(unit);
			if (unit->occupiesCell)
			{
				map.occupy(cmd.x, cmd.y);
			}
			eventLog.log(ctx.tick, io::UnitSpawned{cmd.unitId, "Hunter", cmd.x, cmd.y});
		});

	parser.add<io::March>(
		[&](auto cmd)
		{
			auto unit = units.findById(cmd.unitId);
			if (!unit)
			{
				throw std::runtime_error("MARCH: unknown unit id " + std::to_string(cmd.unitId));
			}

			// Only movable units can receive march orders; static units silently ignore MARCH.
			if (auto marchState = unit->components.template get<MarchState>())
			{
				marchState->targetX = cmd.targetX;
				marchState->targetY = cmd.targetY;
				marchState->hasTarget = true;
				eventLog.log(ctx.tick, io::MarchStarted{cmd.unitId, unit->x, unit->y, cmd.targetX, cmd.targetY});
			}
		});

	parser.parse(file);

	TurnManager(ctx).run();

	return 0;
}
