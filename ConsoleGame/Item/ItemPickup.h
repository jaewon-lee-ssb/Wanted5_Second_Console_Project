#pragma once

#include <Actor/Actor.h>

class ItemData;

class ItemPickup : public Craft::Actor
{
	TYPE_DECLARATIONS(ItemPickup, Actor)

public:
	ItemPickup(const Craft::Vector2F& position);
	~ItemPickup() = default;

private:

private:
	std::weak_ptr<ItemData> itemData;


};

