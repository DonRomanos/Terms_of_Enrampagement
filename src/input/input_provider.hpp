#pragma once

#include <vector>
#include <compare>

namespace input
{
	// This is kinda an anti corruption layer against glfw at the moment.
	struct Event
	{
		int key;
		int action;
		int modifiers;
	
		friend constexpr auto operator<=>(const Event& lhs, const Event& rhs) = default;
	};

	// Some special events for now, will be handled properly later.
	constexpr Event WindowShouldClose{ 1000,0,0 };

	class InputProvider
	{
	public:
		virtual const std::vector<Event>& poll_events() = 0;
		virtual ~InputProvider() = default;
	};
}