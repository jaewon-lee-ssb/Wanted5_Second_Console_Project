#include <UIManager/UIManager.h>
#include <UIManager/UIElement.h>

#include <cassert>

namespace Craft
{

	void UIManager::Tick(float deltaTime)
	{
		for (const std::shared_ptr<UIElement>& ui : uiList)
		{
			if (!ui)
			{
				continue;
			}

			ui->Tick(deltaTime);
		}
	}

	void UIManager::Draw()
	{
		for (const std::shared_ptr<UIElement>& ui : uiList)
		{
			if (!ui || !ui->IsVisible())
			{
				continue;
			}

			ui->Draw();
		}
	}

	UIManager& UIManager::Get()
	{
		static UIManager instance;
		return instance;
	}

	void UIManager::Clear()
	{
		uiList.clear();
	}
}