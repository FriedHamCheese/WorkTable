#ifndef bar_hpp
#define bar_hpp

#include "Task.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include <string>
#include <chrono>
#include <memory>

struct BarConstructorArgs;
class Bar_TaskGroup : public TaskGroup{	
	private:
	Task _nearest_due_date_task;
	Task _furthest_due_date_task;
	void sort_and_reassign();

	public:
	Bar_TaskGroup(const TaskGroup& taskgroup);
	const Task& nearest_due_date_task() const {return _nearest_due_date_task;}
	const Task& furthest_due_date_task() const {return _furthest_due_date_task;}
	
	void add_task(const Task& task);
	void merge_taskgroup(const TaskGroup& other);
	void delete_task_at(const std::size_t index);
	
	const Task& operator[] (const std::size_t index) const{return this->tasks[index];}
	void set_task_at(const std::size_t index, const Task& other);
};

class Bar : public Fl_Button{
	public:
	Bar(const BarConstructorArgs& args);
	Bar(const int xpos, const int ypos, const int width, const int height, const Bar_TaskGroup& taskgroup, const std::chrono::days& days_from_interval);
	
	void merge_taskgroup(const TaskGroup& taskgroup);
	
	void update_task(const char* const task_name, const std::chrono::year_month_day& due_date, const std::chrono::days& days_from_interval, const int parent_xpos);
	void update_width(const std::chrono::days& days_from_interval, const int parent_xpos);
	
	bool is_single_task() const {return this->taskgroup.tasks.size() == 1;}
	Task get_single_task() const {return this->taskgroup.tasks[0];}
	TaskGroup get_taskgroup() const {return this->taskgroup;}
	
	static int calc_height(const int height_with_yspacing) noexcept;
	static int calc_height(const int timeline_height, const int task_count) noexcept;
	static int calc_height_with_yspacing(const int timeline_height, const int task_count) noexcept;
	static int calc_ypos(const int parent_ypos, const int height_with_yspacing, const int item_index) noexcept;
	
	static int calc_bar_width(const std::chrono::days& days_remaining, const std::chrono::days& days_from_interval) noexcept;
				
	protected:
	void draw() override;
	int handle(const int event) override;

	private:
	Bar_TaskGroup taskgroup;
	std::chrono::days days_from_interval;
	
	void update_label();
	void update_color_from_days_remaining() noexcept;
	
	void left_mouse_click_callback();
	void right_mouse_click_callback();
	
	//3/4 of the height would be the actual height, leave the remaining for spacing between the bars.
	static constexpr float bar_height_ratio_with_spacing = 0.75;
	static constexpr float bar_yspacing_ratio = 1 - bar_height_ratio_with_spacing;
};

class BarGroup;

struct BarConstructorArgs{
	BarConstructorArgs(const BarGroup* const parent, const TaskGroup& taskgroup, 
						const int task_count, const int item_index);
	
	int xpos;
	int ypos;
	int width;
	int height;
	std::chrono::days days_from_interval;
	Bar_TaskGroup taskgroup;
};

Fl_Color get_bar_color(const int days_until_deadline) noexcept;

#endif