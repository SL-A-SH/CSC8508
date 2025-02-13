#include "SampleLevel.h"

SampleLevel::SampleLevel()
{
	
}

SampleLevel::~SampleLevel()
{

}

void SampleLevel::Init()
{
	Level::Init();
}

void SampleLevel::Update(float dt)
{
	Level::Update(dt);
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().UpdateCamera(dt);
}
