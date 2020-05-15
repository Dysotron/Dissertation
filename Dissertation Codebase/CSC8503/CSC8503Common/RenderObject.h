#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/TextureBase.h"
#include "../../Common/ShaderBase.h"
#include "../../Common/Vector4.h"

namespace NCL {
	using namespace NCL::Rendering;

	class MeshGeometry;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader);
			~RenderObject();

			void SetTexture(TextureBase* t, int index) {
				texture[index] = t;
			}

			TextureBase* GetTexture(int index) const {
				return texture[index];
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}
			
			ShaderBase*		GetShadowShader() const {
				return shadowShader;
			}

			void SetShadowShader(ShaderBase* shader)
			{
				shadowShader = shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

		protected:
			MeshGeometry*	mesh;
			TextureBase*	texture[10];
			ShaderBase*		shader;
			ShaderBase*		shadowShader;
			Transform*		transform;
			Vector4			colour;
		};
	}
}
