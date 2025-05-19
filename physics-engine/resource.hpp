#pragma once
#include <unordered_map>
#include <memory>
#include <glad/glad.h>

#include "geometry.hpp"
#include "types.hpp"
#include "shader.hpp"

namespace Resources {
	template<class T>
	class Resource {
	public:
		virtual void init() = 0;
	public:
		T& get(const std::string& uri) const {
			if (_map.contains(uri))
				return *_map.at(uri).get();
			else
				throw std::invalid_argument("Registry: invalid argument for key " + uri + " in registry map");
		}
		std::unordered_map<std::string, std::shared_ptr<T>>& get_all() {
			return _map;
		}
		void add(const std::string uri, T object) {
			_map.insert({ uri, std::shared_ptr<T>(new T(object)) });
		}
	protected:
		Resource() {
			_map = std::unordered_map<std::string, std::shared_ptr<T>>();
			_map.clear();
		}

		~Resource() {
			_map.clear();
		}
		std::unordered_map<std::string, std::shared_ptr<T>> _map;
	};

	class GeometryResources : public Resource<Geometry::Geometry3D> {
	public:
		GeometryResources() = default;
		~GeometryResources() = default;
	public:
		virtual void init();
	};

	class ShaderResources : public Resource<Shader> {
	public:
		ShaderResources() = default;
		~ShaderResources() = default;
	public:
		virtual void init();
	};

	class TextureResources : public Resource<texture_t> {
	public:
		TextureResources() = default;
		~TextureResources();
	public:
		virtual void init();
	};

	// This is the one point of access to all resources for this application.
	// glfw and opengl must be defined before initalizing this class due to texture loading.
	// Perhaps in the future we split this into two, GameRegistry and GLResources.
	// GameRegistry would handle things like geometry data, and maybe sounds for example in the future and run on its own thread.
	// GLResources would handle all GL objects (textures, buffer objects, wayyy more perhaps) and run in a GL thread/context.
	// Also, optimally, this class should be static (it could be singleton, but singleton is way overrused and incorrectly!)
	// However, static isn't thread safe without mutexes.
	class ResourceManager final {
	public:
		ResourceManager()
			: Textures(std::make_unique<TextureResources>()),
			Geometries(std::make_unique<GeometryResources>()),
			Shaders(std::make_unique<ShaderResources>()) {}
	public:
		void init();

		texture_t& getTexture(std::string name);
		Geometry::Geometry3D& getGeometry(std::string name);
		Shader& getShader(std::string name);
		~ResourceManager() = default;
	private:
		std::unique_ptr<TextureResources> Textures;
		std::unique_ptr<GeometryResources> Geometries;
		std::unique_ptr<ShaderResources> Shaders;
	};
};