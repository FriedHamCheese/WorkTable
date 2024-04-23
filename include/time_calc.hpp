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