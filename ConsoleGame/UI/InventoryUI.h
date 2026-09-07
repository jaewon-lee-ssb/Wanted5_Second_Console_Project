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
	void UpdatePreviewSlot(const Inventory& inventory);
	void HandleMouseInput(Inventory& inventory);
	void ClearHoveredSlot();
	void ClearPreviewSlot();

	void DrawOuter(const Inventory& inventory);
	void DrawSlots(const Inventory& inventory);
	void DrawItems(const Inventory& inventory);

	Craft::Vector2F GetSlotPosition(int x, int y) const;
	Craft::Vector2F GetItemPosition(const Item& item) const;

private:
	// 화면에 표시할 인벤토리를 관찰한다.
	std::weak_ptr<Inventory> inventory;


	// 들고있는 아이템
	std::weak_ptr<Item> grabbedItem;

	// 카메라 영향을 받지 않는 화면 좌표이다.
	Craft::Vector2F position;

	Craft::PixelImage slotImage;
	Craft::PixelImage hoveredSlotImage;
	Craft::PixelImage backgroundImage;

	Craft::PixelImage placeableSlotImage;
	Craft::PixelImage blockedSlotImage;

	// 마우스가 올려져있는 슬롯의 인덱스
	Craft::Vector2I hoveredSlot = Craft::Vector2I(-1, -1);

	// 놓을 자리를 보여줄 미리보기 슬롯
	Craft::Vector2I previewSlot = Craft::Vector2I(-1, -1);

	bool canPlaceGrabbedItem = false;

	// 잡은 아이템의 슬롯 차이칸
	Craft::Vector2I grabbedSlotOffset = Craft::Vector2I::Zero;

	// 잡고있는 부분의 마우스와의 거리차이
	Craft::Vector2F grabOffset = Craft::Vector2F::Zero;

	static constexpr int slotWidth = 32;
	static constexpr int slotHeight = 16;
	static constexpr int slotBorderWidth = 2;
	static constexpr int slotBorderHeight = 1;

	static constexpr int outerBorderWidth = 4;
	static constexpr int outerBorderHeight = 2;
};

