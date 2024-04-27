/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

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

std::chrono::year_month_day Task::due_date() const{
	return _due_date;
}

void Task::due_date(const std::chrono::year_month_day& new_due_date){
	_due_date = new_due_date;
	_days_remaining = delta_days(new_due_date, get_current_ymd());
}	

std::chrono::days Task::days_remaining() const{
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

bool Task::due_date_is_later(const Task& lhs, const Task& rhs) noexcept{
	return lhs.due_date() > rhs.due_date();
}

