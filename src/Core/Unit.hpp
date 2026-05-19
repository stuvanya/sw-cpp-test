#pragma once

#include <IO/System/TypeRegistry.hpp>
#include <cstdint>

namespace sw
{
	// Represents a single unit on the map.
	// Type-specific behavior is stored in `components` via TypeRegistry.
	struct Unit
	{
		uint32_t id{0};
		uint32_t x{0};
		uint32_t y{0};
		int32_t hp{0};			  // signed: can go negative after damage
		bool occupiesCell{true};  // false for flying/invisible units
		bool targetable{true};	  // false for units that cannot be attacked
		TypeRegistry components;

		Unit(uint32_t id, uint32_t x, uint32_t y, int32_t hp) :
				id(id),
				x(x),
				y(y),
				hp(hp)
		{}

		bool isAlive() const
		{
			return hp > 0;
		}
	};
}
