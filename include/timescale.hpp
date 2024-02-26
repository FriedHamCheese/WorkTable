#ifndef Timescale_hpp
#define Timescale_hpp

#include <span>
#include <string>
#include <algorithm>
#include <stdexcept>

enum class Timescale{
	Week, TwoWeeks, Month
};

namespace timescale{
	inline constexpr Timescale default_timescale = Timescale::Month;

	inline constexpr Timescale smallest_timescale = Timescale::Week;
	inline constexpr Timescale widest_timescale = Timescale::Month;


	inline constexpr int smallest_timescale_int = static_cast<int>(smallest_timescale);
	inline constexpr int widest_timescale_int = static_cast<int>(widest_timescale);

	inline constexpr Timescale zoomin_timescale(const Timescale timescale) noexcept{
		const int timescale_int = static_cast<int>(timescale);
		return static_cast<Timescale>(std::clamp(timescale_int - 1, smallest_timescale_int, widest_timescale_int));
	}

	inline constexpr Timescale zoomout_timescale(const Timescale timescale) noexcept{
		const int timescale_int = static_cast<int>(timescale);
		return static_cast<Timescale>(std::clamp(timescale_int + 1, smallest_timescale_int, widest_timescale_int));
	}

	inline constexpr int timescale_to_int(const Timescale timescale){
		return static_cast<int>(timescale);
	}


	inline constexpr const char* timescale_strs[] = {
		"1 Week", "2 Weeks", "1 Month"
	};

	inline constexpr const char* get_timescale_str(const Timescale timescale){
		std::span timescale_strs_span(timescale_strs);
		const decltype(timescale_strs_span.size()) index = timescale_to_int(timescale);
		
		const bool valid_index = index >= 0 && index < timescale_strs_span.size();
		if(valid_index) return timescale_strs[index];
		else throw std::invalid_argument("timescale.hpp: get_timescale_str(): invalid timescale argument: " + std::to_string(index) + "\n");
	}
}


#endif