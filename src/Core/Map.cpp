#include "Map.hpp"

#include <cmath>

namespace sw
{
	Map::Map(uint32_t width, uint32_t height) :
			_width(width),
			_height(height)
	{}

	bool Map::isInBounds(uint32_t x, uint32_t y) const
	{
		return x < _width && y < _height;
	}

	bool Map::isOccupied(uint32_t x, uint32_t y) const
	{
		return _occupied.count({x, y}) > 0;
	}

	void Map::occupy(uint32_t x, uint32_t y)
	{
		_occupied.insert({x, y});
	}

	void Map::free(uint32_t x, uint32_t y)
	{
		_occupied.erase({x, y});
	}

	std::vector<std::pair<uint32_t, uint32_t>> Map::getFreeNeighbors(uint32_t x, uint32_t y) const
	{
		std::vector<std::pair<uint32_t, uint32_t>> result;

		// Check all 8 directions around (x, y)
		for (int dx = -1; dx <= 1; ++dx)
		{
			for (int dy = -1; dy <= 1; ++dy)
			{
				if (dx == 0 && dy == 0)
				{
					continue;
				}

				// Use signed arithmetic to avoid underflow on uint32_t
				int nx = static_cast<int>(x) + dx;
				int ny = static_cast<int>(y) + dy;

				if (nx < 0 || ny < 0)
				{
					continue;
				}

				uint32_t ux = static_cast<uint32_t>(nx);
				uint32_t uy = static_cast<uint32_t>(ny);

				if (isInBounds(ux, uy) && !isOccupied(ux, uy))
				{
					result.push_back({ux, uy});
				}
			}
		}

		return result;
	}

	uint32_t Map::distance(uint32_t ax, uint32_t ay, uint32_t bx, uint32_t by)
	{
		uint32_t dx = (ax > bx) ? (ax - bx) : (bx - ax);
		uint32_t dy = (ay > by) ? (ay - by) : (by - ay);
		return std::max(dx, dy);
	}

	bool Map::isNeighbor(uint32_t ax, uint32_t ay, uint32_t bx, uint32_t by)
	{
		return distance(ax, ay, bx, by) == 1;
	}
}
