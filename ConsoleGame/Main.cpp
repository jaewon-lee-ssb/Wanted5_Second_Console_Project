#include <Engine/Engine.h>
#include <Level/StartLevel.h>

#include <Utility/Random.h>

using namespace Craft;

int main()
{
	Engine engine;
	Utility::SetRandomSeed();
	engine.AddNewLevel<StartLevel>();
	engine.Run();
}
