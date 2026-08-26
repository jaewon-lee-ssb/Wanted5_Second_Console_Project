#pragma once

#include <Utility/CollisionContext.h>

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
		void ProcessCollision(Utility::CollisionContext& collisionContext, std::vector<std::shared_ptr<Actor>>& actorList);

		// 충돌 가능여부 검사
		bool CanCollide(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right);

	private:
		// 두 액터가 서로 충돌했는지 확인하는 함수.
		bool TestSwept(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right);
		bool TestAABB(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right);
	};
}


