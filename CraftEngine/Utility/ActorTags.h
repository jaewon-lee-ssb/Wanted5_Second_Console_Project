#pragma once

#include <Core/Core.h>

namespace Utility
{
	enum class CRAFT_API ActorTags
	{
		None = 0,
		Player,
		Enemy,
		Block,
		WaterBomb,
		PlayerAttackBox,
		EnemyAttackBox,
		Item,
		Trigger
	};

	// 현재 게임에 있는 충돌체크
	// 플레이어 <-> 아이템
	// 플레이어 <-> 적공격
	// 적 <-> 플레이어공격
	// 폭탄 <-> 폭발
	
}