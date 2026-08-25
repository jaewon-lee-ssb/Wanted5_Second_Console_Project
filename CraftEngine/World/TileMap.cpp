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

		if (loadedWidth <= 0 ||	loadedHeight <= 0 || loadedTiles.empty())
		{
			return false;
		}

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

		for (int tileY = 0; tileY < mapHeight; ++tileY)
		{
			for (int tileX = 0; tileX < mapWidth; ++tileX)
			{
				if (!IsSolid(tileX, tileY))
				{
					continue;
				}

				for (int localY = 0; localY < tileHeight; ++localY)
				{
					for (int localX = 0; localX < tileWidth; ++localX)
					{
						int pixelX = tileX * tileWidth + localX;
						int pixelY = tileY * tileHeight + localY;

						Pixel& pixel = mapImage.At(pixelX, pixelY);
						pixel.transparent = false;
						pixel.color = BackgroundColor::Brown;
					}
				}
			}
		}

		ChangeImage(mapImage);
		pivot = Vector2F::Zero;

		

	

		


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
}