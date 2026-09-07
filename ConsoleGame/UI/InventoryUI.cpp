#include <UI/InventoryUI.h>

#include <Inventory/Inventory.h>

#include <Render/Renderer.h>
#include <Input/Input.h>
#include <Math/Color.h>

InventoryUI::InventoryUI(const std::shared_ptr<Inventory>& inventory, const Craft::Vector2F& position)
	: inventory(inventory), position(position)
{
	CreateBackgroundImage(*inventory);
	CreateSlotImage(slotImage, Craft::BackgroundColor::DarkGray, Craft::BackgroundColor::Black);
	CreateSlotImage(hoveredSlotImage, Craft::BackgroundColor::LightCyan, Craft::BackgroundColor::DarkGray);

	CreateSlotImage(placeableSlotImage, Craft::BackgroundColor::LightGreen, Craft::BackgroundColor::DarkGray);
	CreateSlotImage(blockedSlotImage, Craft::BackgroundColor::LightRed, Craft::BackgroundColor::DarkGray);

	SetVisible(false);
}

void InventoryUI::Tick(float deltaTime)
{
	ToggleVisibility();

	if (!IsVisible())
	{
		ClearHoveredSlot();
		return;
	}

	auto lockedInventory = inventory.lock();

	if (!lockedInventory)
	{
		ClearHoveredSlot();
		return;
	}

	UpdateHoveredSlot(*lockedInventory);
	HandleMouseInput(*lockedInventory);
	UpdatePreviewSlot(*lockedInventory);

	auto grabbed = grabbedItem.lock();
	if (!grabbed)
	{
		return;
	}

	if (Craft::Input::Get().GetKeyDown('R'))
	{
		grabbed->SetRotated(!grabbed->IsRotated());
	}

}

void InventoryUI::Draw()
{
	auto lockedInventory = inventory.lock();

	if (!lockedInventory)
	{
		return;
	}

	DrawOuter(*lockedInventory);
	DrawSlots(*lockedInventory);
	DrawItems(*lockedInventory);
}

void InventoryUI::CreateSlotImage(Craft::PixelImage& image, Craft::BackgroundColor borderColor, Craft::BackgroundColor innerColor)
{
	image.width = slotWidth;
	image.height = slotHeight;
	image.pixels.resize(slotWidth * slotHeight);

	for (int y = 0; y < slotHeight; ++y)
	{
		for (int x = 0; x < slotWidth; ++x)
		{
			Craft::Pixel& pixel = image.At(x, y);

			const bool isBorder = x < slotBorderWidth || x >= slotWidth - slotBorderWidth || y < slotBorderHeight || y >= slotHeight - slotBorderHeight;

			pixel.transparent = false;
			pixel.color = isBorder ? borderColor : innerColor;
		}
	}
}

void InventoryUI::CreateBackgroundImage(const Inventory& inventory)
{
	backgroundImage.width = inventory.GetColumnCount() * slotWidth + outerBorderWidth * 2;

	backgroundImage.height = inventory.GetRowCount() * slotHeight + outerBorderHeight * 2;

	backgroundImage.pixels.resize(backgroundImage.width * backgroundImage.height);

	for (Craft::Pixel& pixel : backgroundImage.pixels)
	{
		pixel.transparent = false;
		pixel.color = Craft::BackgroundColor::DarkGray;
	}
}

void InventoryUI::ToggleVisibility()
{
	if (!Craft::Input::Get().GetKeyDown(VK_TAB))
	{
		return;
	}

	SetVisible(!IsVisible());
}

void InventoryUI::UpdateHoveredSlot(const Inventory& inventory)
{
	ClearHoveredSlot();

	const Craft::Vector2F& mousePosition = Craft::Input::Get().GetMousePosition();

	const float localX = mousePosition.x - position.x;
	const float localY = mousePosition.y - position.y;

	const float inventoryWidth = static_cast<float>(inventory.GetColumnCount() * slotWidth);

	const float inventoryHeight = static_cast<float>(inventory.GetRowCount() * slotHeight);

	if (localX < 0.f || localX >= inventoryWidth || localY < 0.f || localY >= inventoryHeight)
	{
		return;
	}

	hoveredSlot.x = static_cast<int>(localX) / slotWidth;
	hoveredSlot.y = static_cast<int>(localY) / slotHeight;
}

void InventoryUI::UpdatePreviewSlot(const Inventory& inventory)
{
	ClearPreviewSlot();

	std::shared_ptr<Item> grabbed = grabbedItem.lock();

	if (!grabbed)
	{
		return;
	}

	if (hoveredSlot.x < 0 || hoveredSlot.y < 0)
	{
		return;
	}

	// 미리 놓을칸 위치 업데이트
	previewSlot.x = hoveredSlot.x - grabbedSlotOffset.x;
	previewSlot.y = hoveredSlot.y - grabbedSlotOffset.y;

	canPlaceGrabbedItem = inventory.CanPlaceItem(grabbed, previewSlot.x, previewSlot.y);
}

