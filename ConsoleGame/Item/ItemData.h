#pragma once

#include <Resource/PixelImage.h>

#include <Item/ItemType.h>

#include <string>

struct ItemData
{
	int id = 0;

	std::string name;

	int inventoryWidth = 1;
	int inventoryHeight = 1;

	int sellPrice = 0;

	ItemType type = ItemType::DropItem;

	Craft::PixelImage image;
	Craft::PixelImage rotatedImage;
};