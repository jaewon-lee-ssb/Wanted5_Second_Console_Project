#include <Actor/Enemy/Hostile/Pufferfish.h>

#include <Actor/Enemy/Hostile/EnemyAttackBox.h>

#include <Engine/Engine.h>
#include <Level/Level.h>
#include <Resource/TextImageLoader.h>
#include <World/TileMap.h>
#include <Utility/Random.h>

Pufferfish::Pufferfish(const Craft::Vector2F& position)
	: super(position)
{
	curState = EnemyState::Patrol;
	enemySpriteAnimation[static_cast<int>(EnemyState::Patrol)] = Craft::TextImageLoader::LoadAnimation(enemyPatrolFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Chase)] = Craft::TextImageLoader::LoadAnimation(enemyChaseFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Attack)] = Craft::TextImageLoader::LoadAnimation(enemyAttackFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Damaged)] = Craft::TextImageLoader::LoadAnimation(enemyDamagedFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Flee)] = Craft::TextImageLoader::LoadAnimation(enemyFleeFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Return)] = Craft::TextImageLoader::LoadAnimation(enemyPatrolFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Dead)] = Craft::TextImageLoader::LoadAnimation(enemyDeadFilename);

	ChangeImage(enemySpriteAnimation[static_cast<int>(curState)][0]);

	InitEnemy();
}

Craft::Bounds Pufferfish::GetSpawnBounds(const Craft::Vector2F& position)
{
	const float halfWidth = CollisionWidth * 0.5f;
	const float halfHeight = CollisionHeight * 0.5f;

	return Craft::Bounds(position.x - halfWidth, position.x + halfWidth, position.y - halfHeight, position.y + halfHeight);
}

void Pufferfish::BeginPlay()
{
	super::BeginPlay();
}

void Pufferfish::Tick(float deltaTime)
{
	super::Tick(deltaTime);


	//  TODO : 공격 범위 안에 들어왔으면 공격

	// 순찰중이거나 다시 돌아오는중에 플레이어를 찾으면 다시 도망
	if ((curState == EnemyState::Patrol || curState == EnemyState::Return) && DetectTarget())
	{
		ChangeEnemyState(EnemyState::Chase);

		ResetPath();
	}


	// 디버그 경로 출력
	if (auto map = tileMap.lock())
	{
		map->QueueDebugPath(movePath, currentPathIndex);
	}
	
}

void Pufferfish::UpdateState(float deltaTime)
{
	super::UpdateState(deltaTime);

	chaseTimer.Tick(deltaTime);
	attackCooltime.Tick(deltaTime);
	lifetimeTimer.Tick(deltaTime);

	if (lifetimeTimer.IsTimeOut())
	{
		lifetimeTimer.Reset();
		enemyMoveSpeed += 10.f;
	}

	switch (curState)
	{
	case EnemyState::Patrol:
		UpdatePatrol(deltaTime);
		break;
	case EnemyState::Chase:
		UpdateChase(deltaTime);
		break;
	case EnemyState::Damaged:
		UpdateDamaged(deltaTime);
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
	if (movePath.empty())
	{
		if (!enemyWaitTimer.IsTimeOut())
		{
			return;
		}

		Craft::Vector2F randPosition = GetPosition();
		if (FindRandomPatrolPoint(randPosition, patrolRadius))
		{
			// 랜덤 위치 찾을때 이미 검증을 하므로 검증 안해도됨
			FindPathTo(randPosition);
			enemyWaitTimer.SetTargetTime(Utility::RandomRange(0.f, patrolRetryInterval));
		}
	}
	else
	{
		FollowPath(deltaTime);
	}
}

void Pufferfish::UpdateChase(float deltaTime)
{
	// 체력이 적다면

	bool isMoveEnd = false; 
	bool isLowHp = false;

	if (Hp <= 50.f)
	{
		isLowHp = true;
	}

	// 체력이 적으면 player를 찾아도 도망
	if (isLowHp)
	{
		ChangeEnemyState(EnemyState::Flee);
	}

	// 공격체크
	if (AttackRangeCheck())
	{
		if (attackCooltime.IsTimeOut())
		{
			ChangeEnemyState(EnemyState::Attack);
			attackCooltime.Reset();
			enemyWaitTimer.SetTargetTime(0.2f* enemySpriteAnimation[static_cast<int>(EnemyState::Attack)].size());
			enemyWaitTimer.Reset();
			isAttackReady = true;
		}
		
	}



	// 이동은 계속 해준다
	if (!movePath.empty())
	{
		isMoveEnd = FollowPath(deltaTime);
	}

	if (movePath.empty() && !DetectTarget())
	{
		enemyWaitTimer.SetTargetTime(5.f);
	}

	if (movePath.empty() && !DetectTarget() && enemyWaitTimer.IsTimeOut())
	{
		ChangeEnemyState(EnemyState::Return);
		ResetPath();
		return;
	}

	auto target = targetPtr.lock();
	if (!target || !chaseTimer.IsTimeOut())
	{
		return;
	}
	

	// 찾지 못했으면 

	if (!DetectTarget())
	{

		return;
	}

	// 찾았으면 타겟의 위치 갱신
	if(isMoveEnd || movePath.empty())
	{ 
		chaseTimer.Reset();
		FindPathTo(target->GetPosition());
	}
}

void Pufferfish::UpdateAttack(float deltaTime)
{
	// 공격
	if (isAttackReady)
	{
		auto target = targetPtr.lock();
		if (!target)
		{
			return;
		}


		// 공격 준비 됐으면 공격
		isAttackReady = false;
		
		// 공격 액터 생성
		GetOwner()->SpawnActor<EnemyAttackBox>(target->GetPosition(), Craft::Vector2F(20.f, 10.f), attackDamage);
	}

	if (enemyWaitTimer.IsTimeOut())
	{
		ChangeEnemyState(EnemyState::Chase);

	}
}

void Pufferfish::UpdateDamaged(float deltaTime)
{
	
	if (enemyWaitTimer.IsTimeOut())
	{
		// 피격 애니메이션이 끝나면 도망상태로 
		ChangeEnemyState(EnemyState::Chase);
	}
}

void Pufferfish::UpdateFlee(float deltaTime)
{
	auto target = targetPtr.lock();
	auto map = tileMap.lock();

	if (!target || !map)
	{
		return;
	}

	const Craft::Vector2F difference = GetPosition() - target->GetPosition();

	flipX = difference.x < 0.f ? false : true;

	const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);
	const float fleeDistanceSquared = fleeEndDistance * fleeEndDistance;

	if (adjustedDistanceSquared >= fleeDistanceSquared)
	{
		ChangeEnemyState(EnemyState::Return);

		ResetPath();

		return;
	}

	if (adjustedDistanceSquared <= 0.f)
	{
		return;
	}

	const Craft::Vector2F direction = difference.Normalize();

	const Craft::Vector2F movement{ direction.x * enemyMoveSpeed * 2 * deltaTime, direction.y * enemyMoveSpeed * deltaTime };

	MoveWithTileCollision(movement);
}

