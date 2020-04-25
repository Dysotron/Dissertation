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

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	Debug::SetRenderer(renderer);

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

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("doge.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
	planetShader = new OGLShader("tessVert.glsl", "displaceFrag.glsl", "", "sphereTCS.glsl", "sphereTES.glsl");

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
	if (!inSelectionMode)
	{
		world->GetMainCamera()->UpdateCamera(dt);
	}
	if (lockedObject != nullptr)
	{
		LockedCameraMovement();
	}

	UpdateKeys();

	if (useGravity)
	{
		Debug::Print("(G)ravity on", Vector2(10, 40));
	}
	else
	{
		Debug::Print("(G)ravity off", Vector2(10, 40));
	}

	SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

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

	if (lockedObject)
	{
		LockedObjectMovement();
	}
	else
	{
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement()
{
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
	{
		selectionObject->GetPhysicsObject()->AddForce(-rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
	{
		selectionObject->GetPhysicsObject()->AddForce(rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP))
	{
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))
	{
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}
}

void  TutorialGame::LockedCameraMovement()
{
	if (lockedObject != nullptr)
	{
		Vector3 objPos = lockedObject->GetTransform().GetWorldPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}
}


void TutorialGame::DebugObjectMovement()
{
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject)
	{
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
		{
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
		{
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7))
		{
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8))
		{
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
		{
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP))
		{
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))
		{
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5))
		{
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool TutorialGame::SelectObject()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q))
	{
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode)
		{
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else
		{
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode)
	{
		renderer->DrawString("Press Q to change to camera mode!", Vector2(10, 0));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT))
		{
			if (selectionObject)
			{	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true))
			{
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else
			{
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L))
		{
			if (selectionObject)
			{
				if (lockedObject == selectionObject)
				{
					lockedObject = nullptr;
				}
				else
				{
					lockedObject = selectionObject;
				}
			}
		}
	}
	else
	{
		renderer->DrawString("Press Q to change to select mode!", Vector2(10, 0));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject()
{

}

void TutorialGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld()
{
	world->ClearAndErase();
	physics->Clear();

	AddSphereToWorld(Vector3(100, 0, 0), 10);
	AddPlanetToWorld();
}

//From here on it's functions to add in objects to the world!



GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass)
{
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddPlanetToWorld()
{
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(10,10,10);
	SphereVolume* volume = new SphereVolume(10);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(Vector3(0,0,0));

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), planetMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(0);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

OGLMesh* TutorialGame::CreatePlanetMesh()
{
	float length = 100;
	float elevation = sqrt((length * length) - ((length / 2) * (length / 2)));
	Vector3 octaVert[6];


	//vertices needed to construct octahedron
	octaVert[0] = Vector3(-length / 2, 0, 0);
	octaVert[1] = Vector3(length / 2, 0, 0);
	octaVert[2] = Vector3(-length / 2, 0, length);
	octaVert[3] = Vector3(length / 2, 0, length);
	octaVert[4] = Vector3(0, elevation, length / 2);
	octaVert[5] = Vector3(0, -elevation, length / 2);

	Vector3 middle = (octaVert[4] + octaVert[5]) / 2;

	std::vector < Vector3 > verts;

	//back top
	verts.emplace_back(octaVert[0]);
	verts.emplace_back(octaVert[4]);
	verts.emplace_back(octaVert[1]);

	//front top
	verts.emplace_back(octaVert[2]);
	verts.emplace_back(octaVert[3]);
	verts.emplace_back(octaVert[4]);

	//right top
	verts.emplace_back(octaVert[1]);
	verts.emplace_back(octaVert[4]);
	verts.emplace_back(octaVert[3]);

	//left top
	verts.emplace_back(octaVert[0]);
	verts.emplace_back(octaVert[2]);
	verts.emplace_back(octaVert[4]);

	//back bottom
	verts.emplace_back(octaVert[0]);
	verts.emplace_back(octaVert[1]);
	verts.emplace_back(octaVert[5]);

	//front bottom
	verts.emplace_back(octaVert[2]);
	verts.emplace_back(octaVert[5]);
	verts.emplace_back(octaVert[3]);

	//right bottom
	verts.emplace_back(octaVert[1]);
	verts.emplace_back(octaVert[3]);
	verts.emplace_back(octaVert[5]);

	//left bottom
	verts.emplace_back(octaVert[0]);
	verts.emplace_back(octaVert[5]);
	verts.emplace_back(octaVert[2]);

	std::vector < Vector3 > normals;

	for (int i = 0; i < verts.size(); i++) //replace this with calculations, create triangle around vertex, use to calculate normals, should give same values
	{
		Vector3& a = verts[i];
		Vector3 normal;

		normal = verts[i] - middle; //direction vector between middle and vertex
		normals.emplace_back(normal);
	}

	std::vector < Vector2 > texCoords;

	for (int i = 0; i < 8; i++)
	{
		texCoords.push_back(Vector2(0, 1));
		texCoords.push_back(Vector2(1, 1));
		texCoords.push_back(Vector2(0.5, 0));
	}

	std::vector < Vector4 > white;

	for (int i = 0; i < 24; i++)
	{
		white.push_back(Vector4(1, 1, 1, 1));
	}

	OGLMesh* top = new OGLMesh(); //top half
	top->SetVertexPositions(verts);
	top->SetVertexNormals(normals);
	top->SetVertexColours(white);
	top->SetVertexTextureCoords(texCoords);
	top->SetPrimitiveType(GeometryPrimitive::Triangles);
	top->UploadToGPU();

	return top;
}




