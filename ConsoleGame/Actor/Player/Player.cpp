#include <Actor/Player/Player.h>

#include <Actor/Player/Harpoon.h>

#include <Input/Input.h>
#include <Math/Vector2.h>
#include <Level/Level.h>
#include <Camera/Camera.h>
#include <Resource/TextImageLoader.h>

#include <Collision/GameCollisionLayers.h>

#include <Windows.h>
#include <iomanip>
#include <sstream>
#include <memory>

using namespace Craft;

Player::Player(const Vector2F& position)
	: Actor(PixelImage{}, position, Utility::ActorTags::Player)
{
	// 플레이어 이미지 지정
	playerIdleImage = TextImageLoader::Load(playerIdleFilename);
	ChangeImage(playerIdleImage);

	// 0 - 왼쪽
	playerAttackPoint[0] = Vector2F(GetPosition().x - GetPivot().x, GetPosition().y);
	// 1 - 오른쪽
	playerAttackPoint[1] = Vector2F(GetPosition().x + GetPivot().x, GetPosition().y);

	SetCollisionLayer(GameCollision::Player);
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);



	// 종료 처리.
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		QuitGame();
	}


	/*Vector2F mouseScreenPosition = Input::Get().GetMousePressedPosition();

	Vector2F mouseWorldPosition = GetOwner()->GetCamera()->ScreenToWorld(mouseScreenPosition);
	Vector2F playerScreenPosition = GetOwner()->GetCamera()->WorldToScreen(position);

	const bool attackDirection = mouseScreenPosition.x < playerScreenPosition.x;*/


	Vector2F mousePosition = Input::Get().GetMousePosition();

	mousePosition = GetOwner()->GetCamera()->ScreenToWorld(mousePosition);

	/*std::ostringstream title;
	title << std::fixed << std::setprecision(2)
		<< "X: " << mousePosition.x << " | Y: " << mousePosition.y;

	SetConsoleTitleA(title.str().c_str());*/


	playerMoveDir = Vector2F::Zero;
	if (Input::Get().GetKey('D'))
	{
		playerMoveDir.x = 1.0f;
	}
	if (Input::Get().GetKey('A'))
	{
		playerMoveDir.x = -1.0f;
	}
	if (Input::Get().GetKey('W'))
	{
		playerMoveDir.y = -1.0f;
	}
	if (Input::Get().GetKey('S'))
	{
		playerMoveDir.y = 1.0f;
	}

	if (Input::Get().GetKeyDown(VK_LBUTTON))
	{
		// 방향 체크해서 공격위치 조정
		Vector2F attackPosition = mousePosition.x < position.x ? playerAttackPoint[0] : playerAttackPoint[1];

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

		GetOwner()->SpawnActor<Harpoon>(attackPosition, mousePosition);
	}

	Move(deltaTime);
}

void Player::Move(float deltaTime)
{
	if (playerMoveDir.Length() <= 0)
	{
		return;
	}

	Vector2F newPosition = GetPosition();

	newPosition = newPosition + playerMoveDir.Normalize() * playerMoveSpeed * deltaTime;

	SetPosition(newPosition);

	// 공격 위치 업데이트
	// 0 - 왼쪽
	playerAttackPoint[0] = Vector2F(GetPosition().x - GetPivot().x, GetPosition().y);
	// 1 - 오른쪽
	playerAttackPoint[1] = Vector2F(GetPosition().x + GetPivot().x, GetPosition().y);
}
