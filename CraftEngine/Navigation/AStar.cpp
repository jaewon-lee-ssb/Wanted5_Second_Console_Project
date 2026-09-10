#include <Navigation/AStar.h>

#include <Utility/Timer/PerformanceTimer.h>

#include <algorithm>
#include <cstdlib>
#include <sstream>

#define NOMINMAX
#include <Windows.h>

namespace
{
	struct AStarMetrics
	{
		unsigned long long createdNodes = 0;
		unsigned long long expandedNodes = 0;
		unsigned long long neighborChecks = 0;
		unsigned long long openPushes = 0;
		unsigned long long openPops = 0;
		unsigned long long openLookupChecks = 0;
		unsigned long long openLookupHits = 0;
		unsigned long long closedChecks = 0;
		unsigned long long costUpdates = 0;
		size_t peakOpenSize = 0;
		size_t pathNodeCount = 0;
		bool pathFound = false;

		std::string ToLogText() const
		{
			std::ostringstream output;
			output << "PathFound=" << (pathFound ? 1 : 0)
				<< " | PathNodes=" << pathNodeCount
				<< " | CreatedNodes=" << createdNodes
				<< " | ExpandedNodes=" << expandedNodes
				<< " | NeighborChecks=" << neighborChecks
				<< " | OpenPushes=" << openPushes
				<< " | OpenPops=" << openPops
				<< " | OpenLookupChecks=" << openLookupChecks
				<< " | OpenLookupHits=" << openLookupHits
				<< " | ClosedChecks=" << closedChecks
				<< " | CostUpdates=" << costUpdates
				<< " | PeakOpenSize=" << peakOpenSize;
			return output.str();
		}
	};
}

namespace Craft
{
	AStar::AStar()
	{

	}

	AStar::~AStar()
	{
		Clear();
	}

	// 최적화 필요
	std::vector<Vector2I> AStar::FindPath(const Vector2I& startPosition, const Vector2I& goalPosition, int mapWidth, int mapHeight, const CanMove& canMove)
	{
		if (!canMove)
		{
			return {};
		}

		AStarMetrics metrics;
		PerformanceTimer timer("AStar::FindPath");
		timer.SetDetailsProvider([&metrics]() { return metrics.ToLogText(); });

		// 이전에 탐색한 결과 초기화.
		Clear();

		if (!IsInRange(startPosition.x, startPosition.y, mapWidth, mapHeight) || !IsInRange(goalPosition.x, goalPosition.y, mapWidth, mapHeight))
		{
			return {};
		}

		if (!canMove(startPosition.x, startPosition.y) || !canMove(goalPosition.x, goalPosition.y))
		{
			return {};
		}

		// 탐색 시작
		// 시작 / 목표 노드 생성
		startNode = CreateNode(startPosition);
		goalNode = CreateNode(goalPosition);
		metrics.createdNodes += 2;

		// 시작 노드의 비용 계산 및 openList에 추가해 탐색 시작.
		startNode->gCost = 0.f;
		startNode->hCost = CalculateHeuristic(startPosition, goalPosition);
		startNode->fCost = startNode->gCost + startNode->hCost;

		openList.emplace_back(startNode);
		++metrics.openPushes;
		metrics.peakOpenSize = std::max(metrics.peakOpenSize, openList.size());

		// 편의를 위해 사전 비용 설정.
		const float diagonalCost = 1.41421f;
		const std::vector<Direction> directions =
		{
			{  0, -1, 1.f }, { 0,  1, 1.f },	// 상하
			{ -1,  0, 1.f }, { 1,  0, 1.f },	// 좌우
			{ -1, -1, diagonalCost},			// 좌상단
			{  1, -1, diagonalCost},			// 우상단
			{ -1,  1, diagonalCost},			// 좌하단
			{  1,  1, diagonalCost},			// 우하단
		};

		// openList가 빌 때까지 탐색 반복.
		while (!openList.empty())
		{
			// openList에서 fCost가 가장 작은 노드를 선택.
			// 이진힙(heap)을 사용하면 최적화 가능.
			Node* currentNode = openList[0];
			// 순차 도는 방법 최적화 가능.
			for (Node* node : openList)
			{
				// 더 작은 비용의 노드 검색.
				if (node->fCost < currentNode->fCost || (node->fCost == currentNode->fCost && node->hCost < currentNode->hCost))
				{
					currentNode = node;
				}
			}

			// 목표 노드인지 확인.
			if (IsDestination(currentNode))
			{
				// 이동 경로 제작 후 반환.
				std::vector<Vector2I> path = ConstructPath(currentNode);
				metrics.pathFound = true;
				metrics.pathNodeCount = path.size();
				return path;
			}

			// 현재 노드를 openList에서 제거.
			// 방문 처리를 위해.
			auto iterator = std::find(openList.begin(), openList.end(), currentNode);
			// 검색에 성공했는지 확인.
			if (iterator != openList.end())
			{
				// openList에서 제거.
				openList.erase(iterator);
			}
			++metrics.openPops;
			++metrics.expandedNodes;

			// 탐색을 마친 노드를 closedList에 추가.
			closedList.emplace_back(currentNode);

			// 현재 위치를 기준으로 주변 (8방향)의 이웃노드를 탐색.
			for (const Direction& direction : directions)
			{
				++metrics.neighborChecks;
				// 현재 노드를 기준으로 인접한 노드의 좌표 계산.
				// 새로운 좌표(위치) = 현재 위치 + 이동 방향
				int newX = currentNode->position.x + direction.x;
				int newY = currentNode->position.y + direction.y;

				// 예외처리
				if (!IsInRange(newX, newY, mapWidth, mapHeight))
				{
					continue;
				}

				++metrics.closedChecks;
				// 이미 방문한 곳이라면 건너뛰기
				if (IsInClosedList(newX, newY))
				{
					continue;
				}

				// 새로운 위치가 장애물인지 확인.
				if (!canMove(newX, newY))
				{
					continue;
				}

				// 대각선 이동 시 장애물을 통과하는지 확인.
				if (IsDiagonalBlocked(currentNode->position, direction, canMove))
				{
					continue;
				}

				

				// 현재 노드를 거쳐서 새로운 위치로 가는데 드는 비용 계산.
				float newGCost = currentNode->gCost + direction.cost;
				++metrics.openLookupChecks;
				// 이미 openList에 있는데 비용면에서 더 나은지 확인.
				Node* openNode = FindOpenNode(newX, newY);
				if (openNode)
				{
					++metrics.openLookupHits;
					// 비용을 비교.
					if (newGCost < openNode->gCost)
					{
						openNode->gCost = newGCost;
						openNode->fCost = openNode->gCost + openNode->hCost;
						openNode->parent = currentNode;
						++metrics.costUpdates;
					}

					continue;
				}
				
				// 이웃노드 생성 및 openList에 추가
				Node* neighborNode = CreateNode(Vector2I(newX, newY), currentNode);
				++metrics.createdNodes;

				// 새로운 노드의 비용 계산.
				neighborNode->gCost = newGCost;
				neighborNode->hCost = CalculateHeuristic(neighborNode->position, goalNode->position);
				neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;

				// 새로운 노드를 openList에 추가.
				openList.emplace_back(neighborNode);
				++metrics.openPushes;

				// 시각화를 위한 처리
				// 아직 여기서 해야하는가에 대해 모름

				// 그리드 그리기

				

			}
		}

		return {};
	}

