/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef timecalc_hpp
#define timecalc_hpp

#include "Timescale.hpp"
#include <chrono>

/**
\file time_calc.hpp
Provides utilities for manipulating dates.
*/

///Returns current date of today.
std::chrono::year_month_day get_current_ymd();

///Returns the date which is a week ahead of the provided date.
std::chrono::year_month_day get_next_week(const std::chrono::year_month_day& ymd);
///Returns the date which is 2 weeks ahead of the provided date.
std::chrono::year_month_day get_next_twoweeks(const std::chrono::year_month_day& ymd);
///Returns the date which is a month ahead of the provided date.
std::chrono::year_month_day get_next_month(const std::chrono::year_month_day& ymd);
///Returns the date which is timescale ahead of the provided date.
std::chrono::year_month_day get_next_interval(const std::chrono::year_month_day& current_ymd, const Timescale timescale);

///Returns the amount of days from a - b. The days from the subtraction can be a negative number.
std::chrono::days delta_days(const std::chrono::year_month_day& a, const std::chrono::year_month_day& b);
#endif