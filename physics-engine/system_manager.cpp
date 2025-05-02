#include "system_manager.hpp"
#include <iostream>

void SystemManager::onEntityDestroyed(Entity entity) {
	for (auto const& pair : mSystems) {
		auto const& system = pair.second;
		system->m_entities.erase(entity);
	}
}
void SystemManager::onEntitySignatureChange(Entity entity, Signature signature) {
	for (auto const& pair : mSystems) {
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& systemSignature = mSignatures[type];

		if ((signature & systemSignature) == systemSignature) {
			system->m_entities.insert(entity);
		} else {
			system->m_entities.erase(entity);
		}
	}
}