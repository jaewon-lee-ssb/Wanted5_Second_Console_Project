#pragma once

#include <Core/Core.h>

#include <memory>

namespace Craft
{
	// Actor 기반의 클래스에서 타입 시스템을 제공하는 최상위 클래스.
	// 런타임(실행 중)에 타입 확인 및 형변환 기능을 제공.
	class CRAFT_API CraftObject
	{
	public:
		// 가상 소멸자.
		virtual ~CraftObject() = default;

		// 현재 객체의 타입의 ID를 반환하는 함수.
		// 함수 끝 부분에 = 0 을 붙여주면 순수 가상 함수로 만들 수 있음.
		// 특정 함수를 필수로 구현하도록 강제하고 싶을 때 활용.
		// C++에서 인터페이스를 사용할 때 이렇게 구성.
		virtual size_t GetType() const = 0;

		// 전달된 타입 ID와 현재 객체 비교.
		virtual bool Is(size_t id) const
		{
			return false;
		}

		// 타입 질문 함수.
		// 이 함수를 사용하기 위해서는 T 타입이 static TypeId 함수를 가져야만 함.
		template<typename T>
		bool IsTypeOf() const
		{
			return Is(T::TypeId());
		}

		// 스마트 포인터 간의 형변환 유틸리티 함수.
		template<typename T, typename U>
		std::shared_ptr<T> Cast(const std::shared_ptr<U>& object)
		{
			// 에외 처리
			if (!object)
			{
				return nullptr;
			}

			// object의 실제 타입이 T(또는 T의 파생)인지 확인 후 캐스팅(형변환).
			if (object->Is(T::TypeId()))
			{
				// 가벼운 형변환을 통해 반환
				// static_pointer_cast는 런타임에 타입 검증을 하지 않기 때문에
				// 상대적으로 가벼움.
				return std::static_pointer_cast<T>(object);
			}

			// 형변환이 허용되지 않는 경우에는 null 변환.
			return nullptr;
		}
	};
}

// 타입 시스템을 사용할 클래스(Actor타입)에 추가할 매크로.
#define TYPE_DECLARATIONS(Type, ParentType)							\
	using super = ParentType;										\
protected:															\
	static size_t TypeIdClass()										\
	{																\
		static int runTimeTypeId = 0;								\
		return reinterpret_cast<size_t>(&runTimeTypeId);			\
	}																\
public:																\
	static size_t TypeId()											\
	{																\
		return Type::TypeIdClass();									\
	}																\
	virtual size_t GetType() const override							\
	{																\
		return Type::TypeIdClass();									\
	}																\
	virtual bool Is(size_t id) const override						\
	{																\
		return (id == TypeIdClass()) ? true : ParentType::Is(id);	\
	}																