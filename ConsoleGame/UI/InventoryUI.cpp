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
	CreateSlotImage(selectedSlotImage, Craft::BackgroundColor::Yellow, Craft::BackgroundColor::Black);

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

	hoveredSlotX = static_cast<int>(localX) / slotWidth;
	hoveredSlotY = static_cast<int>(localY) / slotHeight;
}

void InventoryUI::HandleMouseInput(const Inventory& inventory)
{
	if (!Craft::Input::Get().GetKeyDown(VK_LBUTTON))
	{
		return;
	}

	if (hoveredSlotX < 0 || hoveredSlotY < 0)
	{
		return;
	}

	selectedItem = inventory.GetItemAt(hoveredSlotX, hoveredSlotY);
}

void InventoryUI::ClearHoveredSlot()
{
	hoveredSlotX = -1;
	hoveredSlotY = -1;
}

void InventoryUI::DrawOuter(const Inventory& inventory)
{
	Craft::Renderer::Get().SubmitUI(backgroundImage, Craft::Vector2F(position.x - outerBorderWidth, position.y - outerBorderHeight),
		Craft::Vector2F::Zero, 9999);
}

void InventoryUI::DrawSlots(const Inventory& inventory)
{
	std::shared_ptr<Item> selected = selectedItem.lock();

	const int columnCount = inventory.GetColumnCount();
	const int rowCount = inventory.GetRowCount();

	for (int y = 0; y < rowCount; ++y)
	{
		for (int x = 0; x < columnCount; ++x)
		{
			Craft::Vector2F slotPosition(position.x + x * slotWidth, position.y + y * slotHeight);

			const bool isHovered = x == hoveredSlotX && y == hoveredSlotY;

			const bool isSelected = selected && inventory.GetItemAt(x, y) == selected;

			const Craft::PixelImage* image = &slotImage;

			if (isHovered)
			{
				image = &hoveredSlotImage;
			}

			if (isSelected)
			{
				image = &selectedSlotImage;
			}

			Craft::Renderer::Get().SubmitUI(*image, slotPosition);
		}
	}
}

void InventoryUI::DrawItems(const Inventory& inventory)
{
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


		Craft::Renderer::Get().SubmitUI(itemImage, GetItemPosition(*item), Craft::Vector2F::Zero, 10001);
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
