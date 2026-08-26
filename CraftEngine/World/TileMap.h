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
		// 벽이나 물 색 변경
		void BuildVisualImage();

		// 바다 색 게터
		BackgroundColor GetWaterColor(int worldX, int worldY, int worldHeight);

		// 바다 색 칠하기
		void DrawWaterColor();

		// 암초 색 칠하기
		void DrawSubmergedRock();

	private:
		int mapWidth = 0;
		int mapHeight = 0;

		int tileWidth = 8;
		int tileHeight = 4;

		std::vector<TileType> tiles;

		PixelImage mapImage;
	};
}




