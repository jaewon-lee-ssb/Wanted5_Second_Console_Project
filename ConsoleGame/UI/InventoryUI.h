#pragma once

#include <UIManager/UIElement.h>
#include <Math/Vector2.h>
#include <Resource/PixelImage.h>

#include <memory>

class Inventory;

class InventoryUI : public Craft::UIElement
{
public:
	InventoryUI(const std::shared_ptr<const Inventory>& inventory, const Craft::Vector2F& position);

	virtual ~InventoryUI() = default;

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:
	void CreateSlotImage();

private:
	// 화면에 표시할 인벤토리를 관찰한다.
	std::weak_ptr<const Inventory> inventory;

	// 카메라 영향을 받지 않는 화면 좌표이다.
	Craft::Vector2F position;

	Craft::PixelImage slotImage;

	static constexpr int slotWidth = 6;
	static constexpr int slotHeight = 3;
};

