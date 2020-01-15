#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>

class SpinlockBarrier
{
public:
	explicit SpinlockBarrier(const unsigned int count) :
		m_count(count), m_generation(0),
		m_count_reset_value(count),
		m_post_phase_action(nullptr),
		m_post_phase_action_interval(1),
		m_post_phase_action_count(1)
	{
	}

	explicit SpinlockBarrier(const unsigned int count, std::function<void()> post_phase_action) :
		m_count(count), m_generation(0),
		m_count_reset_value(count),
		m_post_phase_action(post_phase_action),
		m_post_phase_action_interval(1),
		m_post_phase_action_count(1)
	{
	}

	explicit SpinlockBarrier(const unsigned int count, std::function<void()> post_phase_action, const unsigned int post_phase_action_interval) :
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

		const unsigned int gen = m_generation;
		if (!--m_count)
		{
			if (m_post_phase_action != nullptr && !--m_post_phase_action_count) {
				m_post_phase_action_count = m_post_phase_action_interval;
				m_post_phase_action();
			}

			m_count = m_count_reset_value;
			m_generation = gen + 1;
			return;
		}

		while (gen == m_generation)
			std::this_thread::yield();
	}

	void setStageCount(const unsigned int count)
	{
		m_count_reset_value = count;
		m_count = count;
	}
private:
	std::atomic<unsigned int> m_count;
	std::atomic<unsigned int> m_generation;
	unsigned int m_count_reset_value;
	std::function<void()> m_post_phase_action;
	unsigned int m_post_phase_action_interval;
	unsigned int m_post_phase_action_count;
};