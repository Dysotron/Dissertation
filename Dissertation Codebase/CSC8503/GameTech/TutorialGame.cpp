#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()
{
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);

	world->GetMainCamera()->SetPosition(Vector3(0, 0, 70));
	world->GetMainCamera()->SetYaw(-30);

	renderer->SetAsteroidSize(asteroidSize);

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	Debug::SetRenderer(renderer);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	InitialiseAssets();
}

void TutorialGame::InitialiseAssets()
{
	auto loadFuncTris = [](const string& name, OGLMesh** into)
	{
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};
	
	auto loadFuncPatch = [](const string& name, OGLMesh** into)
	{
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Patches);
		(*into)->UploadToGPU();
	};

	loadFuncTris("sphere.msh", &sphereMesh);

	loadFuncPatch("planet.msh", &planetMesh);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
	basicShadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	grassTexture = (OGLTexture*)TextureLoader::LoadAPITexture("grass.jpg");
	snowTexture = (OGLTexture*)TextureLoader::LoadAPITexture("snow.jpg");
	waterTexture = (OGLTexture*)TextureLoader::LoadAPITexture("water.jpg");
	sandTexture = (OGLTexture*)TextureLoader::LoadAPITexture("sand.jpg");
	rockTexture = (OGLTexture*)TextureLoader::LoadAPITexture("rock.jpg");
	altRockTexture = (OGLTexture*)TextureLoader::LoadAPITexture("altRock.jpg");
	planetShader = new OGLShader("planetVert.glsl", "planetFrag.glsl", "", "planetTCS.glsl", "planetTES.glsl");
	planetShadowShader = new OGLShader("shadowPlanetVert.glsl", "shadowPlanetFrag.glsl", "", "shadowPlanetTCS.glsl", "shadowPlanetTES.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()
{
	delete sphereMesh;
	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt)
{
	world->GetMainCamera()->UpdateCamera(dt);
	UpdateKeys();

	if (statStart)
	{
		timePassed += dt;
		world->GetMainCamera()->SetPosition(Vector3(0, 0, 70));
		world->GetMainCamera()->SetYaw(-30);
	}

	if (timePassed > 20 && !asteroidFired)
	{
		MoveAsteroid();
		asteroidFired = true;
	}
	

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	CollisionDetection::CollisionInfo collision = physics->GetCollision();

	if (collision.point.penetration > 0) //real collision and not empty
	{
		//pass to renderer so can become uniform
		renderer->SetCollisionCartesian(collision.point.cartesian);
	}

	Debug::FlushRenderables();
	renderer->Render();
}

void TutorialGame::UpdateKeys()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
	{
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2))
	{
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G))
	{
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9))
	{
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10))
	{
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7))
	{
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8))
	{
		world->ShuffleObjects(false);
	}
	
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
	{
		statStart = true;
	}
}

void TutorialGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(5000.0f);
	world->GetMainCamera()->SetPitch(0.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 0, 0));
}

void TutorialGame::InitWorld()
{
	world->ClearAndErase();
	physics->Clear();

	asteroid = AddSphereToWorld(Vector3(-200, 67, 20), asteroidSize);
	//asteroid = AddSphereToWorld(Vector3(200, 67, 20), asteroidSize);
	//asteroid = AddSphereToWorld(Vector3(0, 100, 20), asteroidSize);

	planet = AddPlanetToWorld();
}

GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass)
{
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	RenderObject* rObj = new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader);
	rObj->SetShadowShader(basicShadowShader);

	sphere->SetRenderObject(rObj);
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddPlanetToWorld()
{
	GameObject* planet = new GameObject();

	float size = 20.0f;

	Vector3 sphereSize = Vector3(size, size, size);
	SphereVolume* volume = new SphereVolume(size);
	planet->SetBoundingVolume((CollisionVolume*)volume);
	planet->GetTransform().SetWorldScale(sphereSize);
	planet->GetTransform().SetWorldPosition(Vector3(0,0,0));

	RenderObject* rObj = new RenderObject(&planet->GetTransform(), planetMesh, grassTexture, planetShader);
	rObj->SetTexture(snowTexture, 1);
	rObj->SetTexture(waterTexture, 2);
	rObj->SetTexture(sandTexture, 3);
	rObj->SetTexture(rockTexture, 4);
	rObj->SetTexture(altRockTexture, 5);
	rObj->SetShadowShader(planetShadowShader);
	planet->SetRenderObject(rObj);
	planet->SetPhysicsObject(new PhysicsObject(&planet->GetTransform(), planet->GetBoundingVolume()));

	planet->GetPhysicsObject()->SetInverseMass(0);
	planet->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(planet);

	return planet;
}

void TutorialGame::MoveAsteroid()
{
	Vector3 asteroidPos = asteroid->GetTransform().GetWorldPosition();
	Vector3 planetPos = planet->GetTransform().GetWorldPosition();

	Vector3 direction = planetPos - asteroidPos;
	direction.Normalise();

	asteroid->GetPhysicsObject()->AddForce(direction * 10000);
}


