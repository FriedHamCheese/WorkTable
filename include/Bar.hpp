/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

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
and methods for mutation which guarantee the tasks be in sorted order.

\note When attempting to add or delete a task, or merge with another TaskGroup, one should use the methods provided in the class,
  in order to ensure the tasks are sorted.
  If you just want to iterate over the tasks without mutating the due dates or the order of the tasks, you can access the tasks directly like TaskGroup.
*/
class Bar_TaskGroup : public TaskGroup{	
	private:
	Task _nearest_due_date_task;
	Task _furthest_due_date_task;
	///Sort the underlying TaskGroup::tasks and reassign this->_nearest_due_date_task and this->furthest_due_date_task.
	void sort_and_reassign();

	public:
	///Constructs the object from its base class, sort its task from nearest due date to furthest, 
	///then assign this->nearest_due_date_task and this->furthest_due_date_task.
	Bar_TaskGroup(const TaskGroup& taskgroup);
	const Task& nearest_due_date_task() const {return _nearest_due_date_task;}
	const Task& furthest_due_date_task() const {return _furthest_due_date_task;}
	
	///Adds the provided task to the underlying TaskGroup::tasks, re-sort it, and reassign the nearest and furthest tasks. 
	void add_task(const Task& task);
	///Merge other.tasks to this->tasks, re-sort this->tasks, and ressign the nearest and furthest tasks.
	///The group name is of the object being called, this->name.
	void merge_taskgroup(const TaskGroup& other);
	///Deletes the task at the given index. Returns true if the index is valid, else false and does nothing to the container.
	bool delete_task_at(const std::size_t index);
	///Sets the value of Task at given index to of the provided argument.
	///Returns true if index is valid, else false and does nothing to the container.
	bool set_task_at(const std::size_t index, const Task& other);
	
	///Equivalent to this->tasks[index].
	const Task& operator[] (const std::size_t index) const{return this->tasks[index];}
};

/**
An Fl_Button which represents and keeps track of either a single task or a group of tasks.
*/
class Bar : public Fl_Button{
	public:
	///The preferred constructor.
	Bar(const BarConstructorArgs& args);
	///Constructor for the one with BarConstructorArgs as parameter to call, no reason to use this.
	Bar(const int xpos, const int ypos, const int width, const int height, const Bar_TaskGroup& taskgroup, const std::chrono::days& days_from_interval);
	
	///Merges its Bar_TaskGroup with the provided group, updates its width to of the task with furthest due date, and updates its text.
	void merge_taskgroup(const TaskGroup& taskgroup);
	
	///Updates its group name. This does not affect the label of a Bar which represent a single task.
	void update_group_name(const char* const group_name);
	///Update the properties of its task, then its width, color, and label. 
	///This is from a chain of requests where the task edit window, which the Bar has requested if it represents a single task, requests the same Bar to modify its contents.
	///If somehow this is called and the Bar represents a group, it modifies the first task.
	void update_task(const char* const task_name, const std::chrono::year_month_day& due_date, const std::chrono::days& days_from_interval, const int parent_xpos);
	/**
	Updates both xpos and width of the Bar according to its Bar_TaskGroup.
	There are 3 cases which result in different xpos and width:
	- The Bar represents a single task and is overdue, or a group with all the tasks overdue: xpos = parent_xpos, width = BarGroup::overdue_bar_width.
	- The Bar represents a single task and is not overdue, or a group with no overdue tasks: xpos = parent_xpos + BarGroup::bar_xoffset;  width = Bar::calc_bar_width().
	- The Bar represents a group with intermixed overdue and non-overdue tasks: xpos = parent_xpos + BarGroup::bar_xoffset; width = BarGroup::overdue_bar_width + Bar::calc_bar_width().
	
	xpos at parent_xpos means it is behind the current date line (the left line), with BarGroup::bar_xoffset added makes the Bar to start exactly at the line.
	*/
	void update_width(const std::chrono::days& days_from_interval, const int parent_xpos);
	
