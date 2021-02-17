#pragma once
#include <atomic>
#include <thread>
#include <functional>

class SpinlockBarrier
{
public:
	explicit SpinlockBarrier(const uint_fast32_t count) :
		m_count(count), m_generation(0),
		m_count_reset_value(count),
		m_post_phase_action(nullptr),
		m_post_phase_action_interval(1),
		m_post_phase_action_count(1)
	{
	}

	explicit SpinlockBarrier(const uint_fast32_t count, std::function<void()> post_phase_action) :
		m_count(count), m_generation(0),
		m_count_reset_value(count),
		m_post_phase_action(post_phase_action),
		m_post_phase_action_interval(1),
		m_post_phase_action_count(1)
	{
	}

	explicit SpinlockBarrier(const uint_fast32_t count, std::function<void()> post_phase_action, const uint_fast32_t post_phase_action_interval) :
		m_count(count), m_generation(0),
		m_count_reset_value(count),
		m_post_phase_action(post_phase_action),
		m_post_phase_action_interval(post_phase_action_interval),
		m_post_phase_action_count(post_phase_action_interval)
	{
	}

	void wait()
	{
		if (m_count_reset_value <= 1) {
			if (m_post_phase_action != nullptr && !--m_post_phase_action_count) {
				m_post_phase_action_count = m_post_phase_action_interval;
				m_post_phase_action();
			}
			return;
		}

		const auto gen = m_generation.load();
		if (!--m_count)
		{
			if (m_post_phase_action != nullptr && !--m_post_phase_action_count) {
				m_post_phase_action_count = m_post_phase_action_interval;
				m_post_phase_action();
			}

			m_count = m_count_reset_value;
			m_generation += 1;
			return;
		}

		while (gen == m_generation)
			std::this_thread::yield();
	}

	void setStageCount(const uint_fast32_t count)
	{
		m_count_reset_value = count;
		m_count = count;
	}
private:
	std::atomic<uint_fast32_t> m_count;
	std::atomic<uint_fast32_t> m_generation;
	uint_fast32_t m_count_reset_value;
	std::function<void()> m_post_phase_action;
	uint_fast32_t m_post_phase_action_interval;
	uint_fast32_t m_post_phase_action_count;
};
