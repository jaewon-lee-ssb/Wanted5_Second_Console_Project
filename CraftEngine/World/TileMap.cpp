#include <World/TileMap.h>

#include <fstream>
#include <cassert>

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


		file.close();

		return true;

	}

	TileType TileMap::GetTile(int x, int y) const
	{
		if (x < 0 || x >= mapWidth ||
			y < 0 || y >= mapHeight)
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

	void TileMap::Draw()
	{
		super::Draw();
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
		const bool alternate = ((worldX + worldY) % 2) == 0;

		if (pattern == 0)
		{
			return BackgroundColor::LightCyan;
		}

		if (depth < 0.2f)
		{
			return alternate ? BackgroundColor::LightCyan : BackgroundColor::Cyan;
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
						BackgroundColor color = BackgroundColor::Purple;

						// 아래쪽과 오른쪽은 그림자
						if ((exposedDown && localY == tileHeight - 1) || (exposedRight && localX == tileWidth - 1))
						{
							color = BackgroundColor::Blue;
						}

						// 위쪽과 왼쪽은 빛을 받는 면
						if ((exposedUp && localY == 0) ||
							(exposedLeft && localX == 0))
						{
							color = BackgroundColor::LightMagenta;
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
								color = BackgroundColor::Blue;
							}
						}

						pixel.color = color;
					}
				}
			}
		}
	}

}