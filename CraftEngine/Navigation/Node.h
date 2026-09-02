#pragma once

#include <Core/Core.h>

#include <Math/Vector2.h>

namespace Craft
{
	class CRAFT_API Node
	{
	public:
		Node(const Vector2I& position, Node* parent = nullptr)
			: position(position), parent(parent)
		{

		}

	public:
		// 노드의 위치 정보.
		Vector2I position;

		// 노드의 비용정보.
		float gCost = 0.f;
		float hCost = 0.f;
		float fCost = 0.f;

		// 최종 경로를 역추적할 때 사용할 부모 노드.
		Node* parent = nullptr;

	};
}