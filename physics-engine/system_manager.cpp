#include "system_manager.hpp"

void SystemManager::onEntityDestroyed(Entity entity) {
	for (auto const& pair : mSystems) {
		auto const& system = pair.second;
		system->mEntities.erase(entity);
	}
}
void SystemManager::onEntitySignatureChange(Entity entity, Signature signature) {
	for (auto const& pair : mSystems) {
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& systemSignature = mSignatures[type];

		if ((signature & systemSignature) == systemSignature) {
			system->mEntities.insert(entity);
		}
		else {
			system->mEntities.erase(entity);
		}
	}
}