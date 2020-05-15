#pragma once

#include "GameObject.h"
#include "BonusItem.h"
#include "GameWorld.h"

namespace NCL
{
	namespace CSC8503
	{
		class Player : public GameObject
		{
		public:
			Player(string name,  bool isStatic, GameWorld* world) : GameObject(name, isStatic)
			{
				this->world = world;
			}
			~Player() {};

			GameObject* GetItem()
			{
				return heldItem;
			}

			void SetItem(GameObject* item)
			{
				heldItem = item;
			}

			void OnCollisionBegin(GameObject* otherObject) 
			{
				//std::cout << "OnCollisionBegin event occured!\n";

				Vector3 playerPos = this->GetTransform().GetWorldPosition();

				speed = 20; // reset speed in case goose no longer in water

				if (!otherObject->GetCollected())
				{
					if ((otherObject->GetName() == "apple" || otherObject->GetName() == "snitch") && heldItem == nullptr)
					{
						//otherObject->GetTransform().SetWorldPosition(playerPos);
						heldItem = otherObject;
						holding = true;
					}

					if ((otherObject->GetName() == "apple") && heldItem == otherObject)
					{
						playerPos.y += 1.5f;
						//otherObject->GetTransform().SetWorldPosition(playerPos);

						heldItem->GetPhysicsObject()->AddForce(Vector3(0, 100, 20));
						//heldItem = otherObject;
						//holding = true;
					}

					else if ((otherObject->GetName() == "snitch") && heldItem == otherObject)
					{
						playerPos.y += 1.0f;
						//otherObject->GetTransform().SetWorldPosition(playerPos);
						//heldItem = otherObject;
						//holding = true;
					}
				}
				

				if (otherObject->GetName() == "water")
				{
					speed = 40;
				}

				else if (otherObject->GetName() == "island" && heldItem)
				{
					Vector3 islandPos = otherObject->GetTransform().GetWorldPosition();
					islandPos.y += 10;
					heldItem->GetPhysicsObject()->SetInverseMass(0);
					heldItem->GetTransform().SetWorldPosition(islandPos);
					heldItem->SetCollected(true);
					
					if (heldItem->GetName() == "apple")
					{
						world->MinusAppleCount();
					}

					world->AddToScore(heldItem->GetScoreValue());

					heldItem = nullptr;
				}

				else if ((otherObject->GetName() == "character" || otherObject->GetName() == "keeper") && heldItem)
				{
					int randX = 20 + (std::rand() % (190 - 20 + 1));
					int randZ = 20 + (std::rand() % (190 - 20 + 1));
					Vector3 pos = Vector3(randX, 10, randZ);
					std::cout << pos.x << "," << pos.y << "," << pos.z << "\n";
					//pos.y += 10;
					heldItem->GetTransform().SetWorldPosition(pos);
					heldItem = nullptr;
				}
			}

			void OnCollisionEnd(GameObject* otherObject) 
			{
				//std::cout << "OnCollisionEnd event occured!\n";
			}

		protected:
			bool holding = false;
			GameWorld* world;

			GameObject* heldItem = nullptr; //pointer to item this object is holding, nullptr if no item
		};

	}
}