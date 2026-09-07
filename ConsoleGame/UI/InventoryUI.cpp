#include <UI/InventoryUI.h>

#include <Inventory/Inventory.h>

#include <Render/Renderer.h>
#include <Math/Color.h>

InventoryUI::InventoryUI(const std::shared_ptr<const Inventory>& inventory, const Craft::Vector2F& position)
	: inventory(inventory), position(position)
{
	CreateSlotImage();
}

void InventoryUI::Tick(float deltaTime)
{
	
}

void InventoryUI::Draw()
{
	auto lockedInventory = inventory.lock();

	if (!lockedInventory)
	{
		return;
	}

	const int columnCount = lockedInventory->GetColumnCount();
	const int rowCount = lockedInventory->GetRowCount();
	
	for (int y = 0; y < rowCount; ++y)
	{
		for (int x = 0; x < columnCount; ++x)
		{
			Craft::Vector2F slotPosition(position.x + x * slotWidth, position.y + y * slotHeight);

			Craft::Renderer::Get().SubmitUI(slotImage, slotPosition);
		}
	}
}

void InventoryUI::CreateSlotImage()
{
	slotImage.width = slotWidth;
	slotImage.height = slotHeight;
	slotImage.pixels.resize(slotWidth * slotHeight);

	for (int y = 0; y < slotHeight; ++y)
	{
		for (int x = 0; x < slotWidth; ++x)
		{
			Craft::Pixel& pixel = slotImage.At(x, y);

			const bool isBorder = x == 0 || x == slotWidth - 1 || y == 0 || y == slotHeight - 1;

			pixel.transparent = false;
			pixel.color = isBorder ? Craft::BackgroundColor::LightGray : Craft::BackgroundColor::Black;
		}
	}
}
