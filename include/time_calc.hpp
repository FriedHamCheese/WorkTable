#ifndef timecalc_hpp
#define timecalc_hpp

#include "Timescale.hpp"
#include <chrono>

std::chrono::year_month_day get_current_ymd() noexcept;

std::chrono::year_month_day get_next_week(const std::chrono::year_month_day& ymd) noexcept;
std::chrono::year_month_day get_next_twoweeks(const std::chrono::year_month_day& ymd) noexcept;
std::chrono::year_month_day get_next_month(const std::chrono::year_month_day& ymd) noexcept;
std::chrono::year_month_day get_next_interval(const std::chrono::year_month_day& current_ymd, const Timescale timescale);

std::chrono::days delta_days(const std::chrono::year_month_day& a, const std::chrono::year_month_day& b) noexcept;
#endif