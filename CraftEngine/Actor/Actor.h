#pragma once

#include <Core/Core.h>
#include <Core/CraftObject.h>
#include <Math/Vector2.h>
#include <Math/Color.h>
#include <Physics/Bounds.h>
#include <Utility/ActorTags.h>
#include <Resource/PixelImage.h>
#include <Physics/CollisionLayer.h>

#include <memory>
#include <string>
#include <cmath>
#include <vector>

namespace Craft
{
	// 전방 선언.
	class Level;

	class CRAFT_API Actor : public CraftObject
	{
		// 타입 정보 설정을 위한 매크로 추가.
		TYPE_DECLARATIONS(Actor, CraftObject)

	public:
		Actor(const PixelImage& image, const Vector2F& position = Vector2F::Zero, Utility::ActorTags actorTag = Utility::ActorTags::None);
		virtual ~Actor();

		// 프레임 이벤트 함수.
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 충돌 시 호출될 충돌 이벤트 함수.
		virtual void OnCollision(const std::shared_ptr<Actor>& other);

		// 액터의 이미지 변경.
		inline void ChangeImage(const PixelImage& newImage)
		{
			// 새로운 이미지로 교체
			image = newImage;
			pivot = Vector2F(image.width / 2.0f, image.height / 2.0f);
		}

		// 액터를 레벨에서 제거할 때 사용할 함수.
		void Destroy();

		// 게임(엔진) 종료 요청 함수.
		void QuitGame();

		// 프레임 종료 후 현재 위치를 이전 위치로 저장하는 함수
		void SavePreviousState();

		// Getter / Setter
		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool IsActive() const { return isActive && !hasExpired; }
		inline bool HasExpired() const { return hasExpired; }

		inline std::shared_ptr<Level> GetOwner() const { return owner.lock(); }
		inline void SetOwner(std::weak_ptr<Level> newOwner) { owner = newOwner; }

		inline Vector2F GetPosition() const { return position; }
		void SetPosition(const Vector2F& newPosition);

		inline Vector2F GetPivot() const { return pivot; }
		//inline void SetPivot() { pivot.x = image.width / 2.f; pivot.y = image.height / 2.f; }

		// 이전 위치 반환 함수
		inline Vector2F GetPreviousPosition() const { return previousPosition; }

		// 액터의 문자열 너비 반환 함수.
		inline int GetWidth() const { return image.width; }
		inline int GetHeight() const { return image.height; }

		inline Bounds GetBounds() const;
		inline Bounds GetBoundsAt(const Vector2F& targetPosition) const;

		inline Utility::ActorTags GetActorTag() const { return actorTag; }

		// 충돌 레이어 마스크 관련 게터/세터
		inline CollisionLayer GetCollisionLayer() const { return collisionLayer; }
		inline CollisionLayer GetCollisionMask() const { return collisionMask; }

		inline void SetCollisionLayer(CollisionLayer layer) { collisionLayer = layer; }
		inline void SetCollisionMask(CollisionLayer mask) { collisionMask = mask; }


	protected:
		// BeginPlay 이벤트 처리 여부 플래그.
		bool hasBeganPlay = false;

		// 액터 활성화 여부 플래그
		bool isActive = true;

		// 액터 삭제 예약 설정 플래그
		bool hasExpired = false;

		// 오너쉽 - 액터를 소유하는 레벨 객체
		// weak_ptr -> 약참조 -> 실제 사용을 위해서는 해당위치가 유효한지 확인해야 함
		std::weak_ptr<Level> owner;

		// 화면에 그릴 글자(이미지).
		PixelImage image;

		// 2중 버퍼링 할때 추가할것
		// 그리기 정렬 순서.
		int sortingOrder = 0;

		// 액터 위치.
		Vector2F position;

		// 좌우반전
		bool flipX = false;

		// 피벗 위치
		Vector2F pivot;

		// 이전 프레임 액터 위치
		Vector2F previousPosition;

		// 액터의 태그
		Utility::ActorTags actorTag;

	private:
		CollisionLayer collisionLayer = Collision::None;
		CollisionLayer collisionMask = Collision::None;
	};
}
