#include <Inventory/Inventory.h>

#include <algorithm>

Inventory::Inventory(int columnCount, int rowCount)
	: columnCount(columnCount), rowCount(rowCount), slots(columnCount * rowCount)
{

}

bool Inventory::CanPlaceItem(const std::shared_ptr<Item>& item, int startX, int startY) const
{
	if (!item || !item->GetData())
	{
		return false;
	}

	const int itemWidth = item->GetInventoryWidth();
	const int itemHeight = item->GetInventoryHeight();

	// 아이템 크기가 0인부분이 있으면
	if (itemWidth <= 0 || itemHeight <= 0)
	{
		return false;
	}

	for (int y = 0; y < itemHeight; ++y)
	{
		for (int x = 0; x < itemWidth; ++x)
		{
			const int slotX = startX + x;
			const int slotY = startY + y;

			// 인벤토리 위치를 벗어남
			if (!IsValidPosition(slotX, slotY))
			{
				return false;
			}
			
			const auto& placedItem = slots[GetIndex(slotX, slotY)];

			// 이미 다른 아이템이 들어 있음
			if (placedItem && placedItem != item)
			{
				return false;
			}
		}
	}

	return true;
}

bool Inventory::PlaceItem(const std::shared_ptr<Item>& item, int startX, int startY)
{
	if (!item || item->IsPlaced())
	{
		return false;
	}

	if (!CanPlaceItem(item, startX, startY))
	{
		return false;
	}

	const int itemWidth = item->GetInventoryWidth();
	const int itemHeight = item->GetInventoryHeight();

	for (int y = 0; y < itemHeight; ++y)
	{
		for (int x = 0; x < itemWidth; ++x)
		{
			slots[GetIndex(startX + x, startY + y)] = item;
		}
	}

	item->SetInventoryPosition(startX, startY);

	if (std::find(items.begin(), items.end(), item) == items.end())
	{
		items.emplace_back(item);
	}

	return true;
}

bool Inventory::AddItem(const std::shared_ptr<Item>& item)
{
	if (!item || !item->GetData())
	{
		return false;
	}

	// 왼쪽 위부터 배치 가능한 위치 검색
	for (int y = 0; y < rowCount; ++y)
	{
		for (int x = 0; x < columnCount; ++x)
		{
			if (PlaceItem(item, x, y))
			{
				return true;
			}
		}
	}

	// 들어갈 공간이 없음
	return false;
}

void Inventory::RemoveItem(const std::shared_ptr<Item>& item)
{
	ClearItemSlots(item);

	items.erase(std::remove(items.begin(), items.end(), item), items.end());
}

void Inventory::ClearItemSlots(const std::shared_ptr<Item>& item)
{
	if (!item)
	{
		return;
	}

	for (auto& slot : slots)
	{
		if (slot == item)
		{
			slot.reset();
		}
	}

	item->ClearInventoryPosition();
}

bool Inventory::RotateItem(const std::shared_ptr<Item>& item)
{
	if (!item || !item->IsPlaced())
	{
		return false;
	}

	const int oldX = item->GetInventoryX();
	const int oldY = item->GetInventoryY();
	const bool oldRotated = item->IsRotated();

	ClearItemSlots(item);

	item->SetRotated(!oldRotated);

	if (PlaceItem(item, oldX, oldY))
	{
		return true;
	}

	item->SetRotated(oldRotated);
	PlaceItem(item, oldX, oldY);

	return false;
}

bool Inventory::MoveItem(const std::shared_ptr<Item>& item, int newX, int newY)
{
	if (!item || !item->IsPlaced())
	{
		return false;
	}

	const int oldX = item->GetInventoryX();
	const int oldY = item->GetInventoryY();

	// 기존 격자에서만 빼기
	ClearItemSlots(item);

	if (PlaceItem(item, newX, newY))
	{
		return true;
	}

	// 실패하면 원래 자리로 복구
	PlaceItem(item, oldX, oldY);

	return false;
}

int Inventory::CalculateTotalSellPrice() const
{
	int totalPrice = 0;

	for (const auto& item : items)
	{
		if (item)
		{
			totalPrice += item->GetTotalSellPrice();
		}
	}

	return totalPrice;
}

void Inventory::Clear()
{
	for (auto& item : items)
	{
		ClearItemSlots(item);
	}

	items.clear();
}

std::shared_ptr<Item> Inventory::GetItemAt(int x, int y) const
{
	if (!IsValidPosition(x, y))
	{
		return nullptr;
	}

	return slots[GetIndex(x, y)];
}

bool Inventory::IsValidPosition(int x, int y) const
{
	return x >= 0 && x < columnCount && y >= 0 && y < rowCount;
}

int Inventory::GetIndex(int x, int y) const
{
	return y * columnCount + x;
}
