#include <Actor/Interaction/ExitZone.h>

#include <Input/Input.h>
#include <Resource/TextImageLoader.h>

ExitZone::ExitZone(const Craft::Vector2F& position)
	: super({}, position)
{
	ChangeImage(Craft::TextImageLoader::LoadAnimation(exitzoneFilename)[0]);
}

void ExitZone::Tick(float deltaTime)
{
	super::Tick(deltaTime);

    if (!IsPlayerInside())
    {
        escapeHoldTime = 0.f;
        return;
    }

    if (Craft::Input::Get().GetKey('E'))
    {
        escapeHoldTime += deltaTime;

        if (escapeHoldTime >= requiredHoldTime)
        {
            escapeRequested = true;
        }
    }
    else
    {
        escapeHoldTime = 0.f;
    }
}

bool ExitZone::IsPlayerInside()
{
    auto player = playerPtr.lock();

    if (!player)
    {
        return false;
    }

    const Craft::Vector2F difference = player->GetPosition() - position;

    const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);

    const float adjustedDetectRadius = interactionRadius * interactionRadius;

    if (adjustedDistanceSquared <= adjustedDetectRadius)
    {
        return true;
    }

    return false;
}