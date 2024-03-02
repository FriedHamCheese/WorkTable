#ifndef Task_hpp
#define Task_hpp

#include "time_calc.hpp"

#include <string>
#include <chrono>

class Task{
	public:
	Task(const std::chrono::year_month_day& due_date, const std::string& name);	
	Task(const std::chrono::year_month_day& due_date, const std::string& name, const std::chrono::year_month_day& current_ymd);
	
	std::chrono::year_month_day due_date() const;
	std::chrono::days days_remaining() const;
	
	std::string name() const;
	
	void due_date(const std::chrono::year_month_day& new_due_date);
	void name(const std::string& new_name);

	static bool due_date_is_earlier(const Task& lhs, const Task& rhs) noexcept;	
	
	private:
	std::chrono::year_month_day _due_date;
	std::chrono::days _days_remaining;
	std::string _name;
};

#endif