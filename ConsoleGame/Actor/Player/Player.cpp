#include <Actor/Player/Player.h>

#include <Resource/TextImageLoader.h>

Player::Player(const Craft::Vector2F& position)
	: Actor(Craft::PixelImage{}, position, Utility::ActorTags::Player)
{
	playerIdleImage = Craft::TextImageLoader::Load(playerIdleFilename);
	ChangeImage(playerIdleImage);
}

void Player::Tick(float deltaTime)
{
}