void InventoryUI::HandleMouseInput(Inventory& inventory)
{
	if (!Craft::Input::Get().GetKeyDown(VK_LBUTTON))
	{
		return;
	}

	if (hoveredSlot.x < 0 || hoveredSlot.y < 0)
	{
		return;
	}

	auto item = grabbedItem.lock();

	// 현재 선택한 아이템이 없을때
	if (!item)
	{
		auto grabItem = inventory.GetItemAt(hoveredSlot.x, hoveredSlot.y);
		if (grabItem)
		{
			grabbedItem = grabItem;

			const Craft::Vector2F& mousePosition = Craft::Input::Get().GetMousePosition();

			grabOffset = mousePosition - GetItemPosition(*grabItem);

			grabbedSlotOffset.x = hoveredSlot.x - grabItem->GetInventoryX();
			grabbedSlotOffset.y = hoveredSlot.y - grabItem->GetInventoryY();
		}
	}
	// 선택된 아이템이 있었을때
	else
	{
		const int targetX = hoveredSlot.x - grabbedSlotOffset.x;
		const int targetY = hoveredSlot.y - grabbedSlotOffset.y;

		if (inventory.MoveItem(item, targetX, targetY))
		{
			grabbedItem.reset();
		}
	}

	
}

void InventoryUI::ClearHoveredSlot()
{
	hoveredSlot.x = -1;
	hoveredSlot.y = -1;
}

void InventoryUI::ClearPreviewSlot()
{
	previewSlot.x = -1;
	previewSlot.y = -1;
	canPlaceGrabbedItem = false;
}

void InventoryUI::DrawOuter(const Inventory& inventory)
{
	Craft::Renderer::Get().SubmitUI(backgroundImage, Craft::Vector2F(position.x - outerBorderWidth, position.y - outerBorderHeight),
		Craft::Vector2F::Zero, 9999);
}

void InventoryUI::DrawSlots(const Inventory& inventory)
{
	std::shared_ptr<Item> grabbed = grabbedItem.lock();

	const int columnCount = inventory.GetColumnCount();
	const int rowCount = inventory.GetRowCount();

	for (int y = 0; y < rowCount; ++y)
	{
		for (int x = 0; x < columnCount; ++x)
		{
			Craft::Vector2F slotPosition(position.x + x * slotWidth, position.y + y * slotHeight);

			const bool isHovered = x == hoveredSlot.x && y == hoveredSlot.y;


			const bool isPreviewSlot = grabbed && x >= previewSlot.x && x < previewSlot.x + grabbed->GetInventoryWidth() &&
				y >= previewSlot.y && y < previewSlot.y + grabbed->GetInventoryHeight();

			const Craft::PixelImage* image = &slotImage;

			if (isHovered)
			{
				image = &hoveredSlotImage;
			}

			if (isPreviewSlot)
			{
				image = canPlaceGrabbedItem ? &placeableSlotImage : &blockedSlotImage;
			}

			Craft::Renderer::Get().SubmitUI(*image, slotPosition);
		}
	}
}

void InventoryUI::DrawItems(const Inventory& inventory)
{
	std::shared_ptr<Item> grabbed = grabbedItem.lock();

	// 슬롯에 아이템 출력부분
	for (const std::shared_ptr<Item>& item : inventory.GetItems())
	{
		if (!item || !item->GetData())
		{
			continue;
		}

		const Craft::PixelImage& itemImage = item->GetData()->image;

		if (itemImage.pixels.empty())
		{
			continue;
		}

		Craft::Vector2F drawPosition = GetItemPosition(*item);

		if (item == grabbed)
		{
			const Craft::Vector2F& mousePosition = Craft::Input::Get().GetMousePosition();

			drawPosition = mousePosition - grabOffset;
		}

		Craft::Renderer::Get().SubmitUI(itemImage, drawPosition, Craft::Vector2F::Zero, 10001);
	}
}

Craft::Vector2F InventoryUI::GetSlotPosition(int x, int y) const
{
	return Craft::Vector2F(position.x + x * slotWidth, position.y + y * slotHeight);
}

Craft::Vector2F InventoryUI::GetItemPosition(const Item& item) const
{
	const std::shared_ptr<const ItemData> data = item.GetData();

	if (!data)
	{
		return position;
	}

	const float itemAreaWidth = static_cast<float>(item.GetInventoryWidth() * slotWidth);

	const float itemAreaHeight = static_cast<float>(item.GetInventoryHeight() * slotHeight);

	return Craft::Vector2F(position.x + item.GetInventoryX() * slotWidth + (itemAreaWidth - data->image.width) * 0.5f,
		position.y + item.GetInventoryY() * slotHeight + (itemAreaHeight - data->image.height) * 0.5f);
}
