#include <Camera/Camera.h>
#include <Actor/Actor.h>
#include <Engine/Engine.h>

#include <algorithm>

namespace Craft
{
	Camera::Camera()
	{
	}

	void Camera::SetTarget(const std::shared_ptr<Actor>& target)
	{
		this->target = target;
	}

	Vector2F Camera::WorldToScreen(const Vector2F& world) const
	{
		return (world - position) * zoom;
	}

	Vector2F Camera::ScreenToWorld(const Vector2F& screen) const
	{
		return screen * (1.f / zoom) + position;
	}

	void Camera::SetZoom(float newZoom)
	{
		zoom = (std::max)(0.1f, newZoom);
	}

	void Camera::Tick(float deltaTime)
	{
		// 타겟을 따라다니도록 타겟의 위치를 업데이트
		if (auto player = target.lock())
		{
			// 확대할수록 화면에 보이는 월드 범위가 줄어든다.
			const Vector2F visibleWorldSize(Engine::Get().GetWidth() / zoom, Engine::Get().GetHeight() / zoom);
			const Vector2F cameraOffset = visibleWorldSize * 0.5f;
			const Vector2F newPosition = player->GetPosition() - cameraOffset;
			const float maxCameraX = (std::max)(0.f, mapSize.x - visibleWorldSize.x);
			const float maxCameraY = (std::max)(0.f, mapSize.y - visibleWorldSize.y);
		
			position.x = (std::clamp)(newPosition.x, 0.f, maxCameraX);
			position.y = (std::clamp)(newPosition.y, 0.f, maxCameraY);
		}
	}
}
