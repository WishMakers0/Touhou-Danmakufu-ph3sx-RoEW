#pragma once

#include "../pch.h"
#include "RenderObject.hpp"

namespace directx {
	//*******************************************************************
	//ObjMtlMesh
	//*******************************************************************
	class ObjMtlMesh;
	class ObjMtlMeshData : public DxMeshData {
		friend ObjMtlMesh;
	public:
		class Material;
		class Object;
		class RenderObject;

		struct NormalData {
			std::vector<uint16_t> listIndex_;
			D3DXVECTOR3 normal_;
			virtual ~NormalData() {}
		};
	protected:
		std::wstring path_;
		std::vector<RenderObject*> renderList_;
		std::vector<Material*> materialList_;

		void _ReadMaterial(gstd::Scanner& scanner);
		void _ReadObject(gstd::Scanner& scanner);
	public:
		ObjMtlMeshData();
		~ObjMtlMeshData();

		bool CreateFromFileReader(shared_ptr<gstd::FileReader> reader);
	};

	class ObjMtlMeshData::Material {
		friend ObjMtlMesh;
		friend ObjMtlMeshData;
		friend ObjMtlMeshData::RenderObject;
	protected:
		std::wstring name_;
		D3DMATERIAL9 mat_;
		shared_ptr<Texture> texture_;
		std::string pathTextureAlpha_;
		std::string pathTextureBump_;
	public:
		Material() : texture_(nullptr) { ZeroMemory(&mat_, sizeof(D3DMATERIAL9)); };
		virtual ~Material() {};
	};

	class ObjMtlMeshData::Object {
		friend ObjMtlMeshData;
	protected:
		struct Face {
			struct Vertex {
				size_t indexVertex_;
				D3DXVECTOR2 tcoord_;
			};
			int indexMaterial_ = -1;
			std::vector<Vertex> vertices_;
		};

		bool bVisible_;
		D3DXVECTOR3 color_;

		std::wstring name_;
		std::vector<D3DXVECTOR3> vertices_;
		std::vector<Face> faces_;
	public:
		Object() : bVisible_(false), color_(1, 1, 1) {}
		virtual ~Object() {}
	};

	class ObjMtlMeshData::RenderObject : public RenderObjectNX {
		friend ObjMtlMeshData;
	protected:
		static Material* nullMaterial_;

		Material* material_;
		D3DXVECTOR3 objectColor_;
	public:
		RenderObject() : material_(nullptr), objectColor_(1, 1, 1) {};
		virtual ~RenderObject() {};

		virtual void Render() {}
		void Render(D3DXMATRIX* matTransform);
	};

	class ObjMtlMesh : public DxMesh {
	public:
		ObjMtlMesh() {}
		virtual ~ObjMtlMesh() {}

		virtual bool CreateFromFileReader(shared_ptr<gstd::FileReader> reader);
		virtual bool CreateFromFileInLoadThread(const std::wstring& path);
		virtual std::wstring GetPath();

		virtual void Render();
		virtual void Render(const D3DXVECTOR2& angX, const D3DXVECTOR2& angY, const D3DXVECTOR2& angZ);
	};
}