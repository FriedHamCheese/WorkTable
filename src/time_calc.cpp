#include "time_calc.hpp"
#include "Timescale.hpp"

#include <chrono>
#include <string>
#include <stdexcept>

std::chrono::year_month_day get_current_ymd() noexcept{
	const std::chrono::time_point now(std::chrono::system_clock::now());
    return std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(now));
}

std::chrono::year_month_day get_next_week(const std::chrono::year_month_day& ymd) noexcept{
	const std::chrono::sys_days ymd_in_days = std::chrono::sys_days(ymd);
	const std::chrono::sys_days ymd_next_week = ymd_in_days + std::chrono::days(7);
	
	return std::chrono::year_month_day(ymd_next_week);
}

std::chrono::year_month_day get_next_twoweeks(const std::chrono::year_month_day& ymd) noexcept{
	const std::chrono::sys_days ymd_in_days = std::chrono::sys_days(ymd);
	const std::chrono::sys_days ymd_next_week = ymd_in_days + std::chrono::days(14);
	
	return std::chrono::year_month_day(ymd_next_week);
}

std::chrono::year_month_day get_next_month(const std::chrono::year_month_day& ymd) noexcept{
	const std::chrono::sys_days ymd_in_days = std::chrono::sys_days(ymd);
	
	const auto last_day_of_current_month = std::chrono::year_month_day_last(ymd.year(), std::chrono::month_day_last(ymd.month()));
	const auto days_in_current_month = std::chrono::days(unsigned(last_day_of_current_month.day()));
		
	return std::chrono::year_month_day(ymd_in_days + days_in_current_month);
}

std::chrono::year_month_day get_next_interval(const std::chrono::year_month_day& current_ymd, const Timescale timescale){
	switch(timescale){
		case Timescale::Week:
		return get_next_week(current_ymd);
			
		case Timescale::TwoWeeks:
		return get_next_twoweeks(current_ymd);
			
		case Timescale::Month:
		return get_next_month(current_ymd);
		
		default:
		throw std::invalid_argument(std::string("time_calc.cpp: get_next_interval(): timescale argument not supported in switch case") + " (" + std::to_string(timescale::timescale_to_int(timescale)) + ")\n");		
	}
}

std::chrono::days delta_days(const std::chrono::year_month_day& a, const std::chrono::year_month_day& b) noexcept{	
	return std::chrono::sys_days(a) - std::chrono::sys_days(b);
}