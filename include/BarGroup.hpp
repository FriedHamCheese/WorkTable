/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef bargroup_hpp
#define bargroup_hpp

#include "Bar.hpp"
#include "Task.hpp"
#include "Timescale.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>

#include <vector>
#include <memory>
#include <cstdint>

/**
A container and drawing area for bars. It is responsible for:
- Being a container for bars and passing requests from and to individual bars.
- Keeping track of current timescale and requesting the bars to render accordingly.
- Keeping track of the view mode, request and act accordingly.
- Keeping track of unsaved changes made to tasks, once its deconstructor is called, it is responsible for giving the user the chance to save their tasks.
*/
class BarGroup : public Fl_Group{
	public:
	///Initialises its members, but most importantly: constructs the bars from the task file.
	///
	///Catches exceptions and display an error window when error occurs while loading tasks to bars.
	BarGroup(const int xpos, const int ypos, const int width, const int height);
	
	///Check if there are any unsaved changes when the program is exiting. If there is, show a window for the user to save.
	///
	///Catches exceptions and display an error window if errors occurred while displaying an option window or saving tasks to file.
	virtual ~BarGroup();
	
	///Constucts a new Bar which represent the provided task, readjust vertical scale, and redraw BarGroup.
	void add_task(const Task& task);
	///Deletes the Bar at given index, readjust vertical scale, and redraw BarGroup. 
	///The Bar may either represent a task or a group.
	///\returns false if the index refers to an invalid Bar or the index is invalid for this->paged_taskgroups if in group view, else true.
	bool delete_bar(const int item_index);
	///Request the Bar at the provided index to change its task properties accordingly. This redraws BarGroup.
	///
	///Throws std::invalid_argument if the index refers to an invalid Bar.
	void modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index);
	///Request the Bar at the provided index to change its group name. This redraws BarGroup.
	///\returns false if the index refers to an invalid Bar.
	bool modify_group(const char* const group_name, const int item_index);

	///Passes request for showing a window for editing task, from Bar to MainWindow.
	///\returns false if the Bar is not a member of BarGroup.
	bool request_window_for_editing_task(const Bar* const bar);
	///Passes request for showing a window for editing group, from a Bar to MainWindow.	
	///\returns false if the Bar is not a member of BarGroup.	
	bool request_window_for_editing_group(const Bar* const bar);
	
	///Save the bars in root view to this->paged_taskgroups and shift from root view to group view.
	///
	///Throws std::invalid_argument if the pointer is not a member of BarGroup.
	///This redraws BarGroup.
	void display_tasks_in_task_group(const Bar* const bar);
	///Shift from group view back to root view. This redraws BarGroup.
	void show_taskgroups();
	
	///Saves the current state of all tasks and groups to task file. 
	///The function is able to be called while in root view and group view as well.
	void save_tasks_to_file();
	///Reverts the current state of all tasks and groups back to of the task file.
	///The function is able to be called while in root view and group view as well.	
	///
	///May throw std::ios_base::failure for unspecified IO error or std::runtime_error when the file cannot be opened.
	void revert_to_tasks_from_file();
	
	///Called by its bars when the user released a mouse button after dragging the bar to an area which is not the same bar.
	///
	///This function will either: move the task out of the task group and hide the root group box of MainWindow, merge the bar with another bar, or do nothing if the bar was released elsewhere.
	void handle_bar_mouse_button_release(const Bar* const clicked_bar);
	///This function will request MainWindow to show the root group box if the current view mode is in group view.
	///Else do nothing.
	void signal_bar_being_dragged();
	///Requests MainWindow to hide its root group box.
	void signal_hide_root_group_box();
	
	std::chrono::days get_days_from_interval() const;
	
	//layout specific code
	///Returns the horizontal offset of this->current_date_label. This is used by MainWindow for alignment of its widgets.
	int current_date_label_xpos() const;
	///Returns the xpos of the right edge of this->interval_date_label. Which is just its xpos + width. This is used by MainWindow for alignment of its widgets.	
	int xpos_right_of_interval_date_label() const;
	
	///Returns this->unsaved_changes_made_to_tasks.
	bool has_unsaved_changes_to_tasks() const;

	///Attempts to narrow down the timescale, if the timescale is already the narrowest, no visual changes will be made. 
	///\returns the current timescale after being narrowed down.
	Timescale zoomin_timescale();
	///Attempts to widen the timescale, if the timescale is already the widest, no visual changes will be made. 
	///\returns the current timescale after being widen.	
	Timescale zoomout_timescale();
	
	static constexpr int width = 1400;	///< Value for width argument for the caller constructing this object.
	static constexpr int height = 700;	///< Value for height argument for the caller constructing this object.
	static constexpr int bar_xoffset = 50;	///< Horizontal offset for bars which do not have overdue tasks.
	/**
	Width for bars which do have overdue tasks or is one. 
	- If the Bar is a group with overdue and non-overdue tasks intermixed, calculate its regular width and add this to the width.
	- If the Bar is a single overdued task, use this as its width. Else use the regular calculated width at non-overdue xoffset.
	*/
	static constexpr int overdue_bar_width = bar_xoffset; 
	/**
	This value is used for calculating non-overdue bar widths.	
	We subtract the first [bar_xoffset] pixels because the interval line (right line) is drawn [bar_xoffset] pixels before reaching full width of BarGroup.
	The other [bar_xoffset] pixels are for non-overdue bars which have [bar_xoffset] pixels of xoffset because the current date line (left line) is drawn [bar_xoffset] pixels after the xpos of BarGroup and that is where non-overdues start.
	*/
	static constexpr int bar_max_width = width - (2*bar_xoffset);
	
	protected:
	///Overrided Fl_Group::draw() to draw the left and right lines.
	void draw() override;
		
	private:
	///Constucts bars which represent tasks and groups in task file.
	///
	///May throw std::ios_base::failure for unspecified IO error or std::runtime_error when the file cannot be opened.	
	void load_tasks_to_bars();
	
	/**
	Constructs a bar and takes ownership of it. It does not adjust BarGroup's vertical layout or redraw.
	This function is used for adding multiple bars without recalculating, readjusting and redrawing for performance.
	
	The bar may either represent a single task or a group, depending on if the provided TaskGroup has a single task or multiple tasks in it, respectively.
	*/
	void add_bar(const TaskGroup& taskgroup, const int total_items, const int item_index);
	
	/**
	Constructs a bar, takes ownership of it, readjusts vertical layout and redraws BarGroup.
	This is convenient for adding a single bar.
	
	The bar may either represent a single task or a group, depending on if the provided TaskGroup has a single task or multiple tasks in it, respectively.
	*/
	void add_bar(const Task& task);
	
	///Returns the index of the pointer in this->bars. If the pointer is not in the container, a -1 is returned.
	int_least64_t get_item_index(const Bar* const bar) const;
	
	///Readjusts vertical layout in order to fit all of its bars properly without calling this->redraw().
	void adjust_vertical_layout();
	/**
	Recalculates the date timescale ahead from today, changes this->interval_date_label, and requests its bars to adjust its widths. This redraws BarGroup.
	If the timescale requested is the same of what BarGroup has, it does nothing.
	
	Catches a std::invalid_argument throw from invalid timescale enum and notifies by displaying a window.
	\returns The Timescale which BarGroup adjusted to.
	*/
	Timescale change_timescale(const Timescale timescale);
	
	///Returns the horizontal offset of the left line.
	int current_date_line_xpos() const;
	///Returns the horizontal offset of the right line.
	int next_interval_date_line_xpos() const;
	
	///Returns true if BarGroup is displaying group view, false for root view.
	bool displaying_a_taskgroup() const{return this->task_group_id != this->not_in_any_group;}
	///Returns true if a Bar was dropped in the root group box of MainWindow.
	bool check_mouse_released_in_root_group_box();
	
	private:
	std::vector<std::unique_ptr<Bar>> bars;
	
	/**
	This stores all of the TaskGroup of all the bars in root view in order to shift back from group view.
	
	You would iterate over each Bar in root view and store their TaskGroup before shifting to group view.
	If a task is added, modified, deleted or moved out while in group view, 
	it must be properly recorded to the container as in the perspective of root view as well.
	In root view, it is not necessary to access this.
	*/
	std::vector<TaskGroup> paged_taskgroups;
	/**
	This is the index of the task group being viewed in group view.
	
	This should be set accordingly in order to determine whether BarGroup is in root view or group view.
	If it is set to BarGroup::not_in_any_group, then BarGroup is assumed to be in root view, else in group view.
	It is also used for accessing and identifying the exact group in this->paged_taskgroups which is being displayed by BarGroup.
	*/
	std::int_least64_t task_group_id;
	///A value of this->task_group_id conveying BarGroup is not in any specific task group, i.e., BarGroup is in root view.
	///Use this->displaying_a_taskgroup() to determine if BarGroup is displaying a specific group or not.
	static constexpr const std::int_least64_t not_in_any_group = -1;
	
	Fl_Box current_date_label;
	Fl_Box interval_date_label;
		
	Timescale current_timescale;
	
	const std::chrono::year_month_day current_ymd;	
	std::chrono::year_month_day next_interval;

	bool unsaved_changes_made_to_tasks;
		
	static constexpr int date_label_yraise = 20;
	static constexpr int date_label_width = 70;
	static constexpr int date_label_height = 15;
};

#endif