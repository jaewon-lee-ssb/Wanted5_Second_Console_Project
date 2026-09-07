#pragma once

#include <Item/ItemData.h>

#include <memory>
#include <utility>

class Inventory;

class Item
{
	friend class Inventory;

public:
	explicit Item(std::shared_ptr<const ItemData> newData)
		: data(std::move(newData))
	{

	}

	std::shared_ptr<const ItemData> GetData() const { return data; }

	int GetInventoryWidth() const;
	int GetInventoryHeight() const;

	int GetInventoryX() const { return inventoryX; }
	int GetInventoryY() const { return inventoryY; }

	bool IsPlaced() const { return inventoryX >= 0 && inventoryY >= 0; }
	bool IsRotated() const { return isRotated; }
	int GetCount() const { return count; }

	int GetTotalSellPrice() const;

	void SetRotated(bool rotated);

private:
	void SetInventoryPosition(int x, int y);
	void ClearInventoryPosition();
	
	

private:
	std::shared_ptr<const ItemData> data;

	int count = 1;
	int inventoryX = -1;
	int inventoryY = -1;

	bool isRotated = false;
};

