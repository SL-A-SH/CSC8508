#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "CollisionDetection.h"
#include "Quaternion.h"

#include "Constraint.h"

#include "Debug.h"
#include "Window.h"
#include <functional>
using namespace NCL;
using namespace CSC8503;

PhysicsSystem::PhysicsSystem(GameWorld& g) : gameWorld(g) {
	applyGravity = false;
	useBroadPhase = true; // 
	dTOffset = 0.0f;
	globalDamping = 0.995f;
	SetGravity(Vector3(0.0f, -9.8f, 0.0f));
}

PhysicsSystem::~PhysicsSystem() {
}

void PhysicsSystem::SetGravity(const Vector3& g) {
	gravity = g;
}

/*

If the 'game' is ever reset, the PhysicsSystem must be
'cleared' to remove any old collisions that might still
be hanging around in the collision list. If your engine
is expanded to allow objects to be removed from the world,
you'll need to iterate through this collisions list to remove
any collisions they are in.

*/
void PhysicsSystem::Clear() {
	allCollisions.clear();
}

/*

This is the core of the physics engine update

*/

bool useSimpleContainer = false;

int constraintIterationCount = 10;

//This is the fixed timestep we'd LIKE to have
const int   idealHZ = 120;
const float idealDT = 1.0f / idealHZ;

/*
This is the fixed update we actually have...
If physics takes too long it starts to kill the framerate, it'll drop the
iteration count down until the FPS stabilises, even if that ends up
being at a low rate.
*/
int realHZ = idealHZ;
float realDT = idealDT;

void PhysicsSystem::Update(float dt) {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::B)) {
		useBroadPhase = !useBroadPhase;
		std::cout << "Setting broadphase to " << useBroadPhase << std::endl;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::N)) {
		useSimpleContainer = !useSimpleContainer;
		std::cout << "Setting broad container to " << useSimpleContainer << std::endl;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::I)) {
		constraintIterationCount--;
		std::cout << "Setting constraint iterations to " << constraintIterationCount << std::endl;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::O)) {
		constraintIterationCount++;
		std::cout << "Setting constraint iterations to " << constraintIterationCount << std::endl;
	}

	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!

	GameTimer t;
	t.GetTimeDeltaSeconds();

	if (useBroadPhase) {
		UpdateObjectAABBs();
	}
	int iteratorCount = 0;
	while (dTOffset > realDT) {
		IntegrateAccel(realDT); //Update accelerations from external forces
		if (useBroadPhase) {
			BroadPhase();
			NarrowPhase();
		}
		else {
			BasicCollisionDetection();
		}

		//This is our simple iterative solver - 
		//we just run things multiple times, slowly moving things forward
		//and then rechecking that the constraints have been met		
		float constraintDt = realDT / (float)constraintIterationCount;
		for (int i = 0; i < constraintIterationCount; ++i) {
			UpdateConstraints(constraintDt);
		}
		IntegrateVelocity(realDT); //update positions from new velocity changes

		dTOffset -= realDT;
		iteratorCount++;
	}

	ClearForces();	//Once we've finished with the forces, reset them to zero

	UpdateCollisionList(); //Remove any old collisions

	t.Tick();
	float updateTime = t.GetTimeDeltaSeconds();

	//Uh oh, physics is taking too long...
	if (updateTime > realDT) {
		realHZ /= 2;
		realDT *= 2;
		//std::cout << "Dropping iteration count due to long physics time...(now " << realHZ << ")\n";
	}
	else if (dt * 2 < realDT) { //we have plenty of room to increase iteration count!
		int temp = realHZ;
		realHZ *= 2;
		realDT /= 2;

		if (realHZ > idealHZ) {
			realHZ = idealHZ;
			realDT = idealDT;
		}
		if (temp != realHZ) {

			//		std::cout << "Raising iteration count due to short physics time...(now " << realHZ << ")\n";
		}
	}
}

/*
Later on we're going to need to keep track of collisions
across multiple frames, so we store them in a set.

The first time they are added, we tell the objects they are colliding.
The frame they are to be removed, we tell them they're no longer colliding.

From this simple mechanism, we we build up gameplay interactions inside the
OnCollisionBegin / OnCollisionEnd functions (removing health when hit by a
rocket launcher, gaining a point when the player hits the gold coin, and so on).
*/
void PhysicsSystem::UpdateCollisionList() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end(); ) {
		if ((*i).framesLeft == numCollisionFrames) {
			i->a->OnCollisionBegin(i->b);
			i->b->OnCollisionBegin(i->a);
		}

		CollisionDetection::CollisionInfo& in = const_cast<CollisionDetection::CollisionInfo&>(*i);
		in.framesLeft--;

		if ((*i).framesLeft < 0) {
			i->a->OnCollisionEnd(i->b);
			i->b->OnCollisionEnd(i->a);
			i = allCollisions.erase(i);
		}
		else {
			++i;
		}
	}
}

