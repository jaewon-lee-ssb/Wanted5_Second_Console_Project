#pragma once

#include <Core/Core.h>
#include <Actor/Actor.h>

#include <vector>
#include <memory>
#include <cmath>

namespace Craft
{
	class Camera;

	// 모든 게임 레벨의 기반 클래스.
	// 게임에 배치되는 액터를 관리.
	// 액터에 프레임 이벤트를 전달하는 기능.
	// 액터에 필요한 부가 기능 제공.
	// : shared_from_this()  - this 포인터를 shared_ptr로 변환
	// : weak_from_this() - this 포인터를 weak_ptr로 변환.
	class CRAFT_API Level : public std::enable_shared_from_this<Level>
	{
		// Engine 프렌드 선언.
		friend class Engine;
		
	public:
		Level();
		virtual ~Level();

		// 레벨 초기화 이벤트 함수.
		virtual void OnInitialized();
		
		// 프레임 이벤트 함수.
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		inline const std::unique_ptr<Camera>& GetCamera() const
		{
			return camera;
		}

		inline std::shared_ptr<Actor> GetPlayer() const
		{
			return player.lock();
		}


		// 액터 생성 함수 템플릿.
		// 액터 생성 함수이므로 T는 Actor 하위 객체여야만 한다
		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>
		std::shared_ptr<T> SpawnActor(Args&& ...args)
		{
			// 새로운 액터 객체 생성.
			std::shared_ptr<T> newActor = std::make_shared<T>(std::forward<Args>(args)...);

			// 추가 요청 목록에 추가.
			addRequestedActorList.emplace_back(newActor);

			// 오너쉽 설정.
			newActor->SetOwner(weak_from_this());

			// 생성된 액터 반환
			return newActor;
		}

		// 원하는 액터를 찾는 함수
		template<typename T, typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>
		std::shared_ptr<T> FindActor()
		{
			// 레벨에 배치된 액터 목록 순회
			for (const std::shared_ptr<Actor>& actor : actorList)
			{
				// 원하는 타입의 액터를 찾았으면 형변환 후 반환.
				if (actor->IsTypeOf<T>())
				{
					return std::static_pointer_cast<T>(actor);
				}
			}

			// 못찾은 경우 null 반환
			return nullptr;
		}

		// 원하는 모든 액터를 찾는 함수
		template<typename T, typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>
		std::vector<std::shared_ptr<T>> FindActors()
		{
			std::vector<std::shared_ptr<T>> foundActors;

			for (const std::shared_ptr<Actor>& actor : actorList)
			{
				if (actor->IsTypeOf<T>())
				{
					foundActors.emplace_back(std::static_pointer_cast<T>(actor));
				}
			}
			return foundActors;
		}

		template<typename T, typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>
		std::shared_ptr<T> FindActorAt(const Vector2F& position)
		{
			for (const std::shared_ptr<Actor>& actor : actorList)
			{
				if (actor->IsTypeOf<T>() && actor->GetPosition() == Vector2F(std::floor(position.x),std::floor(position.y)))
				{
					return std::static_pointer_cast<T>(actor);
				}
			}

			return nullptr;
		}

		// Getter
		inline bool HasInitialized() const { return hasInitialized; }
		inline bool IsFirstActorAdd() const { return firstActorAdd; }

	protected:
		// 이전 프레임에 추가/제거 요청된 액터 처리 함수.
		void ProcessAddAndDestroyActors();

		// 액터의 이전 프레임 상태 저장 함수.
		void SavePreviousActorStates();

	protected:
		// 레벨 초기화 여부 플래그
		bool hasInitialized = false;

		// 레벨 시작 제일처음 초기화 한번만
		bool firstActorAdd = false;

		// 레벨에 배치된 모든 액터
		std::vector<std::shared_ptr<Actor>> actorList;

		

		// 레벨에 추가 요청된 액터 리스트.
		// 현재 프레임을 안정적으로 처리한 후에 액터 추가 처리.
		// 해당 액터를 바로 추가하면 기존 액터 처리에 문제가 발생할 수 있어서
		// 현재 프레임을 모두 처리한 후에 추가 요청된 액터를 actorList로 옮김.
		std::vector<std::shared_ptr<Actor>> addRequestedActorList;

		// 카메라 생성
		std::unique_ptr<Camera> camera;

	private:
		std::weak_ptr<Actor> player;

		std::vector<std::weak_ptr<Actor>> enemyList;

		std::vector<std::weak_ptr<Actor>> itemList;

		std::vector<std::weak_ptr<Actor>> playerAttackList;

		std::vector<std::weak_ptr<Actor>> enemyAttackList;

		std::vector<std::weak_ptr<Actor>> bombList;

		std::vector<std::weak_ptr<Actor>> triggerList;
	};
}