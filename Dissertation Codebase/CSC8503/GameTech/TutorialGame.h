#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"


namespace NCL
{
	namespace CSC8503
	{
		class TutorialGame
		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void MoveAsteroid();

			void InitWorld();

			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddPlanetToWorld();

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			GameObject* asteroid = nullptr;
			GameObject* planet = nullptr;

			OGLMesh* sphereMesh = nullptr;
			OGLMesh* planetMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;
			OGLShader* planetShader = nullptr;
			OGLShader* planetShadowShader = nullptr;
			OGLShader* basicShadowShader = nullptr;

			OGLTexture* grassTexture = nullptr;
			OGLTexture* snowTexture = nullptr;
			OGLTexture* waterTexture = nullptr;
			OGLTexture* sandTexture = nullptr;
			OGLTexture* rockTexture = nullptr;
			OGLTexture* altRockTexture = nullptr;

			float timePassed;
			bool statStart = false;
			bool asteroidFired = false;

			float asteroidSize = 5.0f;
		};
	}
}

