#pragma once

#include <Core/Core.h>

#include <Actor/Actor.h>
#include <Physics/Bounds.h>

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

		// 좌표의 타일타입 게터함수
		TileType GetTile(int x, int y) const;
		// 좌표의 타일이 벽인지 체크 함수
		bool IsSolid(int x, int y) const;

		// 위치 변환 좌표
		Vector2F TileToWorld(int x, int y) const;
		void WorldToTile(const Vector2F& position, int& x, int& y) const;

		// 타일의 크기 게터
		const Vector2I GetTileSize() const { return Vector2I(tileWidth, tileHeight); }

		// 벽과 충돌 체크 함수
		bool OverlapsSolid(const Bounds& worldBounds) const;

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
		// 맵의 가로 세로길이
		int mapWidth = 0;
		int mapHeight = 0;

		// 타일 하나의 가로 세로 길이
		int tileWidth = 8;
		int tileHeight = 4;

		std::vector<TileType> tiles;

		PixelImage mapImage;
	};
}




