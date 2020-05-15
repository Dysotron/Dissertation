#pragma once
#include "GameObject.h"

namespace NCL 
{
	namespace CSC8503 
	{

		class BonusItem : public GameObject
		{
		public:
			BonusItem(string name, bool isStatic, int score) : GameObject(name, isStatic)
			{
				scoreValue = score;
			}
			~BonusItem() {};

			int GetScoreValue()
			{
				return scoreValue;
			}

			bool GetCollected()
			{
				return collected;
			}

			void SetCollected(bool held)
			{
				this->collected = held;
			}

		protected:
			bool collected = false;
		};

	}
}