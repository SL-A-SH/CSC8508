#include "Player.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

#include "PrisonEscape/Prefabs/Player/PlayerTwo.h"
#include "PrisonEscape/Core/GameBase.h"
#include "PrisonEscape/Core/GameConfigManager.h"

Player::Player(GameWorld* world, const std::string& name) : GameObject(), controller(*Window::GetKeyboard(), *Window::GetMouse())
{
	playerObject = nullptr;
	useGravity = true;
	cameraAttached = true;

	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");
	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	GameObject::SetName(name);
}

Player::~Player()
{
	delete(playerObject);
}

void Player::UpdateGame(float dt) 
{
	UpdatePlayerMovement(dt);
}

void Player::UpdatePlayerMovement(float dt)
{

}