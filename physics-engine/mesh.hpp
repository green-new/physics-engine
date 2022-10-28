#pragma once
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include "shader.hpp"
#include "texture.hpp"
#include "geometry.hpp"
#include "buffer_objects.hpp"

using namespace geolib;

template<class T>
class Mesh3D {
public:
	Mesh3D();
	~Mesh3D();

	GLsizei vertexCount();
	void setShader(Shader& shaderObject);
	virtual void draw();
private:
	Shader& mShader;
	vbo* mPosition;
	vbo* mNormals;
	vao* mVertexArray;
};

template<class T>
class TextureMesh3D : public Mesh3D<T> {
public:
	TextureMesh3D(texture_t TextureID);
	~TextureMesh3D();
	void draw() override;
private:
	texture_t mTextureID;
};

template<class T>
class ColorMesh3D : public Mesh3D<T> {
public:
	ColorMesh3D(color_t Color);
	~ColorMesh3D();
	void draw() override;
private:
	color_t mColor;
};

namespace Shapes {
	class IShape {
	public:
		const Geometry3D& mShapeData;
		GLData mData;
		IShape(std::string geometryName) : mShapeData(resourceManager->getGeometry(geometryName)) {
			GLDataAdapter adapter(mShapeData);
			mData = adapter.requestData();
		}
	};

	class Sphere : public IShape {
		Sphere() : IShape("uvsphere") {
		}
	};

	class Cube : public IShape {
		Cube() : IShape("cube") {
		}
	};

	class Tetrahedron : public IShape {
		Tetrahedron() : IShape("tetrahedron") {
		}
	};

	class Octahedron : public IShape {
		Octahedron() : IShape("octahedron") {
		}
	};

	class Teapot : public IShape {
		Teapot() : IShape("teapot") {
		}
	};

	class Isocahedron : public IShape {
		Isocahedron() : IShape("isocahedron") {
		}
	};

	class CustomShape : public IShape {
		CustomShape(std::string geometryName) : IShape(geometryName) {
		}
	};
}


class ShapeManager {
public:
	template<class T>
	void registerShape() {
		const char* tn = typeid(T).name();

		typenameShapeMap.insert({tn, std::make_shared<T>()});
	}

	template<class T>
	std::shared_ptr<T> getShape() {
		const char* tn = typeid(T).name();

		assert(typenameShapeMap.find(tn) != typenameShapeMap.end() && "[Shape Registry] Shape not registered yet");

		return (T)typenameShapeMap.at();
	}
private:
	std::unordered_map<const char*, std::shared_ptr<Shapes::IShape>> typenameShapeMap;
};