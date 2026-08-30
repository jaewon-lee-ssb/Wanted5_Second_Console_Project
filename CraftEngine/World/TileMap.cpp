#include <World/TileMap.h>
#include <Camera/Camera.h>
#include <Level/Level.h>
#include <Render/Renderer.h>

#include <fstream>
#include <cassert>
#include <algorithm>
#include <limits>
#include <queue>


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


		mapWidth = loadedWidth;
		mapHeight = loadedHeight;
		tiles = std::move(loadedTiles);

		mapImage.width = mapWidth * tileWidth;
		mapImage.height = mapHeight * tileHeight;
		mapImage.pixels.assign(static_cast<size_t>(mapImage.width) * static_cast<size_t>(mapImage.height), Pixel{});

		BuildVisualImage();
		BuildPathDebugImage();


		file.close();

		return true;

	}

	TileType TileMap::GetTile(int x, int y) const
	{
		if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
		{
			return TileType::Wall;
		}

		return tiles[y * mapWidth + x];
	}

	bool TileMap::IsSolid(int x, int y) const
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

	bool TileMap::OverlapsSolid(const Bounds& worldBounds) const
	{
		// 월드 좌표에서 타일맵 위치를 빼서
		// 타일맵 내부 로컬 좌표로 변환
		const float localLeft = worldBounds.left - GetPosition().x;
		const float localRight = worldBounds.right - GetPosition().x;
		const float localTop = worldBounds.top - GetPosition().y;
		const float localBottom = worldBounds.bottom - GetPosition().y;

		// Bounds가 걸치는 타일 좌표 범위 계산
		const int leftTile = static_cast<int>(std::floor(localLeft / tileWidth));
		const int rightTile = static_cast<int>(std::floor(localRight / tileWidth));
		const int topTile = static_cast<int>(std::floor(localTop / tileHeight));
		const int bottomTile = static_cast<int>(std::floor(localBottom / tileHeight));

		// 해당 범위에 벽이 하나라도 있으면 충돌
		for (int tileY = topTile; tileY <= bottomTile; ++tileY)
		{
			for (int tileX = leftTile; tileX <= rightTile; ++tileX)
			{
				if (IsSolid(tileX, tileY))
				{
					return true;
				}
			}
		}

		return false;
	}

	std::vector<Vector2I> TileMap::FindPath(const Vector2F& startWorldPosition, const Vector2F& endWorldPosition, float actorWidth, float actorHeight) const
	{
		std::vector<Vector2I> resultPath;

		int startX = 0;
		int startY = 0;
		int endX = 0;
		int endY = 0;

		WorldToTile(startWorldPosition, startX, startY);
		WorldToTile(endWorldPosition, endX, endY);

		if (startX < 0 || startX >= mapWidth ||
			startY < 0 || startY >= mapHeight ||
			endX < 0 || endX >= mapWidth ||
			endY < 0 || endY >= mapHeight)
		{
			return {};
		}

		// 시작점과 목적지에 액터가 들어갈수 있나 체크
		Vector2F nodeEndPosition = TileToWorld(endX, endY) + Vector2F(tileWidth * 0.5f, tileHeight * 0.5f);

		Bounds startBounds(startWorldPosition.x - actorWidth * 0.5f, startWorldPosition.x + actorWidth * 0.5f,
			startWorldPosition.y - actorHeight * 0.5f, startWorldPosition.y + actorHeight * 0.5f);
		Bounds endBounds(nodeEndPosition.x - actorWidth * 0.5f, nodeEndPosition.x + actorWidth * 0.5f,
			nodeEndPosition.y - actorHeight * 0.5f, nodeEndPosition.y + actorHeight * 0.5f);

		if (OverlapsSolid(startBounds) || OverlapsSolid(endBounds))
		{
			return resultPath;
		}

		const int startIndex = startY * mapWidth + startX;
		const int endIndex = endY * mapWidth + endX;

		const int tileCount = mapWidth * mapHeight;
		constexpr int infiniteCost = (std::numeric_limits<int>::max)();

		std::vector<int> gCosts(tileCount, infiniteCost);
		std::vector<int> parents(tileCount, -1);
		std::vector<bool> closed(tileCount, false);

		std::priority_queue<OpenNode, std::vector<OpenNode>, CompareOpenNode> open;

		auto CalculateH = [endX, endY](int x, int y)
			{
				return std::abs(endX - x) + std::abs(endY - y);
			};
		
		gCosts[startIndex] = 0;

		open.push({ startIndex, 0, CalculateH(startX, startY) });

		const Vector2I directions[] =
		{
			Vector2I(1, 0),
			Vector2I(-1, 0),
			Vector2I(0, 1),
			Vector2I(0, -1),
		};

		while (!open.empty())
		{
			const OpenNode current = open.top();
			open.pop();
			assert(current.index >= 0 && current.index < tileCount);

			// 이미 처리한 타일이면 무시
			if (closed[current.index])
			{
				continue;
			}

			closed[current.index] = true;

			// 목표에 도착
			if (current.index == endIndex)
			{
				int pathIndex = endIndex;

				while (pathIndex != -1)
				{
					const int pathX = pathIndex % mapWidth;
					const int pathY = pathIndex / mapWidth;

					resultPath.emplace_back(pathX, pathY);
					pathIndex = parents[pathIndex];
				}

				// 목표부터 시작점 순서로 들어 있으므로 뒤집기.
				std::reverse(resultPath.begin(), resultPath.end());

				// 첫 번째 좌표는 현재 위치이므로 제거.
				if (!resultPath.empty())
				{
					resultPath.erase(resultPath.begin());
				}

				return resultPath;
			}

			const int currentX = current.index % mapWidth;
			const int currentY = current.index / mapWidth;

			for (const Vector2I& direction : directions)
			{
				const int nextX = currentX + direction.x;
				const int nextY = currentY + direction.y;

				// 인덱스로 바꾸기 전에 맵 범위 검사
				if (nextX < 0 || nextX >= mapWidth || nextY < 0 || nextY >= mapHeight)
				{
					continue;
				}

				Vector2F nodePosition = TileToWorld(nextX, nextY) + Vector2F(tileWidth * 0.5f, tileHeight * 0.5f);

				Bounds actorBounds(nodePosition.x - actorWidth * 0.5f, nodePosition.x + actorWidth * 0.5f,
					nodePosition.y - actorHeight * 0.5f, nodePosition.y + actorHeight * 0.5f);

				if (OverlapsSolid(actorBounds))
				{
					continue;
				}

				const int nextIndex = nextY * mapWidth + nextX;

				if (closed[nextIndex])
				{
					continue;
				}

				// 상하좌우 한 칸의 이동 비용은 1
				const int newGCost = gCosts[current.index] + 1;

				// 기존에 발견한 길보다 길다면 무시
				if (newGCost >= gCosts[nextIndex])
				{
					continue;
				}

				gCosts[nextIndex] = newGCost;
				parents[nextIndex] = current.index;

				const int hCost = CalculateH(nextX, nextY);

				open.push({ nextIndex, newGCost, newGCost + hCost });
			
			}
		}

		// Open이 비었는데 목표를 못 찾았다면 빈 경로 반환
		return resultPath;
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
		for (int tileY = 0; tileY < mapHeight; ++tileY)
		{
			for (int tileX = 0; tileX < mapWidth; ++tileX)
			{
				if (!IsSolid(tileX, tileY))
				{
					continue;
				}

				const bool exposedUp = !IsSolid(tileX, tileY - 1);
				const bool exposedDown = !IsSolid(tileX, tileY + 1);
				const bool exposedLeft = !IsSolid(tileX - 1, tileY);
				const bool exposedRight = !IsSolid(tileX + 1, tileY);

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

}
