#include <World/TileMap.h>
#include <Camera/Camera.h>
#include <Level/Level.h>
#include <Render/Renderer.h>
#include <Navigation/AStar.h>


#include <fstream>
#include <cassert>
#include <algorithm>


namespace Craft
{
	TileMap::TileMap()
		: super(PixelImage{}, Vector2F::Zero)
	{
	}

	bool TileMap::Load(const std::string& filename)
	{
		// 데이터를 저장할 임시 변수
		int loadedWidth = 0;
		int loadedHeight = 0;
		std::vector<TileType> loadedTiles;
		
		// 최종 경로
		std::string path = std::string("../Contents/") + filename;

		// 파일 열기 및 확인
		std::ifstream file(path, std::ios_base::binary);
		assert(file.is_open() && "map file not open");

		if (!file.is_open())
		{
			return false;
		}

		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty())
			{
				return false;
			}

			// \r 개행 문자 삭제
			if (!line.empty() && line.back() == '\r')
			{
				line.pop_back();
			}

			// 맵 가로길이가 같은지 확인
			if (loadedWidth == 0)
			{
				loadedWidth = static_cast<int>(line.size());
			}
			else if (line.size() != loadedWidth)
			{
				return false;
			}

			for (char character : line)
			{
				switch (character)
				{
				case '.':
					loadedTiles.emplace_back(TileType::Empty);
					break;

				case '#':
					loadedTiles.emplace_back(TileType::Wall);
					break;

				default:
					return false;
				}
			}

