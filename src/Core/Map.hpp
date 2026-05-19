#pragma once

#include <cstdint>
#include <set>
#include <utility>
#include <vector>

namespace sw
{
	// Represents the game map. Tracks size and which cells are occupied.
	class Map
	{
	public:
		Map() = default;
		Map(uint32_t width, uint32_t height);

		bool isInBounds(uint32_t x, uint32_t y) const;
		bool isOccupied(uint32_t x, uint32_t y) const;

		void occupy(uint32_t x, uint32_t y);
		void free(uint32_t x, uint32_t y);

		// Returns all free neighboring cells (up to 8 directions).
		std::vector<std::pair<uint32_t, uint32_t>> getFreeNeighbors(uint32_t x, uint32_t y) const;

		uint32_t width() const
		{
			return _width;
		}

		uint32_t height() const
		{
			return _height;
		}

		// Chebyshev distance between two points.
		static uint32_t distance(uint32_t ax, uint32_t ay, uint32_t bx, uint32_t by);

		// Returns true if two points are Chebyshev-adjacent (distance == 1).
		static bool isNeighbor(uint32_t ax, uint32_t ay, uint32_t bx, uint32_t by);

	private:
		uint32_t _width{0};
		uint32_t _height{0};
		std::set<std::pair<uint32_t, uint32_t>> _occupied;
	};
}
