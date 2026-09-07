#pragma once

#include <UIManager/UIElement.h>
#include <Math/Vector2.h>
#include <Resource/PixelImage.h>

#include <memory>

class Inventory;
class Item;

class InventoryUI : public Craft::UIElement
{
public:
	InventoryUI(const std::shared_ptr<Inventory>& inventory, const Craft::Vector2F& position);

	virtual ~InventoryUI() = default;

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:
	// 빈 슬롯 이미지 생성
	void CreateSlotImage(Craft::PixelImage& image, Craft::BackgroundColor borderColor, Craft::BackgroundColor innerColor);
	void CreateBackgroundImage(const Inventory& inventory);
	

	// 인벤토리 껏다 켯다 하는 함수
	void ToggleVisibility();

	void UpdateHoveredSlot(const Inventory& inventory);
	void HandleMouseInput(const Inventory& inventory);
	void ClearHoveredSlot();

	void DrawOuter(const Inventory& inventory);
	void DrawSlots(const Inventory& inventory);
	void DrawItems(const Inventory& inventory);

	Craft::Vector2F GetSlotPosition(int x, int y) const;
	Craft::Vector2F GetItemPosition(const Item& item) const;

private:
	// 화면에 표시할 인벤토리를 관찰한다.
	std::weak_ptr<Inventory> inventory;

	// 선택된 아이템
	std::weak_ptr<Item> selectedItem;

	// 카메라 영향을 받지 않는 화면 좌표이다.
	Craft::Vector2F position;

	Craft::PixelImage slotImage;
	Craft::PixelImage hoveredSlotImage;
	Craft::PixelImage selectedSlotImage;
	Craft::PixelImage backgroundImage;

	int hoveredSlotX = -1;
	int hoveredSlotY = -1;

	static constexpr int slotWidth = 32;
	static constexpr int slotHeight = 16;
	static constexpr int slotBorderWidth = 2;
	static constexpr int slotBorderHeight = 1;

	static constexpr int outerBorderWidth = 4;
	static constexpr int outerBorderHeight = 2;
};

