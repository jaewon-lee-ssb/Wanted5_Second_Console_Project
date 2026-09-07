#pragma once

#include <Item/ItemData.h>

#include <memory>

struct DropEntry
{
	std::shared_ptr<const ItemData> itemData;

	int weight = 0;
};