			++loadedHeight;
		}

		// 가로 세로 길이가 0보다 작거나 타일이 비어있으면 실패
		if (loadedWidth <= 0 ||	loadedHeight <= 0 || loadedTiles.empty())
		{
			return false;
		}

		// 실제 타일 갯수가 가로 x 세로가 맞는지 체크
		const size_t expectedTileCount = static_cast<size_t>(loadedWidth) *	static_cast<size_t>(loadedHeight);
		if (loadedTiles.size() != expectedTileCount)
		{
			return false;
		}

		// 여기까지 왔다면 제대로 파일을 읽었다는 것
		tilemapWidth = loadedWidth;
		tilemapHeight = loadedHeight;
		tiles = std::move(loadedTiles);


		// 실제 맵 크기는 타일맵길이에 1타일의 길이도 곱해준다
		mapImage.width = tilemapWidth * tileWidth;
		mapImage.height = tilemapHeight * tileHeight;
		mapImage.pixels.assign(static_cast<size_t>(mapImage.width) * static_cast<size_t>(mapImage.height), Pixel{});

		BuildVisualImage();
		BuildPathDebugImage();


		file.close();

		return true;

	}

	TileType TileMap::GetTile(int x, int y) const
	{
		if (x < 0 || x >= tilemapWidth || y < 0 || y >= tilemapHeight)
		{
			return TileType::Wall;
		}

		return tiles[y * tilemapWidth + x];
	}

	bool TileMap::IsBlocked(int x, int y) const
	{
		return GetTile(x, y) == TileType::Wall;
	}

	Vector2F TileMap::TileToWorld(int x, int y) const
	{
		return GetPosition() + Vector2F(static_cast<float>(x * tileWidth), static_cast<float>(y * tileHeight));
	}

	void TileMap::WorldToTile(const Vector2F& worldPosition, int& outX, int& outY) const
	{
		const Vector2F localPosition = worldPosition - GetPosition();

		outX = static_cast<int>(std::floor(localPosition.x / tileWidth));

		outY = static_cast<int>(std::floor(localPosition.y / tileHeight));
	}

	bool TileMap::CanOccupyWorld(const Bounds& worldBounds) const
	{
		// 월드 좌표에서 타일맵 위치를 빼서 타일맵 내부 로컬 좌표로 변환
		const float localLeft = worldBounds.left - GetPosition().x;
		const float localRight = worldBounds.right - GetPosition().x;
		const float localTop = worldBounds.top - GetPosition().y;
		const float localBottom = worldBounds.bottom - GetPosition().y;

		// Bounds가 걸치는 타일 좌표 범위 계산
		const int leftTile = static_cast<int>(std::floor(localLeft / tileWidth));
		const int rightTile = static_cast<int>(std::ceil(localRight / tileWidth)) - 1;
		const int topTile = static_cast<int>(std::floor(localTop / tileHeight));
		const int bottomTile = static_cast<int>(std::ceil(localBottom / tileHeight)) - 1;

		// 해당 범위에 벽이 하나라도 있으면 충돌
		for (int tileY = topTile; tileY <= bottomTile; ++tileY)
		{
			for (int tileX = leftTile; tileX <= rightTile; ++tileX)
			{
				if (IsBlocked(tileX, tileY))
				{
					// 벽이 있다
					return false;
				}
			}
		}

		// 벽이 없다
		return true;
	}

	std::vector<Vector2I> TileMap::FindPath(const Vector2F& startWorldPosition, const Vector2F& goalWorldPosition, float actorWidth, float actorHeight) const
	{
		int startX = 0;
		int startY = 0;
		int goalX = 0;
		int goalY = 0;

		// 액터가 현재 위치한 곳 검사
		const Bounds startBounds{
			startWorldPosition.x - actorWidth * 0.5f,
			startWorldPosition.x + actorWidth * 0.5f,
			startWorldPosition.y - actorHeight * 0.5f,
			startWorldPosition.y + actorHeight * 0.5f
		};

		if (CanOccupyWorld(startBounds))
		{
			return {};
		}

		// 목표지점 검사
		WorldToTile(startWorldPosition, startX, startY);
		WorldToTile(goalWorldPosition, goalX, goalY);

		if (!CanOccupyTile(goalX, goalY, actorWidth, actorHeight))
		{
			return {};
		}

		// 예외처리 빠져나왔으면 
		auto canMove = [this, actorWidth, actorHeight](int x, int y)
			{
				return CanOccupyTile(x, y, actorWidth, actorHeight);
			};

		// 경로 검사는 AStar한테 맡김
		AStar pathFinder;

		auto path = pathFinder.FindPath(Vector2I(startX, startY), Vector2I(goalX, goalY), tilemapWidth, tilemapHeight, canMove);

		if (!path.empty())
		{
			path.erase(path.begin());
		}


		return path;
	}

	void TileMap::SetPathDebugEnabled(bool enabled)
	{
		isPathDebugEnabled = enabled;

		if (!isPathDebugEnabled)
		{
			debugPathTiles.clear();
		}
	}

	void TileMap::QueueDebugPath(const std::vector<Vector2I>& path, size_t startIndex)
	{
		if (!isPathDebugEnabled || startIndex >= path.size())
		{
			return;
		}

		debugPathTiles.insert(debugPathTiles.end(), path.begin() + startIndex, path.end());
	}

	void TileMap::Draw()
	{
		super::Draw();

		if (!isPathDebugEnabled || debugPathTiles.empty())
		{
			debugPathTiles.clear();
			return;
		}

		auto owner = GetOwner();

		if (!owner || !owner->GetCamera())
		{
			debugPathTiles.clear();
			return;
		}

		for (const Vector2I& tile : debugPathTiles)
		{
			const Vector2F screenPosition = owner->GetCamera()->WorldToScreen(TileToWorld(tile.x, tile.y));

			Renderer::Get().SubmitWorld(
				debugPathTileImage,
				screenPosition,
				false,
				Vector2F::Zero,
				sortingOrder);
		}

		// 매 프레임 적들이 현재 경로를 다시 등록하도록 큐를 비운다.
		debugPathTiles.clear();
	}

	void TileMap::BuildVisualImage()
	{
		// 바다 먼저 그리고
		DrawWaterColor();


		// 암초 위에 그리기
		DrawSubmergedRock();
		

		// 이미지를 만든 이미지로 교체해준다
		ChangeImage(mapImage);
		pivot = Vector2F::Zero;
	}

	BackgroundColor TileMap::GetWaterColor(int worldX, int worldY, int worldHeight)
	{
		const float depth =	static_cast<float>(worldY) / static_cast<float>(worldHeight); 

		const int pattern = (worldX * 17 + worldY * 31) % 97;

		if (pattern == 0)
		{
			return BackgroundColor::LightCyan;
		}

		if (depth < 0.55f)
		{
			return BackgroundColor::Cyan;
		}

		if (depth < 0.85f)
		{
			return BackgroundColor::Blue;
		}

		return BackgroundColor::Blue;
	}

	void TileMap::DrawWaterColor()
	{
		for (int pixelY = 0; pixelY < mapImage.height; ++pixelY)
		{
			for (int pixelX = 0; pixelX < mapImage.width; ++pixelX)
			{
				Pixel& pixel = mapImage.pixels[pixelY * mapImage.width + pixelX];

				pixel.transparent = false;

				pixel.color = GetWaterColor(pixelX, pixelY, mapImage.height);
			}
		}
	}

	void TileMap::DrawSubmergedRock()
	{
		// 타일
		for (int tileY = 0; tileY < tilemapHeight; ++tileY)
		{
			for (int tileX = 0; tileX < tilemapWidth; ++tileX)
			{
				if (!IsBlocked(tileX, tileY))
				{
					continue;
				}

				const bool exposedUp = !IsBlocked(tileX, tileY - 1);
				const bool exposedDown = !IsBlocked(tileX, tileY + 1);
				const bool exposedLeft = !IsBlocked(tileX - 1, tileY);
				const bool exposedRight = !IsBlocked(tileX + 1, tileY);

				for (int localY = 0; localY < tileHeight; ++localY)
				{
					for (int localX = 0; localX < tileWidth; ++localX)
					{
						const int pixelX = tileX * tileWidth + localX;
						const int pixelY = tileY * tileHeight + localY;

						Pixel& pixel = mapImage.pixels[pixelY * mapImage.width + pixelX];
						pixel.transparent = false;
						BackgroundColor color = BackgroundColor::LightGray;

						const bool isSandSurface = exposedUp && localY == 0;
						const bool isShadowEdge =
							(exposedDown && localY == tileHeight - 1) ||
							(exposedRight && localX == tileWidth - 1);
						const bool isLightEdge = exposedLeft && localX == 0;

						// 바닥 윗면은 모래색, 암석의 아래와 오른쪽은 짙은 회색 그림자로 표현
						if (isSandSurface)
						{
							color = BackgroundColor::Brown;
						}
						else if (isShadowEdge)
						{
							color = BackgroundColor::DarkGray;
						}
						else if (isLightEdge)
						{
							color = BackgroundColor::White;
						}

						const bool isBoundary =
							(exposedUp && localY == 0) ||
							(exposedDown && localY == tileHeight - 1) ||
							(exposedLeft && localX == 0) ||
							(exposedRight && localX == tileWidth - 1);

						if (!isBoundary)
						{
							const int pattern =
								(pixelX * 13 + pixelY * 7) % 29;

							if (pattern == 0)
							{
								color = BackgroundColor::DarkGray;
							}
						}

						pixel.color = color;
					}
				}
			}
		}
	}

	void TileMap::BuildPathDebugImage()
	{
		debugPathTileImage.width = tileWidth;
		debugPathTileImage.height = tileHeight;
		debugPathTileImage.pixels.assign(
			static_cast<size_t>(tileWidth) * static_cast<size_t>(tileHeight),
			Pixel{});

		for (Pixel& pixel : debugPathTileImage.pixels)
		{
			pixel.color = BackgroundColor::Red;
			pixel.transparent = false;
		}
	}

	bool TileMap::CanOccupyTile(int tileX, int tileY, float actorWidth, float actorHeight) const
	{
		// 맵 밖이면 못 들어감
		if (tileX < 0 || tileX >= tilemapWidth || tileY < 0 || tileY >= tilemapHeight)
		{
			return false;
		}

		// 타일의 중앙 월드 좌표 계산
		const Vector2F center = TileToWorld(tileX, tileY) + Vector2F(tileWidth * 0.5f, tileHeight * 0.5f);

		// 액터를 그 위치에 놓았다고 가정한 바운더리
		const Bounds bounds{
			center.x - actorWidth * 0.5f,
			center.x + actorWidth * 0.5f,
			center.y - actorHeight * 0.5f,
			center.y + actorHeight * 0.5f
		};

		// 벽과 겹치지 않으면 들어갈수 있음
		return CanOccupyWorld(bounds);
	}

}
