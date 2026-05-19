#include "TestRunner.hpp"

#include <Core/Map.hpp>

using namespace sw;

TEST(Map_isInBounds_true_for_valid_coordinates)
{
	Map m(5, 5);
	CHECK(m.isInBounds(0, 0));
	CHECK(m.isInBounds(4, 4));
	CHECK(m.isInBounds(2, 3));
}

TEST(Map_isInBounds_false_outside_map)
{
	Map m(5, 5);
	CHECK(!m.isInBounds(5, 0));
	CHECK(!m.isInBounds(0, 5));
	CHECK(!m.isInBounds(5, 5));
}

TEST(Map_occupy_marks_cell_as_occupied)
{
	Map m(5, 5);
	CHECK(!m.isOccupied(2, 2));
	m.occupy(2, 2);
	CHECK(m.isOccupied(2, 2));
}

TEST(Map_free_unmarks_occupied_cell)
{
	Map m(5, 5);
	m.occupy(1, 1);
	m.free(1, 1);
	CHECK(!m.isOccupied(1, 1));
}

TEST(Map_getFreeNeighbors_excludes_occupied_cells)
{
	Map m(5, 5);
	m.occupy(0, 0);
	m.occupy(1, 0);
	m.occupy(2, 0);
	m.occupy(0, 1);
	m.occupy(2, 1);
	m.occupy(0, 2);
	m.occupy(1, 2);
	auto neighbors = m.getFreeNeighbors(1, 1);
	CHECK_EQ(neighbors.size(), std::size_t(1));
	CHECK_EQ(neighbors[0].first, uint32_t(2));
	CHECK_EQ(neighbors[0].second, uint32_t(2));
}

TEST(Map_getFreeNeighbors_at_corner_returns_at_most_3)
{
	Map m(5, 5);
	auto neighbors = m.getFreeNeighbors(0, 0);
	CHECK(neighbors.size() <= 3);
	CHECK(neighbors.size() >= 1);
}

TEST(Map_getFreeNeighbors_empty_when_all_neighbors_occupied)
{
	Map m(3, 3);
	for (int x = 0; x < 3; ++x)
	{
		for (int y = 0; y < 3; ++y)
		{
			if (!(x == 1 && y == 1))
			{
				m.occupy(x, y);
			}
		}
	}
	auto neighbors = m.getFreeNeighbors(1, 1);
	CHECK(neighbors.empty());
}

TEST(Map_distance_Chebyshev_same_cell_is_zero)
{
	CHECK_EQ(Map::distance(3, 3, 3, 3), uint32_t(0));
}

TEST(Map_distance_Chebyshev_diagonal)
{
	CHECK_EQ(Map::distance(0, 0, 3, 4), uint32_t(4));
}

TEST(Map_distance_Chebyshev_horizontal)
{
	CHECK_EQ(Map::distance(0, 0, 5, 0), uint32_t(5));
}

TEST(Map_isNeighbor_true_for_adjacent_cells)
{
	CHECK(Map::isNeighbor(0, 0, 1, 0));
	CHECK(Map::isNeighbor(0, 0, 1, 1));
	CHECK(Map::isNeighbor(2, 2, 1, 1));
}

TEST(Map_isNeighbor_false_for_non_adjacent_cells)
{
	CHECK(!Map::isNeighbor(0, 0, 2, 0));
	CHECK(!Map::isNeighbor(0, 0, 0, 0));
}
