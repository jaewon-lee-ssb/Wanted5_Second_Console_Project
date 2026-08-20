#pragma once

#include <Actor/Actor.h>

#include <vector>
#include <memory>

using namespace Craft;

namespace Utility
{
	struct CollisionContext
	{
		std::weak_ptr<Actor> player;

		std::vector<std::weak_ptr<Actor>> enemyList;
		std::vector<std::weak_ptr<Actor>> itemList;
		std::vector<std::weak_ptr<Actor>> playerAttackList;
		std::vector<std::weak_ptr<Actor>> enemyAttackList;
		std::vector<std::weak_ptr<Actor>> bombList;
		std::vector<std::weak_ptr<Actor>> triggerList;
	};
}

