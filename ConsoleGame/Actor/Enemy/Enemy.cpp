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

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	UpdateState(deltaTime);

	UpdateAnimation(deltaTime);
}

void Enemy::UpdateState(float deltaTime)
{
	switch (enemyState)
	{
	case EnemyState::Patrol:
		UpdatePatrol(deltaTime);
		break;
	case EnemyState::Chase:
		UpdateChase(deltaTime);
		break;
	case EnemyState::Attack:
		UpdateAttack(deltaTime);
		break;
	case EnemyState::Flee:
		UpdateFlee(deltaTime);
		break;
	case EnemyState::Return:
		UpdateReturn(deltaTime);
		break;
	case EnemyState::Dead:
		UpdateDead(deltaTime);
		break;
	}
}

void Enemy::UpdatePatrol(float deltaTime)
{
}

void Enemy::UpdateChase(float deltaTime)
{
}

void Enemy::UpdateAttack(float deltaTime)
{
}

void Enemy::UpdateFlee(float deltaTime)
{
}

void Enemy::UpdateReturn(float deltaTime)
{
}

void Enemy::UpdateDead(float deltaTime)
{
}

void Enemy::UpdateAnimation(float deltaTime)
{
	// 애니메이션 시간 추가
	animationElapsedTime += deltaTime;


	// 애니메이션 프레임 교체
	if (!enemySpriteAnimation[currentStateIndex].empty() && animationElapsedTime >= animationFrameTime)
	{
		animationElapsedTime -= animationFrameTime;

		currentAnimationSpriteIndex = (currentAnimationSpriteIndex + 1) % enemySpriteAnimation[currentStateIndex].size();

		ChangeImage(enemySpriteAnimation[currentStateIndex][currentAnimationSpriteIndex]);
	}
}
