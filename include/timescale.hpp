#ifndef Timescale_hpp
#define Timescale_hpp

#include <span>
#include <string>
#include <algorithm>
#include <stdexcept>

/**
\file timescale.hpp
Provides the Timescale enum and utilities simplifying the manipulation of the enum.
*/

/**
An enum for representing timescales. The enum value should be from most narrow to widest timescale.
\attention If any changes were made to the enum, you should verify the validity of the entire timescale.hpp as well.
*/
enum class Timescale{
	Week, TwoWeeks, Month
};

namespace timescale{
	inline constexpr Timescale default_timescale = Timescale::Month;

	inline constexpr Timescale smallest_timescale = Timescale::Week;
	inline constexpr Timescale widest_timescale = Timescale::Month;


	inline constexpr int smallest_timescale_int = static_cast<int>(smallest_timescale);
	inline constexpr int widest_timescale_int = static_cast<int>(widest_timescale);
	
	///Returns a timescale which is narrowed down (zoomed in) a step further from the provided argument.
	///If the argument already is the most narrow timescale, it returns the same value as the provided argument.
	inline constexpr Timescale zoomin_timescale(const Timescale timescale){
		const int timescale_int = static_cast<int>(timescale);
		return static_cast<Timescale>(std::clamp(timescale_int - 1, smallest_timescale_int, widest_timescale_int));
	}

	///Returns a timescale which is widen (zoomed out) a step further from the provided argument.
	///If the argument already is the widest timescale, it returns the same value as the provided argument.
	inline constexpr Timescale zoomout_timescale(const Timescale timescale){
		const int timescale_int = static_cast<int>(timescale);
		return static_cast<Timescale>(std::clamp(timescale_int + 1, smallest_timescale_int, widest_timescale_int));
	}

	///Converts the enum class to int.
	inline constexpr int timescale_to_int(const Timescale timescale){
		return static_cast<int>(timescale);
	}

	///String equivalent of the enum, you can use this directly, but it provides no size and bounds checking.
	///Use get_timescale_str() instead.
	inline constexpr const char* timescale_strs[] = {
		"1 Week", "2 Weeks", "1 Month"
	};

	///Returns the string equivalent of the provided timescale, bounds checking is performed.
	///If the timescale causes an out-of-bound access to the underlying string equivalent container, a std::invalid_argument is thrown.
	inline constexpr const char* get_timescale_str(const Timescale timescale){
		std::span timescale_strs_span(timescale_strs);
		const decltype(timescale_strs_span.size()) index = timescale_to_int(timescale);
		
		const bool valid_index = index >= 0 && index < timescale_strs_span.size();
		if(valid_index) return timescale_strs[index];
		else throw std::invalid_argument("timescale.hpp: get_timescale_str(): invalid timescale argument: " + std::to_string(index) + "\n");
	}
}


#endif