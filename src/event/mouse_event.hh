#pragma once

#include "event.hh"
#include "mouse_codes.hh"

namespace spoon {

	class mouse_moved_event : public event
	{
	public:
		mouse_moved_event(const float x, const float y)
			: m_mouse_x(x), m_mouse_y(y) {}

		float get_x() const { return m_mouse_x; }
		float get_y() const { return m_mouse_y; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "mouse_moved_event: " << m_mouse_x << ", " << m_mouse_y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(mouse_moved)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)
	private:
		float m_mouse_x, m_mouse_y;
	};

	class mouse_scrolled_event : public event
	{
	public:
		mouse_scrolled_event(const float x_offset, const float y_offset)
			: m_x_offset(x_offset), m_y_offset(y_offset) {}

		float get_x_offset() const { return m_x_offset; }
		float get_y_offset() const { return m_y_offset; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "mouse_scrolled_event: " << get_x_offset() << ", " << get_y_offset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(mouse_scrolled)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)
	private:
		float m_x_offset, m_y_offset;
	};

	class mouse_button_event : public event
	{
	public:
		mouse_code get_mouse_button() const { return m_button; }

		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT | EVENT_CATEGORY_MOUSE_BUTTON)
	protected:
		mouse_button_event(const mouse_code button)
			: m_button(button) {}

		mouse_code m_button;
	};

	class mouse_button_pressed_event : public mouse_button_event
	{
	public:
		mouse_button_pressed_event(const mouse_code button)
			: mouse_button_event(button) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "mouse_button_pressed_event: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(mouse_button_pressed)
	};

	class mouse_button_released_event : public mouse_button_event
	{
	public:
		mouse_button_released_event(const mouse_code button)
			: mouse_button_event(button) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "mouse_button_released_event: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(mouse_button_released)
	};

}
