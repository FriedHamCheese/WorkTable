#ifndef bar_hpp
#define bar_hpp

#include "Task.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include <string>
#include <chrono>
#include <memory>

struct BarConstructorArgs;

class Bar : public Fl_Button{
	public:
	Bar(const BarConstructorArgs& args);
	Bar(const int xpos, const int ypos, const int width, const int height, const Task& task_properties);
	
	void update_task(const char* const task_name, const std::chrono::year_month_day& due_date, const std::chrono::days& days_from_interval);
	void update_width(const std::chrono::days& days_from_interval);
	std::chrono::year_month_day get_due_date() const;
	Task get_task_properties() const noexcept {return this->task_properties;}

	static int calc_height(const int height_with_yspacing) noexcept;
	static int calc_height(const int timeline_height, const int task_count) noexcept;
	static int calc_height_with_yspacing(const int timeline_height, const int task_count) noexcept;
	static int calc_ypos(const int parent_ypos, const int height_with_yspacing, const int item_index) noexcept;
	
	static int calc_bar_width(const std::chrono::days& days_remaining, const std::chrono::days& days_from_interval) noexcept;
		
	static void bar_callback(Fl_Widget* const self, void* const data) noexcept;
	
	static bool due_date_is_earlier(const Bar* const lhs, const Bar* const rhs) noexcept;

	private:
	Task task_properties;	
	
	void update_label();
	void update_color_from_days_remaining() noexcept;
	
	//3/4 of the height would be the actual height, leave the remaining for spacing between the bars.
	static constexpr float bar_height_ratio_with_spacing = 0.75;
	static constexpr float bar_yspacing_ratio = 1 - bar_height_ratio_with_spacing;
};

class BarGroup;

struct BarConstructorArgs{
	BarConstructorArgs(const BarGroup* const parent, const Task& task_properties, 
						const int task_count, const int item_index);
	
	int xpos;
	int ypos;
	int width;
	int height;
	Task task_properties;
};

Fl_Color get_bar_color(const int days_until_deadline) noexcept;

#endif