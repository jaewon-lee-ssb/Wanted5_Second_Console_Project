#pragma once

#include <Core/Core.h>

#include <Actor/Actor.h>

#include <cstdint>
#include <string>
#include <vector>

namespace Craft
{
	enum class TileType : uint8_t
	{
		Empty,
		Wall
	};

	class CRAFT_API TileMap : public Actor
	{
		TYPE_DECLARATIONS(TileMap, Actor)

	public:
		TileMap();
		~TileMap() = default;

		bool Load(const std::string& filename);

		TileType GetTile(int x, int y) const;
		bool IsSolid(int x, int y) const;

		Vector2F TileToWorld(int x, int y) const;
		void WorldToTile(const Vector2F& position, int& x, int& y) const;

		virtual void Draw() override;

	private:
		int mapWidth = 0;
		int mapHeight = 0;

		int tileWidth = 8;
		int tileHeight = 4;

		std::vector<TileType> tiles;

		PixelImage mapImage;
	};
}




