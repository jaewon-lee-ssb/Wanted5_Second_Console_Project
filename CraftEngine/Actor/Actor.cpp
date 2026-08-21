#include <Actor/Actor.h>
#include <Engine/Engine.h>
#include <Level/Level.h>
#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <Resource/PixelImage.h>

#include <Resource/TextImageLoader.h>


namespace Craft
{
	Actor::Actor(const PixelImage& image, const Vector2F& position, Utility::ActorTags actorTag)
		: image(image), position(position), actorTag(actorTag), pivot(Vector2F(image.width / 2, image.height / 2))
	{

	}

	Actor::~Actor()
	{

	}

	void Actor::BeginPlay()
	{
		// 중복 호출 방지를 위해 설정.
		hasBeganPlay = true;
	}

	void Actor::Tick(float deltaTime)
	{

	}

	void Actor::Draw()
	{
		// 비활성화 상태라면 처리를 안함.
		if (!IsActive())
		{
			return;
		}

		// 렌더러에 그릴 데이터 전달.
		if(GetOwner()->GetCamera())
		{ 
			Renderer::Get().SubmitWorld(image, GetOwner()->GetCamera()->WorldToScreen(position), pivot, sortingOrder);
		}
	}

	void Actor::OnCollision(const std::shared_ptr<Actor>& other)
	{
	}

	void Actor::Destroy()
	{
		// 액터 삭제 예약.
		// 다음 프레임에 액터가 레벨에서 제거됨.
		hasExpired = true;
	}

	void Actor::QuitGame()
	{
		// 엔진 종료 요청.
		Engine::Get().Quit();
	}

	void Actor::SavePreviousState()
	{
		previousPosition = position;
	}

	void Actor::SetPosition(const Vector2F& newPosition)
	{
		// 변경하려는 위치 값이 현재 위치와 같은지 비교.
		if (position == newPosition)
		{
			return;
		}

		// 엔진 업데이트
		position = newPosition;
	}

	inline Bounds Actor::GetBounds() const
	{
		const float left = std::floor(position.x - pivot.x);
		const float top = std::floor(position.y - pivot.y);

		return Bounds{ left, left + image.width, top, top + image.height };
	}

	bool Actor::IsSameGrid(const std::shared_ptr<Actor>& other) const
	{
		return std::floor(GetPosition().x) == std::floor(other->GetPosition().x)
			&& std::floor(GetPosition().y) == std::floor(other->GetPosition().y);
	}
}