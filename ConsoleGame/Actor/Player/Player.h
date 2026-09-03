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

	enum class PlayerState
	{
		Idle,
		Swim,
		Count,
	};

public:
	Player(const Craft::Vector2F& position);
	~Player() = default;

	void SetTileMap(const std::shared_ptr<Craft::TileMap>& newMap) { tileMap = newMap; }

	virtual void Tick(float deltaTime) override;

private:
	void Move(float deltaTime);

private:
	// 플레이어 이동관련
	float playerMoveSpeedX = 100.f;
	float playerMoveSpeedY = 50.f;
	
	Craft::Vector2F playerMoveDir = Craft::Vector2F::Zero;

	// 플레이어 애니메이션 관련 변수
	float animationElapsedTime = 0.f;
	float animationFrameTime = 0.2f;

	int currentAnimationSpriteIndex = 0;
	int currentStateIndex = 0;

	// 플레이어 공격관련
	std::array<Craft::Vector2F, 2> playerAttackPoint;

	// 타일 맵 참조
	std::weak_ptr<const Craft::TileMap> tileMap;

	// 플레이어 애니메이션
	std::vector<std::vector<Craft::PixelImage>> playerSpriteAnimation;


	// 플레이어 이미지 파일이름
	const std::string playerIdleFilename = "Actors/Player/player_idle.txt";
	const std::string playerSwimFilename = "Actors/Player/player_swim.txt";
};

