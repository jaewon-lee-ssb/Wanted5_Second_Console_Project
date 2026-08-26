#include <Camera/Camera.h>
#include <Actor/Actor.h>
#include <Engine/Engine.h>

#include <algorithm>

namespace Craft
{
	Camera::Camera()
	{
		offset = Vector2F((Engine::Get().GetWidth() / 2.f), (Engine::Get().GetHeight() / 2.f));
	}

	void Camera::SetTarget(const std::shared_ptr<Actor>& target)
	{
		this->target = target;
	}

	Vector2F Camera::WorldToScreen(const Vector2F& world) const
	{
		// 월드 에서 카메라를 뺴주면 화면 좌표가 나옴
		return world - position;
	}

	Vector2F Camera::ScreenToWorld(const Vector2F& screen) const
	{
		// 화면 = 월드 - 카메라니까
		// 화면 + 카메라 = 월드
		return screen + position;
	}

	void Camera::Tick(float deltaTime)
	{
		// 타겟을 따라다니도록 타겟의 위치를 업데이트
		if (auto player = target.lock())
		{
			Vector2F newPosition = player->GetPosition() - offset;
		
			position.x = (std::clamp)(newPosition.x, 0.f, mapSize.x - Engine::Get().GetWidth());
			position.y = (std::clamp)(newPosition.y, 0.f, mapSize.y - Engine::Get().GetHeight());
		}
	}
}