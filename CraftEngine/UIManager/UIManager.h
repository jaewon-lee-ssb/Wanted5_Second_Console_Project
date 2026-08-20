#pragma once

#include <Core/Core.h>

#include <memory>
#include <vector>

namespace Craft
{
	class UIElement;

	class CRAFT_API UIManager
	{
	public:
		UIManager() = default;
		~UIManager() = default;

		void Tick(float deltaTime);
		void Draw();

		template<typename T, typename... Args>
		std::shared_ptr<T> CreateUI(Args&&... args)
		{
			std::shared_ptr<T> newUI = std::make_shared<T>(std::forward<Args>(args)...);

			uiList.emplace_back(newUI);

			return newUI;
		}

		// 싱글톤 접근 함수.
		static UIManager& Get();

		void Clear();

	
	private:
		std::vector<std::shared_ptr<UIElement>> uiList;



	};
}

