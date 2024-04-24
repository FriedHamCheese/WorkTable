#ifndef bar_hpp
#define bar_hpp

#include "Task.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include <string>
#include <chrono>
#include <memory>

struct BarConstructorArgs;

/**
A variant of TaskGroup for Bar. It provides sorted tasks from nearest due date to furthest,
ease of access to nearest and furthest due date tasks,
and methods of mutation which guarantees sorted tasks.

\note When attempting to add or delete a task, or merge with another Bar_TaskGroup, one should use the methods provided in the class,
  in order to ensure the tasks are sorted.
  If you just want to iterate over the tasks without mutating any, you can access the tasks directly like TaskGroup.
*/
class Bar_TaskGroup : public TaskGroup{	
	private:
	Task _nearest_due_date_task;
	Task _furthest_due_date_task;
	///Sort the underlying TaskGroup::tasks and reassign this->_nearest_due_date_task and this->furthest_due_date_task.
	void sort_and_reassign();

	public:
	Bar_TaskGroup(const TaskGroup& taskgroup);
	const Task& nearest_due_date_task() const {return _nearest_due_date_task;}
	const Task& furthest_due_date_task() const {return _furthest_due_date_task;}
	
	///Adds the provided task to the underlying TaskGroup::tasks, re-sort it, and reassign the nearest and furthest tasks. 
	void add_task(const Task& task);
	///Merge other.tasks to this->tasks, re-sort this->tasks, and ressign the nearest and furthest tasks.
	///The group name is of the object being called, this->name.
	void merge_taskgroup(const TaskGroup& other);
	///Deletes the task at the given index. Returns true if the index is valid, else false and does nothing to the container.
	void delete_task_at(const std::size_t index);
	///Sets the value of Task at given index to of the provided argument.
	///Returns true if index is valid, else false and does nothing to the container.
	void set_task_at(const std::size_t index, const Task& other);
	
	///Equivalent to this->tasks[index].
	const Task& operator[] (const std::size_t index) const{return this->tasks[index];}
};

/**
An Fl_Button which represents and keeps track of either a single task or a group of tasks.
*/
class Bar : public Fl_Button{
	public:
	Bar(const BarConstructorArgs& args);
	///Constructor for the one with BarConstructorArgs as parameter to call, no reason to use this.
	Bar(const int xpos, const int ypos, const int width, const int height, const Bar_TaskGroup& taskgroup, const std::chrono::days& days_from_interval);
	
	///Merges its Bar_TaskGroup with the provided group, updates its width to of the task with furthest due date, and updates its text.
	void merge_taskgroup(const TaskGroup& taskgroup);
	
	///Updates its group name. This does not affect the label of a Bar which represent a single task.
	void update_group_name(const char* const group_name);
	///Update the properties of its task, then its width, color, and label. This is called if the Bar represents a single task, but modifies the first task if it represents a group.
	void update_task(const char* const task_name, const std::chrono::year_month_day& due_date, const std::chrono::days& days_from_interval, const int parent_xpos);
	///Updates both xpos and width of the Bar according to its Bar_TaskGroup.
	void update_width(const std::chrono::days& days_from_interval, const int parent_xpos);
	
	///Returns true if it represents a single task, false for a group.
	bool is_single_task() const {return this->taskgroup.tasks.size() == 1;}
	///Gets the task which the Bar represent. If the Bar represents a group, it returns the first Task in its Bar_TaskGroup.
	Task get_single_task() const {return this->taskgroup.tasks[0];}
	///Returns an upcasted copy of its Bar_TaskGroup.
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

/**
A struct which simplifies the initialisation of the Bar constructor parameters.
*/
struct BarConstructorArgs{
	/**
	\param[in] parent The pointer of its parent, BarGroup.
	\param[in] taskgroup A TaskGroup which the Bar will represent.
	\param[in] task_count The amount of bars in BarGroup, including the Bar which the constructor is about to construct its parameters.
	\param[in] item_index The index of this Bar in BarGroup's Bar container, which its parameters are being constructed.
	*/
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