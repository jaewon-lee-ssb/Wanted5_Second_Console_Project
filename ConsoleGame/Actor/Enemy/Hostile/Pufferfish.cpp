#include <Actor/Enemy/Hostile/Pufferfish.h>

#include <Resource/TextImageLoader.h>
#include <Utility/Random.h>

Pufferfish::Pufferfish(const Craft::Vector2F& position)
	: super(position)
{
	enemyState = EnemyState::Patrol;
	enemySpriteAnimation[static_cast<int>(EnemyState::Patrol)] = Craft::TextImageLoader::LoadAnimation(enemyPatrolFilename);

	ChangeImage(enemySpriteAnimation[enemyState][0]);


	// 속도 지정
	enemyMoveSpeed = 50.f;

	patrolOrigin = position;

	// 패트롤 지연타임
	patrolRetryInterval = Utility::RandomRange(0.f, 0.5f);
}

Craft::Bounds Pufferfish::GetSpawnBounds(const Craft::Vector2F& position)
{
	return Craft::Bounds();
}

void Pufferfish::BeginPlay()
{
	super::BeginPlay();
}

void Pufferfish::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	
}

void Pufferfish::UpdateState(float deltaTime)
{
	super::UpdateState(deltaTime);

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

void Pufferfish::UpdatePatrol(float deltaTime)
{
	
}

void Pufferfish::UpdateChase(float deltaTime)
{

}

void Pufferfish::UpdateAttack(float deltaTime)
{
}

void Pufferfish::UpdateFlee(float deltaTime)
{
}

void Pufferfish::UpdateReturn(float deltaTime)
{
	
}

void Pufferfish::UpdateDead(float deltaTime)
{

}

