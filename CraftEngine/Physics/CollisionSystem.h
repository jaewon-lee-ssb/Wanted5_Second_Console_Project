#pragma once

#include <Physics/RaycastHit.h>
#include <Physics/CollisionLayer.h>
#include <Physics/Bounds.h>

#include <vector>
#include <memory>


namespace Craft
{
	class Actor;

	class CollisionSystem
	{
		// 충돌 이벤트 발행을 위해 액터를 임시 저장할 구조체.
		struct CollisionPair
		{
			std::shared_ptr<Actor> actor;
			std::shared_ptr<Actor> other;
		};
		
	public:
		CollisionSystem() = default;
		~CollisionSystem() = default;

		// 액터를 순회하면서 충돌을 확인하는 함수.
		void ProcessCollision(std::vector<std::shared_ptr<Actor>>& actorList);

		// 충돌 가능여부 검사
		bool CanCollide(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right);

		// 목록 전체를 검사해서 가장 먼저 닿는 액터 반환.
		static bool RaycastSegment(const Vector2F& start, const Vector2F& end, CollisionLayer targetMask, const std::vector<std::shared_ptr<Actor>>& actors, RaycastHit& outHit);


	private:
		// 두 액터가 서로 충돌했는지 확인하는 함수.
		//bool TestSwept(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right);
		bool TestAABB(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right);

		// 선분과 박스 하나의 교차 검사
		static bool TestSegmentAABB(const Vector2F& start, const Vector2F& end, const Bounds& bounds, float& outHitT);
	};
}


