#include <Physics/CollisionSystem.h>
#include <Actor/Actor.h>
#include <Physics/Bounds.h>

#include <algorithm>

namespace Craft
{
	void CollisionSystem::ProcessCollision(Utility::CollisionContext& collisionContext, std::vector<std::shared_ptr<Actor>> actorList)
	{
		// 충돌한 액터에 이벤트를 전달하기 위한 배열.
		std::vector<CollisionPair> collidedActorList;

		// 레벨에 배치된 액터 수.
		
		for (const std::shared_ptr<Actor>& left : actorList)
		{
			for (const std::shared_ptr<Actor>& right : actorList)
			{
				if (left == right)
				{
					continue;
				}

				CanCollide(left, right);
			}
		}

		std::shared_ptr<Actor> player = collisionContext.player.lock();

		if (!player)
		{
			return;
		}

		// 플레이어 <-> 아이템
		for (const auto& weakItem : collisionContext.itemList)
		{
			if (auto item = weakItem.lock())
			{
				if (TestAABB(player, item))
				{
					// 목록에 추가
					collidedActorList.emplace_back(player, item);
				}
			}
		}

		// 플레이어 <-> 적공격
		for (const auto& weakDamageBox : collisionContext.enemyAttackList)
		{
			if (auto damageBox = weakDamageBox.lock())
			{
				if (TestAABB(player, damageBox))
				{
					collidedActorList.emplace_back(player, damageBox);
				}
			}
		}


		for (const auto& weakPlayerAttack : collisionContext.playerAttackList)
		{
			if (auto playerAttack = weakPlayerAttack.lock())
			{
				// 적 <-> 플레이어공격
				for (const auto& weakEnemy : collisionContext.enemyList)
				{
					if (auto enemy = weakEnemy.lock())
					{
						if (TestAABB(enemy, playerAttack))
						{
							collidedActorList.emplace_back(enemy, playerAttack);
						}
					}
				}

				// 플레이어 공격 <-> 폭탄
				for (const auto& weakBomb : collisionContext.bombList)
				{
					if (auto bomb = weakBomb.lock())
					{
						if(TestAABB(playerAttack, bomb))
						{
							// 목록에 추가
							collidedActorList.emplace_back(playerAttack, bomb);
						}
					}
				}
			}
		}
		//트리거 <-> 플레이어
		for (const auto& triggerBox : collisionContext.triggerList)
		{
			auto trigger = triggerBox.lock();
			if (TestAABB(player, trigger))
			{
				collidedActorList.emplace_back(player, trigger);
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

	bool CollisionSystem::TestSwept(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right)
	{
		// 예외 처리
		if (!left || !left->IsActive() || !right || !right->IsActive())
		{
			return false;
		}

		// AABB (Axis Aligned Bounding Box)
		// x의 크기와 y크기를 고려해 박스를 구성하고 두 박스가 서로 겹치는지 확인하는 방법.

		// left 액터의 현재/이전 위치.
		const Vector2F leftCurrent = left->GetPosition();
		const Vector2F leftPrevious = left->GetPreviousPosition();
		
		// right 액터의 현재/이전 위치.
		const Vector2F rightCurrent = right->GetPosition();
		const Vector2F rightPrevious = right->GetPreviousPosition();

		// 이전 프레임 위치와 현재 위치를 모두 포함하는 바운드(충돌 영역) 계산.
		const float leftXMin = min(leftCurrent.x, leftPrevious.x);
		const float leftXMax = max(leftCurrent.x + left->GetWidth(), leftPrevious.x + left->GetWidth());

		// 이전 프레임까지 고려한 y충돌 영역 계산.
		const float leftYMin = min(leftCurrent.y, leftPrevious.y);
		const float leftYMax = max(leftCurrent.y + 1, leftPrevious.y + 1);
			  
		const float rightXMin = min(rightCurrent.x, rightPrevious.x);
		const float rightXMax = max(rightCurrent.x + right->GetWidth(), rightPrevious.x + right->GetWidth());

		const float rightYMin = min(rightCurrent.y, rightPrevious.y);
		const float rightYMax = max(rightCurrent.y + 1, rightPrevious.y + 1);

		// x좌표 기준 비교.
		
		if (leftXMax <= rightXMin || rightXMax <= leftXMin)
		{
			return false;
		}


		// y좌표 기준으로 비교.
		if (leftYMax <= rightYMin || rightYMax <= leftYMin)
		{
			return false;
		}

		// 충돌 발생
		return true;
	}

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

}
