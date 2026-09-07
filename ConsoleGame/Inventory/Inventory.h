#pragma once

#include <Item/Item.h>

#include <memory>
#include <vector>

class Inventory
{
public:
	Inventory(int columnCount, int rowCount);

	// 지정한 위치에 배치할 수 있는지 검사
	bool CanPlaceItem(const std::shared_ptr<Item>& item, int startX, int startY) const;

	// 지정할 위치에 아이템 배치
	bool PlaceItem(const std::shared_ptr<Item>& item, int startX, int startY);

	// 들어갈 수 있는 첫 번째 빈 곳에 자동 배치.
	bool AddItem(const std::shared_ptr<Item>& item);


	// 인벤토리에서 완전히 제거
	void RemoveItem(const std::shared_ptr<Item>& item);

	

	bool RotateItem(const std::shared_ptr<Item>& item);

	bool MoveItem(const std::shared_ptr<Item>& item, int newX, int newY);

	const std::vector<std::shared_ptr<Item>>& GetItems() const { return items; }

	int CalculateTotalSellPrice() const;

	void Clear();

	// 해당 칸에 있는 아이템 반환
	std::shared_ptr<Item> GetItemAt(int x, int y) const;

	int GetColumnCount() const { return columnCount; }
	int GetRowCount() const { return rowCount; }

private:
	// 슬롯만 비움: 이동과 회전에 사용
	void ClearItemSlots(const std::shared_ptr<Item>& item);

	bool IsValidPosition(int x, int y) const;
	int GetIndex(int x, int y) const;

private:
	int columnCount = 0;
	int rowCount = 0;

	std::vector<std::shared_ptr<Item>> slots;

	std::vector<std::shared_ptr<Item>> items;

};