void PhysicsSystem::UpdateObjectAABBs() {
	std::vector<GameObject*> ::const_iterator first;
	std::vector<GameObject*> ::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i)
	{
		(*i)->UpdateBroadphaseAABB();
	}
}

/*

This is how we'll be doing collision detection in tutorial 4.
We step thorugh every pair of objects once (the inner for loop offset
ensures this), and determine whether they collide, and if so, add them
to the collision set for later processing. The set will guarantee that
a particular pair will only be added once, so objects colliding for
multiple frames won't flood the set with duplicates.
*/
void PhysicsSystem::BasicCollisionDetection() {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i)
	{


		if ((*i)->GetPhysicsObject() == nullptr) {
			continue;
		}

		for (auto j = i + 1; j != last; ++j)
		{
			if ((*j)->GetPhysicsObject() == nullptr) {
				continue;
			}
			CollisionDetection::CollisionInfo collisionInfo;
			if (CollisionDetection::ObjectIntersection(*i, *j, collisionInfo)) {
				ImpulseResolveCollision(*collisionInfo.a, *collisionInfo.b, collisionInfo.point);
				collisionInfo.framesLeft = numCollisionFrames;
				allCollisions.insert(collisionInfo);
				std::cout << "Collision Detected!\n";
			}

		}
	}
}

/*

In tutorial 5, we start determining the correct response to a collision,
so that objects separate back out.

*/
void PhysicsSystem::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const {
	PhysicsObject* objectA = a.GetPhysicsObject();
	PhysicsObject* objectB = b.GetPhysicsObject();

	Transform& transformA = a.GetTransform();
	Transform& transformB = b.GetTransform();

	float totalMass = objectA->GetInverseMass() + objectB->GetInverseMass();
	if (totalMass == 0) {
		return;
	}

	transformA.SetPosition(transformA.GetPosition() - (p.normal * p.penetration * (objectA->GetInverseMass() / totalMass)));
	transformB.SetPosition(transformB.GetPosition() + (p.normal * p.penetration * (objectB->GetInverseMass() / totalMass)));

	Vector3 relativeA = p.localA;
	Vector3 relativeB = p.localB;

	Vector3 angluarVelocityA = Vector::Cross(objectA->GetAngularVelocity(), relativeA);
	Vector3 angluarVelocityB = Vector::Cross(objectB->GetAngularVelocity(), relativeB);

	Vector3 fullVelocityA = objectA->GetLinearVelocity() + angluarVelocityA;
	Vector3 fullVelocityB = objectB->GetLinearVelocity() + angluarVelocityB;

	Vector3 contactVelocity = fullVelocityB - fullVelocityA;

	float impulseForce = Vector::Dot(contactVelocity, p.normal);

	Vector3 intertiaA = Vector::Cross(objectA->GetInertiaTensor() * Vector::Cross(relativeA, p.normal), relativeA);
	Vector3 intertiaB = Vector::Cross(objectB->GetInertiaTensor() * Vector::Cross(relativeB, p.normal), relativeB);

	float angularEffect = Vector::Dot(intertiaA + intertiaB, p.normal);

	float cRestitution = 0.66f; /// TODO -> NEEED TO CHANGE THIS TO BE A MATERIAL PROPERTY LATER INTO ENUM // RESEARCH MORE ON THIS.

	float j = (-(1.0f + cRestitution) * impulseForce) / (totalMass + angularEffect);

	Vector3 fullImpulse = p.normal * j;

	objectA->ApplyLinearImpulse(-fullImpulse);
	objectB->ApplyLinearImpulse(fullImpulse);


	objectA->ApplyAngularImpulse(Vector::Cross(relativeA, -fullImpulse));
	objectB->ApplyAngularImpulse(Vector::Cross(relativeB, fullImpulse));


}

/*

Later, we replace the BasicCollisionDetection method with a broadphase
and a narrowphase collision detection method. In the broad phase, we
split the world up using an acceleration structure, so that we can only
compare the collisions that we absolutely need to.

*/
void PhysicsSystem::BroadPhase() {
	broadphaseCollisions.clear();
	QuadTree<GameObject*> tree(Vector2(1024, 1024), 7, 6);

	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i)
	{
		Vector3 halfSizes;
		if (!(*i)->GetBroadphaseAABB(halfSizes)) continue;
		Vector3 pos = (*i)->GetTransform().GetPosition();
		tree.Insert(*i, pos, halfSizes);
	}

	tree.OperateOnContents([&](std::list<QuadTreeEntry	<GameObject*>>& data) {
		CollisionDetection::CollisionInfo info;
		for (auto i = data.begin(); i != data.end(); ++i)
		{
			for (auto j = std::next(i); j != data.end(); ++j)
			{
				info.a = std::min((*i).object, (*j).object);
				info.b = std::max((*i).object, (*j).object);
				broadphaseCollisions.insert(info);
			}
		}
		});
}