	///Returns true if it represents a single task, false for a group.
	bool is_single_task() const {return this->taskgroup.tasks.size() == 1;}
	///Gets the task which the Bar represent. If the Bar represents a group, it returns the first Task in its Bar_TaskGroup.
	Task get_single_task() const {return this->taskgroup.tasks[0];}
	///Returns an upcasted copy of its Bar_TaskGroup.
	TaskGroup get_taskgroup() const {return this->taskgroup;}
	
	///Calculates the Bar's height, from the given height of a Bar with yspacing between the Bar below it included.
	static int calc_height(const int height_with_yspacing) noexcept;
	///Calculates the Bar's height from BarGroup's height and the bar count in BarGroup.
	static int calc_height(const int bargroup_height, const int bar_count) noexcept;
	///Calculates the Bar's height with its spacing between other bars, with BarGroup's height and the bar count in BarGroup.
	static int calc_height_with_yspacing(const int bargroup_height, const int bar_count) noexcept;
	///Calculates the Bar's ypos from BarGroup's ypos, value for the height of a Bar with yspacing between the Bar below included, and the index of the Bar in BarGroup's container of bars.
	static int calc_ypos(const int parent_ypos, const int height_with_yspacing, const int item_index) noexcept;
	///Calculates width of the Bar, if days_remaining is not positive, the return value is clamped to 0 pixels.
	///If the return is 0, the task is overdue. You should use BarGroup::overdue_bar_width as the width and move the Bar's xpos to overdue position.
	static int calc_bar_width(const std::chrono::days& days_remaining, const std::chrono::days& days_from_interval) noexcept;

	protected:
	///Overrides Fl_Button::draw() to draw multiple colors in same button.
	void draw() override;
	/**
	Overrided to handle click, drag and release of mouse controls.
	- When clicked and released, or clicked-dragged-released in same Bar, it should pop up a window for editing a task or for a group of the exact same Bar.
	- When dragged, let BarGroup know.
	- when dragged and released somewhere else, notify BarGroup.
	*/
	int handle(const int event) override;

	private:
	///A container for a task or multiple tasks, the Bar has to behave based on if this has a single task or multiple tasks (group).
	Bar_TaskGroup taskgroup;
	///Days from interval, the interval is just the date timescale ahead of current date.
	///Which just means how many days are in the selected timescale.
	std::chrono::days days_from_interval;	
	
	///Update the label (text) of the Bar according to what it represents.
	void update_label();
	///Relic from 0.4. Because this is taken care of by the draw() override, this just calls redraw.
	void update_color_from_days_remaining() noexcept;
	
	///Will either request a window for a task or shift to group view, depending on if the Bar represent a task or a group respectively.
	///
	///This function catches throws from requesting a window for editing a task or requesting shifting to group view, and will display a window if an error occurred.	
	void left_mouse_click_callback();
	///Will either request a window for a task or for a group, depending on if the Bar represent a task or a group respectively.
	///
	///This function catches throws from requesting windows and will display a window if an error occurred.
	void right_mouse_click_callback();
	
	///3/4 of the height would be the actual height, leave the remaining for spacing between the bars.
	static constexpr float bar_height_ratio_with_spacing = 0.75;
	///The inverse of Bar::bar_height_ratio_with_spacing.
	static constexpr float bar_yspacing_ratio = 1 - bar_height_ratio_with_spacing;
};

class BarGroup;

/**
A struct which simplifies the initialisation of the Bar constructor parameters.
*/
struct BarConstructorArgs{
	/**
	\param parent The pointer of its parent, BarGroup.
	\param taskgroup A TaskGroup which the Bar will represent.
	\param task_count The amount of bars in BarGroup, including the Bar which the constructor is about to construct its parameters.
	\param item_index The index of this Bar in BarGroup's Bar container, which its parameters are being constructed.
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