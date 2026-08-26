#pragma once

#include <Actor/Actor.h>

#include <array>

namespace Craft
{
	class TileMap;
}

class Player : public Craft::Actor
{
	TYPE_DECLARATIONS(Player, Actor)

public:
	Player(const Craft::Vector2F& position);
	~Player() = default;

	void SetTileMap(const std::shared_ptr<Craft::TileMap>& newMap) { tileMap = newMap; }

	virtual void Tick(float deltaTime) override;

private:
	void Move(float deltaTime);

private:
	// 플레이어 이동관련
	float playerMoveSpeed = 70.f;
	
	Craft::Vector2F playerMoveDir = Craft::Vector2F::Zero;

	// 플레이어 공격관련
	std::array<Craft::Vector2F, 2> playerAttackPoint;

	// 타일 맵
	std::weak_ptr<const Craft::TileMap> tileMap;

	// 플레이어 이미지
	Craft::PixelImage playerIdleImage;

	std::string playerIdleFilename = "Actors/Player/player_idle.txt";
};

