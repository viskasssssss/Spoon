#pragma once

#include <functional>
#include <string>
#include <sstream>

#include "def.hh"


namespace spoon {

	enum class event_type
	{
		none = 0,
		window_close, window_resize, window_focus, window_lost_focus, window_moved,
		app_tick, app_update, app_render,
		key_pressed, key_released, key_typed,
		mouse_button_pressed, mouse_button_released, mouse_moved, mouse_scrolled
	};

	enum event_category
	{
		NONE = 0,
		EVENT_CATEGORY_APPLICATION    = BIT(0),
		EVENT_CATEGORY_INPUT          = BIT(1),
		EVENT_CATEGORY_KEYBOARD       = BIT(2),
		EVENT_CATEGORY_MOUSE          = BIT(3),
		EVENT_CATEGORY_MOUSE_BUTTON    = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static event_type get_static_type() { return event_type::type; }\
								virtual event_type get_event_type() const override { return get_static_type(); }\
								virtual const char* get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int get_category_flags() const override { return category; }

	class event
	{
	public:
		virtual ~event() = default;

		bool handled = false;

		virtual event_type get_event_type() const = 0;
		virtual const char* get_name() const = 0;
		virtual int get_category_flags() const = 0;
		virtual std::string to_string() const { return get_name(); }

		bool is_in_category(event_category category)
		{
			return get_category_flags() & category;
		}
	};

	class event_dispatcher
	{
	public:
		event_dispatcher(event& event)
			: m_event(event)
		{
		}
		
		template<typename T, typename F>
		bool dispatch(const F& func)
		{
			if (m_event.get_event_type() == T::get_static_type())
			{
				m_event.handled |= func(static_cast<T&>(m_event));
				return true;
			}
			return false;
		}
	private:
		event& m_event;
	};

	inline std::ostream& operator<<(std::ostream& os, const event& e)
	{
		return os << e.to_string();
	}

}
