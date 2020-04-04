#pragma once

namespace TT {
	enum TileType
	{
		NONE,
		NONE_WITH_SHADOW,
		WEAK_WALL,
		DOUBLE_WEAK_WALL,
		INDESTRUCTIBLE_WALL,
		DOUBLE_INDESTRUCTIBLE_WALL,
		HALF_INDESTRUCTIBLE_WALL,
		BOMB,
		COUNT
	};

	enum class TileState
	{
		NONE = 0,
		AIPLAYER = 1,
		PLAYER = 2,
		BOMB = 4,
		DANGER = 8,
		PLACE_FOR_BOMB = 16
	};

	struct AITileType {
		TileType tileType;
		TileState tileState;
	};
}
