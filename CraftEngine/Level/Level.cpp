#include <Level/Level.h>
#include <Camera/Camera.h>

#include <Physics/CollisionSystem.h>


namespace Craft
{
	Level::Level()
	{

	}

	Level::~Level()
	{

	}

	// 레벨 초기화 이벤트 함수.
	void Level::OnInitialized()
	{
		hasInitialized = true;

		camera = std::make_unique<Camera>();
	}

	// 프레임 이벤트 함수.
	void Level::BeginPlay()
	{
		// 액터 초기화시 1번만 호출되는 이벤트
		for (std::shared_ptr<Actor>& actor : actorList)
		{
			// 이미 BeginPlay가 호출된 액터는 건너뛰기.
			if (actor->HasBeganPlay())
			{
				continue;
			}

			actor->BeginPlay();
		}
	}

	void Level::Tick(float deltaTime)
	{
		for (std::shared_ptr<Actor>& actor : actorList)
		{
			//비활성화 된 액터는 건너뛰기
			if (!actor->IsActive())
			{
				continue;
			}
			actor->Tick(deltaTime);
		}

		camera->Tick(deltaTime);
	}

	void Level::Draw()
	{
		for (std::shared_ptr<Actor>& actor : actorList)
		{
			//비활성화 된 액터는 건너뛰기
			if (!actor->IsActive())
			{
				continue;
			}
			actor->Draw();
		}
	}

	bool Level::RaycastSegment(const Vector2F& start, const Vector2F& end, CollisionLayer targetMask, RaycastHit& outHit) const
	{
		return CollisionSystem::RaycastSegment(start, end, targetMask, actorList, outHit);
	}

	void Level::ProcessAddAndDestroyActors()
	{
		if (HasInitialized())
		{
			firstActorAdd = true;
		}
		// 액터 제거 처리.
		for (auto iterator = actorList.begin(); iterator != actorList.end();)
		{
			// 제거 요청 여부 확인.
			if ((*iterator)->HasExpired())
			{
				iterator = actorList.erase(iterator);
				continue;
			}
			++iterator;
		}

		// 액터 추가 처리.

		// 추가요청된 목록이 있는지 확인.
		if (addRequestedActorList.empty())
		{
			return;
		}

		// 추가 요청 처리.
		for (const std::shared_ptr<Actor>& actor : addRequestedActorList)
		{
			actorList.emplace_back(actor);
			
		}

		// 정리.
		addRequestedActorList.clear();
	}

	void Level::SavePreviousActorStates()
	{
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (!actor->IsActive())
			{
				continue;
			}
			
			actor->SavePreviousState();
		}
	}

}

