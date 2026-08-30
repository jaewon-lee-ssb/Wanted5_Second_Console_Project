#include <Actor/Enemy/Enemy.h>
#include <Actor/Player/Player.h>

#include <Level/Level.h>

Enemy::Enemy(const Craft::Vector2F& position)
	: super({}, position)
{
	enemySpriteAnimation.resize(static_cast<int>(EnemyState::Count));
}

void Enemy::BeginPlay()
{
	target = GetOwner()->FindActor<Player>();
}
