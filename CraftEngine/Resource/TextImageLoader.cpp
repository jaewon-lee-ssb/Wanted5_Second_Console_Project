#include <Resource/TextImageLoader.h>

#include <fstream>
#include <cassert>

namespace Craft
{
	PixelImage TextImageLoader::Load(const std::string& filename)
	{
		PixelImage resultImage;

		// 최종경로
		std::string path = std::string("../Contents/") + filename;

		// 파일 열기 및 확인.
		std::ifstream file(path, std::ios_base::binary);
		assert(file.is_open() && "file not open");

		if (!file.is_open())
		{
			// 오류 출력 또는 로그 기록
			return PixelImage{};
		}

		// 파일 전체 크기 확인.
		file.seekg(0, std::ios_base::end);
		const std::streampos fileSize = file.tellg();

		// 파일 위치를 첫 위치로 되돌리기.
		file.seekg(0, std::ios_base::beg);

		// 파일 내용 전체를 읽기 위한 변수.
		std::string buffer;
		buffer.resize(static_cast<size_t>(fileSize));

		// 파일 내용 전체 읽기.
		file.read(&buffer[0], fileSize);

		// 문자열 buffer를 한 문자씩 읽으면서 처리.
		int index = 0;

		size_t lineEnd = buffer.find('\n');
		
		if (lineEnd == std::string::npos)
		{
			resultImage.width = static_cast<int>(buffer.size());
		}
		else
		{
			resultImage.width = static_cast<int>(lineEnd);

			if (resultImage.width > 0 &&
				buffer[resultImage.width - 1] == '\r')
			{
				--resultImage.width;
			}
		}

		while (true)
		{
			// 종료 조건.
			if (index >= fileSize)
			{
				break;
			}

			// 현재 순번의 문자 값 읽기.
			char mapCharacter = buffer[index];

			// 다음 글자를 읽기위한 준비.
			++index;

			// Windows의 개행 문자는 '\n'이 아니라 '\r\n'이기 때문에
			// \r은 건너뛰기.
			if (mapCharacter == '\r')
			{
				continue;
			}

			// 개행 문자 처리 - 좌표 값 업데이트.
			if (mapCharacter == '\n')
			{
				++resultImage.height;
				continue;
			}

			Pixel pixel;

			// 읽은 문자 별로 처리.
			if (mapCharacter != '.')
			{
				pixel.color = ParseColor(mapCharacter);
				pixel.transparent = false;
			}
			
			resultImage.pixels.emplace_back(pixel);


		}

		if (!buffer.empty() && buffer.back() != '\n')
		{
			++resultImage.height;
		}

		file.close();

		return resultImage;
	}
}