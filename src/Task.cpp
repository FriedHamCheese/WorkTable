#include "Task.hpp"
#include "time_calc.hpp"

#include <string>
#include <chrono>

Task::Task(const std::chrono::year_month_day& due_date, const std::string& name)
:	Task(due_date, name, get_current_ymd())
{
	//nothing here :D
}

Task::Task(const std::chrono::year_month_day& due_date, const std::string& name, const std::chrono::year_month_day& current_ymd)
:	_due_date(due_date), _days_remaining(delta_days(due_date, current_ymd)), _name(name)
{
	//nothing here :D
}

std::chrono::year_month_day Task::due_date() const noexcept{
	return _due_date;
}

void Task::due_date(const std::chrono::year_month_day& new_due_date) noexcept{
	_due_date = new_due_date;
	_days_remaining = delta_days(new_due_date, get_current_ymd());
}	

std::chrono::days Task::days_remaining() const noexcept{
	return _days_remaining;
}

std::string Task::name() const{
	return _name;
}

void Task::name(const std::string& new_name){
	_name = new_name;
}

bool Task::due_date_is_earlier(const Task& lhs, const Task& rhs) noexcept{
	return lhs.due_date() < rhs.due_date();
}


bool task::due_date_is_earlier(const std::chrono::year_month_day& lhs, const std::chrono::year_month_day& rhs) noexcept{
	return lhs < rhs;
}

