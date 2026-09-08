#include <Actor/Player/Player.h>

#include <Actor/Player/Harpoon.h>
#include <Actor/Enemy/Enemy.h>

#include <Inventory/Inventory.h>
#include <Item/Item.h>

#include <Engine/Engine.h>
#include <Input/Input.h>
#include <Math/Vector2.h>
#include <Level/Level.h>
#include <Camera/Camera.h>
#include <Resource/TextImageLoader.h>
#include <Collision/GameCollisionLayers.h>
#include <World/TileMap.h>

#include <Windows.h>
#include <iomanip>
#include <sstream>
#include <memory>

using namespace Craft;

Player::Player(const Vector2F& position)
	: Actor(PixelImage{}, position, Utility::ActorTags::Player)
{
	playerSpriteAnimation.resize(static_cast<int>(PlayerState::Count));

	// 플레이어 이미지 지정
	playerSpriteAnimation[static_cast<int>(PlayerState::Idle)] = (TextImageLoader::LoadAnimation(playerIdleFilename));

	playerSpriteAnimation[static_cast<int>(PlayerState::Swim)] = (TextImageLoader::LoadAnimation(playerSwimFilename));

	playerSpriteAnimation[static_cast<int>(PlayerState::Dead)] = (TextImageLoader::LoadAnimation(playerDeadFilename));

	currentStateIndex = static_cast<int>(PlayerState::Idle);
	ChangeImage(playerSpriteAnimation[currentStateIndex][0]);

	animationTimer.SetTargetTime(animationFrameTime);
	attackCooldownTimer.SetTargetTime(3.f);
	attackLockTimer.SetTargetTime(0.5f);
	damagedTimer.SetTargetTime(3.f);

	// 0 - 왼쪽
	playerAttackPoint[0] = Vector2F(GetPosition().x - GetPivot().x, GetPosition().y);
	// 1 - 오른쪽
	playerAttackPoint[1] = Vector2F(GetPosition().x + GetPivot().x, GetPosition().y);

	SetCollisionLayer(GameCollision::Player);
	SetCollisionMask(GameCollision::EnemyAttack | GameCollision::Item);

}

void Player::TakeDamage(float damage)
{
	if (isDamaged)
	{
		return;
	}

	Craft::Engine::Get().PlayOneShot("playerdamaged.wav");

	damagedTimer.Reset();
	Hp -= damage;
	isDamaged = true;

	if (Hp <= 0.f)
	{
		Hp = 0.f;
		currentStateIndex = static_cast<int>(PlayerState::Dead);
	}
}

