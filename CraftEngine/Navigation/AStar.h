#pragma once

#include <Navigation/Node.h>

#include <functional>
#include <vector>

namespace Craft
{
	class CRAFT_API AStar
	{
	private:
		// 이동방향과 해당 방향의 이동 비용.
		struct Direction
		{
			int x = 0;
			int y = 0;
			float cost = 0.f;
		};

		
	public:
		using CanMove = std::function<bool(int, int)>;

		AStar();
		~AStar();

		// 경로 탐색 요청 함수.
		std::vector<Vector2I> FindPath(const Vector2I& startPosition, const Vector2I& goalPosition,
			int mapWidth, int mapHeight, const CanMove& canMove);

	private:
		// 이전탐색에 사용한 정보 및 노드를 정리하는 함수.
		void Clear();

		// 노드 생성 전문 함수.
		Node* CreateNode(const Vector2I& position, Node* parent = nullptr);

		// 부모 노드를 역추적해서 경로를 구성하는 함수.
		std::vector<Vector2I> ConstructPath(Node* destination);

		// 휴리스틱(추정비용) 계산 함수.
		float CalculateHeuristic(const Vector2I& current, const Vector2I& goal) const;

		// 이동하려는 좌표가 유효한지 확인.
		bool IsInRange(int tileX, int tileY, int mapWidth, int mapHeight);

		// 대각선 이동이 장애물 모서리를 통과하는지 확인.
		bool IsDiagonalBlocked(const Vector2I& current, const Direction& direction, const CanMove& canMove) const;

		// Open 목록 및 Closed 목록 검사 함수.,
		Node* FindOpenNode(int x, int y) const;
		bool IsInClosedList(int x, int y) const;

		// 목적지 인지 확인하는 함수.
		bool IsDestination(const Node* node) const;

	private:
		// 동적 할당된 모든 노드를 소유하는 목록.
		// 메모리 관리를 위해 사용
		std::vector<Node*> allocatedNodes;

		// 탐색할 노드 목록/ 탐색을 마친 노드 목록
		std::vector<Node*> openList;
		std::vector<Node*> closedList;

		// 시작노드 / 목표노드
		Node* startNode = nullptr;
		Node* goalNode = nullptr;
	};
}




