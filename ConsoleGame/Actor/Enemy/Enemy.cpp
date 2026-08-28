#include <Actor/Enemy/Enemy.h>

Enemy::Enemy(const Craft::Vector2F& position)
	: super({}, position)
{
	enemySpriteAnimation.resize(static_cast<int>(EnemyState::Count));
}
