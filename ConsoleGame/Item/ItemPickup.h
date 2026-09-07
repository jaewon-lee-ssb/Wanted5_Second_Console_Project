#pragma once

#include <Actor/Actor.h>
#include <Math/Color.h>
#include <Resource/PixelImage.h>

#include <Item/ItemType.h>

class Item;
class Inventory;

class ItemPickup : public Craft::Actor
{
	TYPE_DECLARATIONS(ItemPickup, Actor)

public:
	ItemPickup(const Craft::Vector2F& position, const std::shared_ptr<Item>& item);
	~ItemPickup() = default;

private:
	virtual void Tick(float deltaTime) override;
	virtual void OnCollision(const std::shared_ptr<Actor>& other) override;

	Craft::BackgroundColor GetGradeColor(ItemGrade grade);
	Craft::PixelImage CreatePickupImage(ItemGrade grade);

private:
	std::shared_ptr<Item> item;

};

