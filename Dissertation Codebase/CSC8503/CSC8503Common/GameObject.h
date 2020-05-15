#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

#include "NavigationGrid.h"

#include <vector>

//#include "Debug.h"

using std::vector;

namespace NCL {
	namespace CSC8503 {
		class NetworkObject;

		class GameObject	{
		public:
			GameObject(string name = "", bool isStatic = true);
			virtual ~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}
			
			bool IsStatic() const {
				return isStatic;
			}

			const Transform& GetConstTransform() const {
				return transform;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			NetworkObject* GetNetworkObject() const {
				return networkObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			void SetSpeed(int speed)
			{
				this->speed = speed;
			}

			const int GetSpeed()
			{
				return speed;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			virtual GameObject* GetItem()
			{
				//return heldItem;
				return nullptr;
			}

			virtual void SetItem(GameObject* item)
			{
				//heldItem = item;
			}

			virtual int GetScoreValue()
			{
				return 0;
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();

			virtual bool GetCollected()
			{
				return false;
			}

			virtual void SetCollected(bool held)
			{
				
			}

			void MoveTowardsNode();
			void GetNextNode();
			void GenerateNodes(Vector3 endPos);

			void MoveTowardsPos(Vector3 pos);

			void InitGrid() {
				grid = new NavigationGrid("TestGrid2.txt");
			}

			vector<Vector3> GetPathNodes()
			{
				return pathNodes;
			}

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;
			NetworkObject*		networkObject;

			bool	isActive;
			bool isStatic;
			string	name;

			int speed = 20;
			int scoreValue;

			Vector3 broadphaseAABB;
			Vector3 initialSpawn;

			vector<Vector3> pathNodes;
			Vector3 endPos;
			Vector3 nextPos;
			NavigationGrid* grid = nullptr;
			NavigationPath outPath;
			int pathIndex = 0;
		};
	}
}

