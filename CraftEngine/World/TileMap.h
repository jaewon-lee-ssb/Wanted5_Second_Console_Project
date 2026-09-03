#pragma once

#include <Core/Core.h>

#include <Actor/Actor.h>
#include <Physics/Bounds.h>

#include <cstdint>
#include <string>
#include <vector>

namespace Craft
{
	struct OpenNode
	{
		int index = -1;
		int gCost = 0;
		int fCost = 0;
	};

	struct CompareOpenNode
	{
		bool operator()(const OpenNode& lhs, const OpenNode& rhs) const
		{
			// priority_queue는 기본적으로 큰 값부터 꺼내므로
			// 비교 방향을 반대로 설정
			return lhs.fCost > rhs.fCost;
		}
	};

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
		bool IsBlocked(int x, int y) const;

		// 위치 변환 좌표
		Vector2F TileToWorld(int x, int y) const;
		void WorldToTile(const Vector2F& position, int& x, int& y) const;

		// 맵크기 반환함수
		const Craft::Vector2F GetMapSize() const { return Craft::Vector2F(static_cast<float>(image.width), static_cast<float>(image.height)); }

		// 타일의 크기 게터
		const Vector2I GetTileSize() const { return Vector2I(tileWidth, tileHeight); }

		// 벽과 충돌 체크 함수
		bool CanOccupyWorld(const Bounds& worldBounds) const;

		// 경로 찾는 함수.
		std::vector<Vector2I> FindPath(const Vector2F& startWorldPosition, const Vector2F& goalWorldPosition, float imageWidth, float imageHeight) const;

		// A* 경로 디버그 표시
		void SetPathDebugEnabled(bool enabled);
		void QueueDebugPath(const std::vector<Vector2I>& path, size_t startIndex = 0);

	private:

		// 그리기함수
		virtual void Draw() override;

		// 벽이나 물 색 변경
		void BuildVisualImage();

		// 바다 색 게터
		BackgroundColor GetWaterColor(int worldX, int worldY, int worldHeight);

		// 깊이에 따른 물 색 칠하기
		void DrawWaterColor();

		// 암초 색 칠하기
		void DrawSubmergedRock();

		// A* 경로 디버그용 타일 이미지 생성
		void BuildPathDebugImage();

		// 해당 타일에 액터가 들어갈 수 있는지 검사.
		bool CanOccupyTile(int tileX, int tileY, float actorWidth, float actorHeight) const;

	private:
		// 맵의 가로 세로길이
		int tilemapWidth = 0;
		int tilemapHeight = 0;

		// 타일 하나의 가로 세로 길이
		int tileWidth = 8;
		int tileHeight = 4;

		// 타일의 정보가 저장되는 배열
		std::vector<TileType> tiles;

		// 타일맵의 이미지
		PixelImage mapImage;

		// A* 경로 디버그 표시
		bool isPathDebugEnabled = false;
		std::vector<Vector2I> debugPathTiles;
		PixelImage debugPathTileImage;
	};
}




