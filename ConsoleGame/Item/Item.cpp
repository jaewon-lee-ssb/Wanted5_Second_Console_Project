#include <Item/Item.h>

int Item::GetInventoryWidth() const
{
	if (!data)
	{
		return 0;
	}

	return isRotated ? data->inventoryHeight : data->inventoryWidth;
}

int Item::GetInventoryHeight() const
{
	if (!data)
	{
		return 0;
	}

	return isRotated ? data->inventoryWidth : data->inventoryHeight;
}

int Item::GetTotalSellPrice() const
{
	if (!data)
	{
		return 0;
	}

	return data->sellPrice * count;
}

void Item::SetInventoryPosition(int x, int y)
{
	inventoryX = x;
	inventoryY = y;
}

void Item::ClearInventoryPosition()
{
	inventoryX = -1;
	inventoryY = -1;
}

void Item::SetRotated(bool rotated)
{
	isRotated = rotated;
}
