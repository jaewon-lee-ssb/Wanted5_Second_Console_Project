#include <Resource/TextImageLoader.h>

#include <fstream>
#include <cassert>

namespace Craft
{
	std::vector<PixelImage> TextImageLoader::LoadAnimation(const std::string& filename)
	{
		// 데이터를 불러와서 저장할 벡터
		std::vector<PixelImage> animationVector;

		// 이미지의 길이값
		int loadedWidth = 0;
		int loadedHeight = 0;

		// 스프라이트 이미지를 저장할 임시 변수
		PixelImage resultImage;

		// 최종경로
		std::string path = std::string("../Contents/") + filename;

		// 파일 열기 및 확인.
		std::ifstream file(path, std::ios_base::binary);
		assert(file.is_open() && "file not open");

		if (!file.is_open())
		{
			// 오류 출력 또는 로그 기록
			return {};
		}

		// 한 줄씩 읽어올 문자열
		std::string line;

		

		while (std::getline(file, line))
		{
			// \r 개행 문자 삭제처리
			if (!line.empty() && line.back() == '\r')
			{
				line.pop_back();
			}
			
			// 스프라이트 구분자 검사
			if (line == "#")
			{
				// 빈 이미지인데 저장하려고 하면 오류
				if (resultImage.pixels.empty() || loadedWidth <= 0 || loadedHeight <= 0)
				{
					return {};
				}

				resultImage.width = loadedWidth;
				resultImage.height = loadedHeight;

				loadedWidth = 0;
				loadedHeight = 0;

				animationVector.emplace_back(resultImage);
				resultImage.Reset();

				continue;
			}

			// 문자열이 비어있으면 종료
			if (line.empty())
			{
				return animationVector;
			}

			

			// 맵 가로길이가 같은지 확인
			if (loadedWidth == 0)
			{
				loadedWidth = static_cast<int>(line.size());
			}
			else if (line.size() != loadedWidth)
			{
				return animationVector;
			}

			

			for (char character : line)
			{
				

				Pixel resultPixel;

				if (character != '.')
				{
					resultPixel.color = ParseColor(character);
					resultPixel.transparent = false;
				}

				resultImage.pixels.emplace_back(resultPixel);
			}

			++loadedHeight;

		}

		// 파일 닫기
		file.close();

		return animationVector;
	}
}