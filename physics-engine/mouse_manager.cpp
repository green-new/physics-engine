#include "mouse_manager.hpp"

using namespace Input;

MouseManager::MouseManager() : mouseCallbacks(), mouseCallbackToIndex(), usedCallbacks(0) {
	mouse = MouseState {};

}

void MouseManager::subscribe(mouse_callback_fn Function) {
	auto sharedFunction = std::make_shared<mouse_callback_fn>(Function);
	size_t newIndex = usedCallbacks;
	mouseCallbacks[newIndex] = sharedFunction;
	mouseCallbackToIndex.insert({ sharedFunction, newIndex });

	++usedCallbacks;
}

void MouseManager::unsubscribe(mouse_callback_fn Function) {
	auto sharedFunction = std::make_shared<mouse_callback_fn>(Function);

	size_t indexToRemove = mouseCallbackToIndex[sharedFunction];
	size_t lastIndex = usedCallbacks;

	mouseCallbacks[indexToRemove] = mouseCallbacks[lastIndex];
	mouseCallbacks[lastIndex] = 0;

	mouseCallbackToIndex.erase(sharedFunction);
	
	--usedCallbacks;
}

void MouseManager::updateMouseState(double xpos, double ypos) {
	if (!mouse.lastMouseX || !mouse.lastMouseY) {
		mouse.lastMouseX = xpos;
		mouse.lastMouseY = ypos;
	}

	double xo = xpos - mouse.lastMouseX;
	double yo = mouse.lastMouseY - ypos;

	mouse.lastMouseX = xpos;
	mouse.lastMouseY = ypos;
	mouse.deltaX = xo;
	mouse.deltaY = yo;

	callbackAll();
}

void MouseManager::callbackAll() {
	for (size_t i = 0; i < usedCallbacks; i++) {
		const auto& Function = mouseCallbacks[i];
		(*Function.get())(mouse);
	}
}