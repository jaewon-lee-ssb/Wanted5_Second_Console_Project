#pragma once

#include <Resource/PixelImage.h>

#include <Item/ItemType.h>

struct ItemData
{
	int id = 0;

	int inventoryWidth = 1;
	int inventoryHeight = 1;

	ItemType type = ItemType::DropItem;

	Craft::PixelImage image;
};