/*

The broadphase will now only give us likely collisions, so we can now go through them,
and work out if they are truly colliding, and if so, add them into the main collision list
*/
void PhysicsSystem::NarrowPhase() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = broadphaseCollisions.begin(); i != broadphaseCollisions.end(); ++i)
	{
		CollisionDetection::CollisionInfo collisionInfo = *i;
		if (CollisionDetection::ObjectIntersection(collisionInfo.a, collisionInfo.b, collisionInfo))
		{
			collisionInfo.framesLeft = numCollisionFrames;
			ImpulseResolveCollision(*collisionInfo.a, *collisionInfo.b, collisionInfo.point);
			allCollisions.insert(collisionInfo);
			Debug::LogCollision(collisionInfo.a->GetName(), collisionInfo.b->GetName(), collisionInfo.point.localA, collisionInfo.point.normal, collisionInfo.point.penetration);

		}
	}

}

/*
Integration of acceleration and velocity is split up, so that we can
move objects multiple times during the course of a PhysicsUpdate,
without worrying about repeated forces accumulating etc.

This function will update both linear and angular acceleration,
based on any forces that have been accumulated in the objects during
the course of the previous game frame.
*/
void PhysicsSystem::IntegrateAccel(float dt) {
	std::vector<GameObject*> ::const_iterator first;
	std::vector<GameObject*> ::const_iterator last;

	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i)
	{
		PhysicsObject* physicsObject = (*i)->GetPhysicsObject();
		if (physicsObject == nullptr) {
			continue;
		}
		float inverseMass = physicsObject->GetInverseMass();
		Vector3 linearVelocity = physicsObject->GetLinearVelocity();
		Vector3 force = physicsObject->GetForce();
		Vector3 acceleration = force * inverseMass;

		if (applyGravity && inverseMass > 0) {
			acceleration += gravity;
		}
		linearVelocity += acceleration * dt;
		physicsObject->SetLinearVelocity(linearVelocity);

		Vector3 torque = physicsObject->GetTorque();
		Vector3 angularVelocity = physicsObject->GetAngularVelocity();

		physicsObject->UpdateInertiaTensor();

		Vector3 angularAcceleration = physicsObject->GetInertiaTensor() * torque;

		angularVelocity += angularAcceleration * dt;
		physicsObject->SetAngularVelocity(angularVelocity);
	}
}

/*
This function integrates linear and angular velocity into
position and orientation. It may be called multiple times
throughout a physics update, to slowly move the objects through
the world, looking for collisions.
*/
void PhysicsSystem::IntegrateVelocity(float dt) {
	std::vector<GameObject*> ::const_iterator first;
	std::vector<GameObject*> ::const_iterator last;

	gameWorld.GetObjectIterators(first, last);

	float frameLinearDamping = 1.0f - (0.4f * dt);

	for (auto i = first; i != last; ++i)
	{
		PhysicsObject* physicsObject = (*i)->GetPhysicsObject();
		if (physicsObject == nullptr) {
			continue;
		}

		Transform& transform = (*i)->GetTransform();
		Vector3 position = transform.GetPosition();
		Vector3 linearVelocity = physicsObject->GetLinearVelocity();
		position += linearVelocity * dt;
		transform.SetPosition(position);

		linearVelocity = linearVelocity * frameLinearDamping;
		physicsObject->SetLinearVelocity(linearVelocity);

		Quaternion orientation = transform.GetOrientation();
		Vector3 angularVelocity = physicsObject->GetAngularVelocity();

		orientation = orientation + (Quaternion(angularVelocity * dt * 0.5f, 0));
		orientation.Normalise();
		transform.SetOrientation(orientation);

		float frameAngularDamping = 1.0f - (0.4f * dt);
		angularVelocity = angularVelocity * frameAngularDamping;
		physicsObject->SetAngularVelocity(angularVelocity);
	}
}

/*
Once we're finished with a physics update, we have to
clear out any accumulated forces, ready to receive new
ones in the next 'game' frame.
*/
void PhysicsSystem::ClearForces() {
	gameWorld.OperateOnContents(
		[](GameObject* o) {
			o->GetPhysicsObject()->ClearForces();
		}
	);
}


/*

As part of the final physics tutorials, we add in the ability
to constrain objects based on some extra calculation, allowing
us to model springs and ropes etc.

*/
void PhysicsSystem::UpdateConstraints(float dt) {
	std::vector<Constraint*>::const_iterator first;
	std::vector<Constraint*>::const_iterator last;
	gameWorld.GetConstraintIterators(first, last);

	for (auto i = first; i != last; ++i) {
		(*i)->UpdateConstraint(dt);
	}
}