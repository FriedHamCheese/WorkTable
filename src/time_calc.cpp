/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

#include "time_calc.hpp"
#include "Timescale.hpp"

#include <chrono>
#include <string>
#include <stdexcept>

std::chrono::year_month_day get_current_ymd(){
	const std::chrono::time_point now(std::chrono::system_clock::now());
    return std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(now));
}

std::chrono::year_month_day get_next_week(const std::chrono::year_month_day& ymd){
	const std::chrono::sys_days ymd_in_days = std::chrono::sys_days(ymd);
	const std::chrono::sys_days ymd_next_week = ymd_in_days + std::chrono::days(7);
	
	return std::chrono::year_month_day(ymd_next_week);
}

std::chrono::year_month_day get_next_twoweeks(const std::chrono::year_month_day& ymd){
	const std::chrono::sys_days ymd_in_days = std::chrono::sys_days(ymd);
	const std::chrono::sys_days ymd_next_week = ymd_in_days + std::chrono::days(14);
	
	return std::chrono::year_month_day(ymd_next_week);
}

std::chrono::year_month_day get_next_month(const std::chrono::year_month_day& ymd){
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

std::chrono::days delta_days(const std::chrono::year_month_day& a, const std::chrono::year_month_day& b){	
	return std::chrono::sys_days(a) - std::chrono::sys_days(b);
}