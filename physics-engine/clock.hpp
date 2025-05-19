#pragma once

struct Clock {
	/* The current tick time. */
	double m_currTime;
	/* The previous tick time. */
	double m_prevTime;
	/* The difference between each tick. */
	double m_deltaTime;

	double m_physTime;
	/* The last time the render tick was executed. */
	double m_renderTime;
	/* The last time the fps tick was executed. */
	double m_fpsTime;

	/* Frequency of physics. */
	double m_physFreq = 1000.0f;
	/* Frequency of rendering. */
	double m_renderFreq = 244.0f;

	/* Flag determining if we should run physics. */
	bool m_physTick;
	/* Flag determining if we should run rendering. */
	bool m_renderTick;
	/* Flag determining if we should run fps timer. */
	bool m_fpsTick;

	/* Time between subsequent physics sim ticks. */
	double m_physDelta;
	/* Time between subsequent render ticks. */
	double m_renderDelta;
	/* Time between subsequent fps ticks. */
	double m_fpsDelta;

	Clock() = default;
	~Clock() = default;

	/* Update the clock times, set flags, etc. */
	void update();

	/* Are we in a physics tick? */
	inline bool isPhysicsTick() const  {
		return m_physTick;
	}

	/* Are we in a render tick? */
	inline bool isRenderTick() const {
		return m_renderTick;
	}

	/* Are we in an fps tick? */
	inline bool isFpsTick() const {
		return m_fpsTick;
	}
};
