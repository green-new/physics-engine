#include "key_subscription.hpp"

using namespace Input;

KeySubscription::KeySubscription() : keysList(), keyStates() {
    keySize = 0;
}

void KeySubscription::setKeyState(GLFWKey key, bool state) {
    keyStates.at(key) = state;
}