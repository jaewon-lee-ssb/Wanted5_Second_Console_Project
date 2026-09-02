#include <Engine/Engine.h>
#include <Level/GameLevel.h>

#include <Utility/Random.h>

using namespace Craft;

int main()
{
	Engine engine;
	Utility::SetRandomSeed();
	engine.AddNewLevel<GameLevel>();
	engine.Run();
}