	void AStar::Clear()
	{
		// 탐색 과정에서 생성했던 모든 노드 삭제 및 해제
		for (Node* node : allocatedNodes)
		{
			delete node;
			node = nullptr;
		}

		allocatedNodes.clear();
		openList.clear();
		closedList.clear();

		startNode = nullptr;
		goalNode = nullptr;
	}

	Node* AStar::CreateNode(const Vector2I& position, Node* parent)
	{
		// 노드를 생성하고 allocatedNodes에 추가
		Node* newNode = new Node(position, parent);
		allocatedNodes.emplace_back(newNode);

		// 생성한 노드 반환
		return newNode;
	}

	std::vector<Vector2I> AStar::ConstructPath(Node* destination)
	{
		// 목표 노드로부터 부모 노드를 따라 경로 역추적.
		std::vector<Vector2I> path;
		Node* current = destination;

		while (current)
		{
			// 현재 노드는 경로 배열에 추가
			path.emplace_back(current->position);

			// 부모 노드로 이동해서 경로를 역추적.
			current = current->parent;
		}

		// 루프가 종료되면 path에는 반대 방향의 경로 정보가 저장됨.
		// 따라서 다시 역방향으로 뒤집기가 필요.
		std::reverse(path.begin(), path.end());

		return path;
	}

	float AStar::CalculateHeuristic(const Vector2I& current, const Vector2I& goal) const
	{
		// 옥타일 비용 계산법.
		// 대각선 이동 허용시 주의사항
		// ->대각선 형태의 장애물을 뚫고가지 못하게 막아야함.

		// 현재 위치와 목표 위치 사이의 차이 계산
		// std::abs -> 절댓값 함수(absolute).
		int diffX = std::abs(current.x - goal.x);
		int diffY = std::abs(current.y - goal.y);

		// 대각선 거리와 남은 직선 거리 분리.
		int diagonalDistance = std::min(diffX, diffY);
		int straightDistance = std::max(diffX, diffY) - diagonalDistance;

		// 대각선 비용.
		const float diagonalCost = 1.41421f;
		const float straightCost = 1.f;

		return diagonalDistance * diagonalCost + straightDistance * straightCost;
	}

	bool AStar::IsInRange(int tileX, int tileY, int mapWidth, int mapHeight)
	{
		return tileX >= 0 && tileX < mapWidth && tileY >= 0 && tileY < mapHeight;
	}

	bool AStar::IsDiagonalBlocked(const Vector2I& current, const Direction& direction, const CanMove& canMove) const
	{
		// 가로 또는 세로 이동이면 대각선 검사가 필요없음.
		if (direction.x == 0 || direction.y == 0)
		{
			return false;
		}

		// 대각선 옆의 가로 칸 검사.
		const bool canMoveHorizontal = canMove(current.x + direction.x, current.y);

		// 대각선 옆의 세로칸 검사.
		const bool canMoveVertical = canMove(current.x, current.y + direction.y);

		// 둘 중 하나라도 막혀있으면 대각선 이동 금지.
		return !canMoveHorizontal || !canMoveVertical;
	}

	Node* AStar::FindOpenNode(int x, int y) const
	{
		// 같은 좌표의 노드를 OpenList에서 찾기.
		// 공간을 더쓰면 최적화 가능
		for (Node* node : openList)
		{
			// 죄표 비교
			if (node->position == Vector2I(x, y))
			{
				return node;
			}
		}
		// 없으면
		return nullptr;
	}

	bool AStar::IsInClosedList(int x, int y) const
	{
		// 같은 좌표가 ClosedList에 있는지 확인.
		for (Node* node : closedList)
		{
			// 좌표 비교
			if (node->position == Vector2I(x, y))
			{
				return true;
			}
		}
		return false;
	}
	bool AStar::IsDestination(const Node* node) const
	{
		// 두 노드 모두 null이 아니고, 두 노드의 위치가 같은지 비교.
		
		return node && goalNode && node->position == goalNode->position;
	}
}