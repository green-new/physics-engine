#pragma once
#include <memory>
#include <unordered_map>
#include "systems.hpp"

class SystemManager {
public:
	template<class T>
	std::shared_ptr<T> registerSystem() {
		const char* tn = typeid(T).name();

		assert(mSystems.find(tn) == mSystems.end() && "[ECS] Error registering system: already exists");

		auto system = std::make_shared<T>();
		mSystems.insert({ tn, system });
		return system;
	}
	template<class T>
	void setSignature(Signature signature) {
		const char* tn = typeid(T).name();

		assert(mSystems.find(tn) != mSystems.end() && "[ECS] Error configuring system signature: system does not exist");

		mSignatures.insert({ tn, signature });
	}
	void onEntityDestroyed(Entity entity);
	void onEntitySignatureChange(Entity entity, Signature signature);
private:
	// Our list of systems.
	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};

	// Our list of signatures.
	std::unordered_map<const char*, Signature> mSignatures{};
};