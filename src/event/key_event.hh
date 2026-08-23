#pragma once

#include "event.hh"
#include "key_codes.hh"

namespace spoon {

	class key_event : public event
	{
	public:
		key_code get_key_code() const { return m_key_code; }

		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT)
	protected:
		key_event(const key_code keycode)
			: m_key_code(keycode) {}

		key_code m_key_code;
	};

	class key_pressed_event : public key_event
	{
	public:
		key_pressed_event(const key_code keycode, bool is_repeat = false)
			: key_event(keycode), m_is_repeat(is_repeat) {}

		bool is_repeat() const { return m_is_repeat; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "key_pressed_event: " << m_key_code << " (repeat = " << m_is_repeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(key_pressed)
	private:
		bool m_is_repeat;
	};

	class key_released_event : public key_event
	{
	public:
		key_released_event(const key_code keycode)
			: key_event(keycode) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "key_released_event: " << m_key_code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(key_released)
	};

	class key_typed_event : public key_event
	{
	public:
		key_typed_event(const key_code keycode)
			: key_event(keycode) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "key_typed_event: " << m_key_code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(key_typed)
	};
}