bool Player::TryAddItem(const std::shared_ptr<Item>& item)
{
	auto targetInventory = inventory.lock();

	if (!targetInventory || !item)
	{
		return false;
	}

	return targetInventory->AddItem(item);
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	animationTimer.Tick(deltaTime);
	attackCooldownTimer.Tick(deltaTime);
	attackLockTimer.Tick(deltaTime);
	damagedTimer.Tick(deltaTime);



	if (!playerSpriteAnimation[currentStateIndex].empty() && animationTimer.IsTimeOut())
	{
		animationTimer.Reset();

		currentAnimationSpriteIndex = (currentAnimationSpriteIndex + 1) % playerSpriteAnimation[currentStateIndex].size();

		ChangeImage(playerSpriteAnimation[currentStateIndex][currentAnimationSpriteIndex]);
	}

	// 종료 처리.
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		QuitGame();
	}

	if (Hp <= 0.f)
	{
		return;
	}

	if (damagedTimer.IsTimeOut())
	{
		isDamaged = false;
	}


	// 공격직후 멈추는 타이머
	if (!attackLockTimer.IsTimeOut())
	{
		isAiming = false;
		GetOwner()->GetCamera()->SetZoom(1.f);
		return;
	}

	
	isAiming = Input::Get().GetKey(VK_RBUTTON);

	if (!attackCooldownTimer.IsTimeOut())
	{
		isAiming = false;
	}

	// 마우스는 직전에 표시된 화면의 카메라 변환으로 월드 좌표를 구한다.
	Vector2F mousePosition = Input::Get().GetMousePosition();

	mousePosition = GetOwner()->GetCamera()->ScreenToWorld(mousePosition);

	GetOwner()->GetCamera()->SetZoom(isAiming ? 2.f : 1.f);

	playerMoveDir = Vector2F::Zero;
	if (!isAiming)
	{
		if (Input::Get().GetKey('D'))
		{
			playerMoveDir.x = 1.0f;
			flipX = true;
		}
		if (Input::Get().GetKey('A'))
		{
			playerMoveDir.x = -1.0f;
			flipX = false;
		}
		if (Input::Get().GetKey('W'))
		{
			playerMoveDir.y = -1.0f;
		}
		if (Input::Get().GetKey('S'))
		{
			playerMoveDir.y = 1.0f;
		}
		if (std::abs(playerMoveDir.Length()) > 0)
		{
			currentStateIndex = static_cast<int>(PlayerState::Swim);
		}
		else
		{
			currentStateIndex = static_cast<int>(PlayerState::Idle);
		}
	}

	

	if (Input::Get().GetKeyDown(VK_LBUTTON) && isAiming && attackCooldownTimer.IsTimeOut())
	{
		Craft::Engine::Get().PlayOneShot("weapon.wav");
		attackCooldownTimer.Reset();
		attackLockTimer.Reset();
		isAiming = false;
		GetOwner()->GetCamera()->SetZoom(1.f);

		// 방향 체크해서 공격위치 조정
		Vector2F attackPosition = mousePosition.x < position.x ? playerAttackPoint[0] : playerAttackPoint[1];
		flipX = mousePosition.x < position.x ? false : true;

		// 마우스 위치 예외 처리.
		if (attackPosition == mousePosition)
		{
			Vector2F tempPosition = mousePosition - position;
			mousePosition.x += tempPosition.x;
			mousePosition.y += tempPosition.y;
		}
		if (mousePosition.x < playerAttackPoint[1].x && mousePosition.x > playerAttackPoint[0].x)
		{
			Vector2F tempPosition = mousePosition - position;
			mousePosition.x = attackPosition.x + tempPosition.x;
			mousePosition.y = attackPosition.y + tempPosition.y;
		}

		auto harpoon = GetOwner()->SpawnActor<Harpoon>(attackPosition, mousePosition, harpoonDamage);

		Craft::RaycastHit hit;

		if (GetOwner()->RaycastSegment(harpoon->GetSegmentStart(), harpoon->GetSegmentEnd(), GameCollision::Enemy, hit))
		{
			auto enemy = Cast<Enemy>(hit.actor);

			if (enemy)
			{
				enemy->TakeDamage(harpoon->GetDamage());
			}
		}
	}

	Move(deltaTime);
}

void Player::Move(float deltaTime)
{
	if (playerMoveDir.Length() <= 0)
	{
		return;
	}

	auto map = tileMap.lock();
	if (!map)
	{
		return;
	}

	Vector2F movement = Vector2F::Zero;

	Vector2F newPosition = GetPosition();

	movement.x = playerMoveDir.Normalize().x * playerMoveSpeedX * deltaTime;
	newPosition.x += movement.x;

	if (map->CanOccupyWorld(GetBoundsAt(newPosition)))
	{
		SetPosition(newPosition);
	}
	else
	{
		newPosition.x = position.x;
	}

	movement.y = playerMoveDir.Normalize().y * playerMoveSpeedY * deltaTime;
	newPosition.y += movement.y;
	if (map->CanOccupyWorld(GetBoundsAt(newPosition)))
	{
		SetPosition(newPosition);
	}
	else
	{
		newPosition.y = position.y;
	}

	SetPosition(newPosition);

	// 공격 위치 업데이트
	// 0 - 왼쪽
	playerAttackPoint[0] = Vector2F(GetPosition().x - GetPivot().x, GetPosition().y);
	// 1 - 오른쪽
	playerAttackPoint[1] = Vector2F(GetPosition().x + GetPivot().x, GetPosition().y);
}