void Pufferfish::UpdateReturn(float deltaTime)
{
	if (movePath.empty() )
	{
		if (!enemyWaitTimer.IsTimeOut())
		{
			return;
		}

		if (FindReturnPath())
		{
			enemyWaitTimer.SetTargetTime(Utility::RandomRange(0.f, patrolRetryInterval));
		}
	}
	else
	{
		FollowPath(deltaTime);
	}


	// 어느정도 거리안에 들어왔으면 다시 패트롤 시작

	const Craft::Vector2F difference = GetPosition() - patrolOrigin;

	const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);
	const float patrolRadiusSquared = patrolRadius * patrolRadius;

	// 패트롤 범위 내에 들어오면 다시 패트롤 시작
	if (adjustedDistanceSquared <= patrolRadiusSquared)
	{
		ChangeEnemyState(EnemyState::Patrol);

		ResetPath();

		return;
	}
}

void Pufferfish::UpdateDead(float deltaTime)
{
	ResetPath();
	if (!isDead)
	{
		Craft::Engine::Get().PlayOneShot("pufferfishdie.wav");
	}
	isDead = true;

	if (enemyWaitTimer.IsTimeOut())
	{
		DropItem();
		Destroy();
	}
}

void Pufferfish::InitEnemy()
{
	super::InitEnemy();

	// 이동속도
	enemyMoveSpeed = 30.f;

	// 체력
	Hp = 150.f;
	isDamaged = false;
	isDead = false;

	// 감지 범위
	detectRadius = 150.f;
	attackRadius = 28.f;

	// 패트롤 범위
	patrolRadius = 50.f;

	// 다시 패트롤하는 쿨타임 랜덤설정할거임
	patrolRetryInterval = 0.5f;

	// 이 거리가 넘으면 도망 종료
	fleeEndDistance = 200.f;

	enemyWaitTimer.SetTargetTime(Utility::RandomRange(0.f, patrolRetryInterval));
	chaseTimer.SetTargetTime(0.5f);
	attackCooltime.SetTargetTime(1.5f);
	lifetimeTimer.SetTargetTime(60.f);
}

bool Pufferfish::AttackRangeCheck()
{
	if (auto target = targetPtr.lock())
	{
		const Craft::Vector2F difference = target->GetPosition() - GetPosition();

		// 타겟과의 차이 거리 제곱
		const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);

		// 감지 범위의 길이 제곱
		const float attackDistanceSquared = attackRadius * attackRadius;

		if (adjustedDistanceSquared <= attackDistanceSquared)
		{
			return true;
		}
	}
	return false;
}

