#pragma once

#include <stdint.h>
#include <string>

class Config {
public:
	Config() : m_width(1920), m_height(1080) { }
	~Config() = default;
public:
	void readConfig(const std::string& filename);

	inline uint16_t getWidth() const {
		return m_width;
	}

	inline uint16_t getHeight() const {
		return m_height;
	}
private:
	uint16_t m_width;
	uint16_t m_height;
};

