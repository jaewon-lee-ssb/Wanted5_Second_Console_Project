#include <Item/ItemPickup.h>

#include <Actor/Player/Player.h>
#include <Inventory/Inventory.h>
#include <Collision/GameCollisionLayers.h>

ItemPickup::ItemPickup(const Craft::Vector2F& position, const std::shared_ptr<Item>& item)
	: super({}, position), item(item)
{
	if (item && item->GetData())
	{
		ChangeImage(CreatePickupImage(item->GetData()->grade));
	}

	SetCollisionLayer(GameCollision::Item);
	SetCollisionMask(GameCollision::Player);
}

void ItemPickup::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void ItemPickup::OnCollision(const std::shared_ptr<Actor>& other)
{
	super::OnCollision(other);

	if (!other || (other->GetCollisionLayer() & GameCollision::Player) == 0)
	{
		return;
	}

	auto player = Cast<Player>(other);

	if (!player)
	{
		return;
	}

	if (player->TryAddItem(item))
	{
		Destroy();
	}

}

Craft::BackgroundColor ItemPickup::GetGradeColor(ItemGrade grade)
{
	switch (grade)
	{
	case ItemGrade::Common:
		return Craft::BackgroundColor::LightGray;

	case ItemGrade::Rare:
		return Craft::BackgroundColor::LightBlue;

	case ItemGrade::Epic:
		return Craft::BackgroundColor::Purple;

	case ItemGrade::Legendary:
		return Craft::BackgroundColor::Yellow;
	}
	return Craft::BackgroundColor::White;
}

Craft::PixelImage ItemPickup::CreatePickupImage(ItemGrade grade)
{
	Craft::PixelImage image;

	image.width = 7;
	image.height = 3;
	image.pixels.resize(image.width * image.height);

	const Craft::BackgroundColor color = GetGradeColor(grade);

	const char* shape[] =
	{
		"..###..",
		".#####.",
		"..###.."
	};

	for (int y = 0; y < image.height; ++y)
	{
		for (int x = 0; x < image.width; ++x)
		{
			if (shape[y][x] != '#')
			{
				continue;
			}

			Craft::Pixel& pixel = image.At(x, y);
			pixel.color = color;
			pixel.transparent = false;
		}
	}

	return image;
}
