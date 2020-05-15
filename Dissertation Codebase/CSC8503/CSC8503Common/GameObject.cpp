#include "GameObject.h"
#include "CollisionDetection.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName, bool isStatic)	{
	name			= objectName;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
	this->isStatic = isStatic;
	this->isStatic = true;
//	grid = NavigationGrid("TestGrid2.txt");
}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

//These would be better as a virtual 'ToAABB' type function, really...
void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetWorldOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}

void GameObject::MoveTowardsNode() //applies forces in the direction of the next node to be visited for pathfinding
{
	Vector3 currentPos = transform.GetWorldPosition();

	int speed = 10;

	//physicsObject->ClearForces();

	if (nextPos.x > currentPos.x) //if the next node has a higher x value than we do now, add force in the positive x direction
	{
		physicsObject->AddForce(Vector3(speed, 0, 0));
	}

	if (nextPos.x < currentPos.x) //if the next node has a smaller x value, apply a force in the negative x direction
	{
		physicsObject->AddForce(Vector3(-speed, 0, 0));
	}

	if (nextPos.z > currentPos.z) //same for z 
	{
		physicsObject->AddForce(Vector3(0, 0, speed));
	}

	if (nextPos.z < currentPos.z)
	{
		physicsObject->AddForce(Vector3(0, 0, -speed));
	}
}

void GameObject::GetNextNode()
{
	Vector3 currentPos = transform.GetWorldPosition();

	if (pathIndex > pathNodes.size() - 1)
	{
		//pathIndex = pathNodes.size() - 1;
		pathIndex = 0;
	}

	nextPos = pathNodes[pathIndex];

	Vector3 next = (currentPos - nextPos);
	next.y = 0.0f;
	float nextDist = next.Length();

	Vector3 end = (currentPos - endPos);
	end.y = 0.0f;
	float endDist = end.Length();

	if (endDist < 0.5f) //close enough to the node
	{
		//at the destination, so do nothing
		return;
	}
	
	else if (nextDist < 0.5f) //close enough to the node
	{

		if (pathIndex < pathNodes.size() - 1)
		{
			pathIndex++;
		}
	}
}

void GameObject::GenerateNodes(Vector3 pos)
{
	outPath.Clear();
	pathNodes.clear();
	bool found = grid->FindPath(transform.GetWorldPosition(), pos, outPath);

	Vector3 temp;

	while (outPath.PopWaypoint(temp))
	{
		pathNodes.push_back(temp);
	}

	for (int i = 0; i < pathNodes.size(); ++i)
	{
		//std::cout << pathNodes[i].x << "," << pathNodes[i].z << "\n";
	}

	//std::cout << "\n\n\n\n\n";

	endPos = pos;

	//pathIndex = 0;
}

void GameObject::MoveTowardsPos(Vector3 pos) //applies forces in the direction of the next node to be visited for pathfinding
{
	Vector3 currentPos = transform.GetWorldPosition();

	int speed = 5;

	if (pos.x > currentPos.x) //if the next node has a higher x value than we do now, add force in the positive x direction
	{
		physicsObject->AddForce(Vector3(speed, 0, 0));
	}

	if (pos.x < currentPos.x) //if the next node has a smaller x value, apply a force in the negative x direction
	{
		physicsObject->AddForce(Vector3(-speed, 0, 0));
	}

	if (pos.z > currentPos.z) //same for z 
	{
		physicsObject->AddForce(Vector3(0, 0, speed));
	}

	if (pos.z < currentPos.z)
	{
		physicsObject->AddForce(Vector3(0, 0, -speed));
	}
}