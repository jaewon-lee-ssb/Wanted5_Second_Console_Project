#include <Physics/CollisionSystem.h>
#include <Actor/Actor.h>
#include <Physics/Bounds.h>

#include <algorithm>

namespace Craft
{
	void CollisionSystem::ProcessCollision(std::vector<std::shared_ptr<Actor>>& actorList)
	{
		// 충돌한 액터에 이벤트를 전달하기 위한 배열.
		std::vector<CollisionPair> collidedActorList;

		for (size_t ix = 0; ix < actorList.size(); ++ix)
		{
			const auto& left = actorList[ix];
			if (!left || !left->IsActive())
			{
				continue;
			}

			for (size_t jx = ix + 1; jx < actorList.size(); ++jx)
			{
				const auto& right = actorList[jx];
				if (!right || !right->IsActive())
				{
					continue;
				}

				if (!CanCollide(left, right))
				{
					continue;
				}

				if (TestAABB(left, right))
				{
					collidedActorList.push_back({ left, right });
				}
			}
		}

		// 충돌 발생한 액터 목록 확인. 충돌한 액터가 없으면 함수 반환.
		if (collidedActorList.empty())
		{
			return;
		}

		// 충돌한 액터에 이벤트 전달.
		for (const CollisionPair& pair : collidedActorList)
		{
			// 예외처리. 이미 삭제되었거나 비활성화된 액터는 제외
			if (!pair.actor->IsActive() || !pair.other->IsActive())
			{
				continue;
			}

			// 충돌 이벤트 전달
			pair.actor->OnCollision(pair.other);
			pair.other->OnCollision(pair.actor);
		}

	}

	bool CollisionSystem::CanCollide(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right)
	{
		if (!left || !right)
		{
			return false;
		}

		bool leftAcceptsRight = (left->GetCollisionMask() & right->GetCollisionLayer()) != 0;

		bool rightAcceptsRight = (right->GetCollisionMask() & left->GetCollisionLayer()) != 0;

		return leftAcceptsRight && rightAcceptsRight;
	}

	bool CollisionSystem::RaycastSegment(const Vector2F& start, const Vector2F& end, CollisionLayer targetMask, const std::vector<std::shared_ptr<Actor>>& actors, RaycastHit& outHit)
	{
		outHit = {};

		float nearestT = 2.f;

		for (const auto& actor : actors)
		{
			if (!actor || !actor->IsActive())
			{
				continue;
			}

			if ((targetMask & actor->GetCollisionLayer()) == 0)
			{
				continue;
			}

			float hitT = 0.f;

			if (TestSegmentAABB(start, end, actor->GetBounds(), hitT) && hitT < nearestT)
			{
				nearestT = hitT;
				outHit.actor = actor;
				outHit.t = hitT;
				outHit.point = start + (end - start) * hitT;
			}
		}

		return outHit.actor != nullptr;
	}

	/* {
		//bool CollisionSystem::TestSwept(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right)
		//{
		//	// 예외 처리
		//	if (!left || !left->IsActive() || !right || !right->IsActive())
		//	{
		//		return false;
		//	}

		//	// AABB (Axis Aligned Bounding Box)
		//	// x의 크기와 y크기를 고려해 박스를 구성하고 두 박스가 서로 겹치는지 확인하는 방법.

		//	// left 액터의 현재/이전 위치.
		//	const Vector2F leftCurrent = left->GetPosition();
		//	const Vector2F leftPrevious = left->GetPreviousPosition();
		//	
		//	// right 액터의 현재/이전 위치.
		//	const Vector2F rightCurrent = right->GetPosition();
		//	const Vector2F rightPrevious = right->GetPreviousPosition();

		//	// 이전 프레임 위치와 현재 위치를 모두 포함하는 바운드(충돌 영역) 계산.
		//	const float leftXMin = std::min(leftCurrent.x, leftPrevious.x);
		//	const float leftXMax = std::max(leftCurrent.x + left->GetWidth(), leftPrevious.x + left->GetWidth());

		//	// 이전 프레임까지 고려한 y충돌 영역 계산.
		//	const float leftYMin = std::min(leftCurrent.y, leftPrevious.y);
		//	const float leftYMax = std::max(leftCurrent.y + 1, leftPrevious.y + 1);
		//		  
		//	const float rightXMin = std::min(rightCurrent.x, rightPrevious.x);
		//	const float rightXMax = std::max(rightCurrent.x + right->GetWidth(), rightPrevious.x + right->GetWidth());

		//	const float rightYMin = std::min(rightCurrent.y, rightPrevious.y);
		//	const float rightYMax = std::max(rightCurrent.y + 1, rightPrevious.y + 1);

		//	// x좌표 기준 비교.
		//	
		//	if (leftXMax <= rightXMin || rightXMax <= leftXMin)
		//	{
		//		return false;
		//	}


		//	// y좌표 기준으로 비교.
		//	if (leftYMax <= rightYMin || rightYMax <= leftYMin)
		//	{
		//		return false;
		//	}

		//	// 충돌 발생
		//	return true;
		//}
	}*/

	bool CollisionSystem::TestAABB(const std::shared_ptr<Actor>& lhs, const std::shared_ptr<Actor>& rhs)
	{
		// 예외 처리
		if (!lhs || !lhs->IsActive() || !rhs || !rhs->IsActive())
		{
			return false;
		}

		Bounds lhsBound = lhs->GetBounds();
		Bounds rhsBound = rhs->GetBounds();


		if (lhsBound.left >= rhsBound.right || lhsBound.right <= rhsBound.left)
		{
			return false;
		}

		if (lhsBound.top >= rhsBound.bottom || lhsBound.bottom <= rhsBound.top)
		{
			return false;
		}

		return true;
	}

	bool CollisionSystem::TestSegmentAABB(const Vector2F& start, const Vector2F& end, const Bounds& bounds, float& outHitT)
	{
		const Craft::Vector2F direction = end - start;

		float tMin = 0.f;
		float tMax = 1.f;

		auto TestAxis = [&tMin, &tMax](
			float start,
			float direction,
			float minimum,
			float maximum)
			{
				constexpr float epsilon = 0.00001f;

				// 해당 축으로 움직이지 않는 선분
				if (std::abs(direction) < epsilon)
				{
					return start >= minimum && start <= maximum;
				}

				float t1 = (minimum - start) / direction;
				float t2 = (maximum - start) / direction;

				if (t1 > t2)
				{
					std::swap(t1, t2);
				}

				tMin = (std::max)(tMin, t1);
				tMax = (std::min)(tMax, t2);

				return tMin <= tMax;
			};

		if (!TestAxis(
			start.x,
			direction.x,
			bounds.left,
			bounds.right))
		{
			return false;
		}

		if (!TestAxis(
			start.y,
			direction.y,
			bounds.top,
			bounds.bottom))
		{
			return false;
		}

		outHitT = tMin;
		return true;
	}

}
