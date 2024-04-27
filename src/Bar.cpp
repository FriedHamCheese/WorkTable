/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Bar.hpp"
#include "BarGroup.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>

#include <FL/fl_draw.h>
#include <FL/fl_ask.h>

#include <cmath>
#include <string>
#include <iostream>

Bar_TaskGroup::Bar_TaskGroup(const TaskGroup& taskgroup)
:	TaskGroup(taskgroup)
{
	this->sort_and_reassign();
}

void Bar_TaskGroup::sort_and_reassign(){
	std::sort(this->tasks.begin(), this->tasks.end(), Task::due_date_is_earlier);
	this->_nearest_due_date_task = this->tasks.front();
	this->_furthest_due_date_task = this->tasks.back();	
}

void Bar_TaskGroup::add_task(const Task& task){
	this->tasks.push_back(task);
	this->sort_and_reassign();
}

void Bar_TaskGroup::merge_taskgroup(const TaskGroup& other){
	this->tasks.reserve(this->tasks.size() + other.tasks.size());
	for(const Task& task : other.tasks){
		this->tasks.push_back(task);
	}
	this->sort_and_reassign();
}

bool Bar_TaskGroup::set_task_at(const std::size_t index, const Task& other){
	if(index >= this->tasks.size()) return false;
	this->tasks[index] = other;
	this->sort_and_reassign();
	return true;
}

bool Bar_TaskGroup::delete_task_at(const std::size_t index){
	if(index >= this->tasks.size()) return false;
	
	const bool is_nearest_due_date_task = index == 0;
	const bool is_furthest_due_date_task = index == this->tasks.size();
	
	if(is_nearest_due_date_task)
		this->_nearest_due_date_task = this->tasks[1];
	if(is_furthest_due_date_task)
		this->_nearest_due_date_task = this->tasks[this->tasks.size() - 1];
	
	this->tasks.erase(this->tasks.begin() + index);
	return true;
}


Bar::Bar(const BarConstructorArgs& args)
:	Bar(args.xpos, args.ypos, args.width, args.height, args.taskgroup, args.days_from_interval)
{
}

Bar::Bar(const int xpos, const int ypos, const int width, const int height, const Bar_TaskGroup& taskgroup, const std::chrono::days& days_from_interval)
:	Fl_Button(xpos, ypos, width, height),
	taskgroup(taskgroup),
	days_from_interval(days_from_interval)
{
	this->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
	this->update_color_from_days_remaining();
	
	//This is a bit whacky,	but the idea is: we just want FLTK to render the text without the backgroud color,
	//as we will do that ourselves. So we tell FLTK to treat this is a no-background widget, draw our colors, and let FLTK draw the text only.
	this->box(FL_NO_BOX);	
	
	this->update_label();
}

void Bar::merge_taskgroup(const TaskGroup& other_taskgroup){
	const bool has_no_taskgroup_name = this->is_single_task();
	if(has_no_taskgroup_name) this->taskgroup.group_name = this->taskgroup.tasks[0].name();
	this->taskgroup.merge_taskgroup(other_taskgroup);
	this->update_width(this->days_from_interval, (this->parent())->x());
	this->update_label();
}


void Bar::update_group_name(const char* const group_name){
	this->taskgroup.group_name = group_name;
	this->update_label();
}

void Bar::update_task(const char* const task_name, const std::chrono::year_month_day& due_date, const std::chrono::days& days_from_interval, const int parent_xpos){
	this->taskgroup.group_name = task_name;
	
	const Task task {due_date, task_name};
	this->taskgroup.set_task_at(0, task);
	
	this->update_label();
	
	this->update_width(days_from_interval, parent_xpos);	
	this->update_color_from_days_remaining();	
}

void Bar::update_width(const std::chrono::days& days_from_interval, const int parent_xpos){
	const int bar_width = Bar::calc_bar_width(this->taskgroup.furthest_due_date_task().days_remaining(), days_from_interval);
	const bool all_tasks_are_overdue = bar_width <= 0;
	
	this->days_from_interval = days_from_interval;
	const bool use_overdue_position = this->taskgroup.nearest_due_date_task().days_remaining().count() < 1;
	
	if(use_overdue_position && all_tasks_are_overdue)
		this->resize(parent_xpos, this->y(), BarGroup::bar_xoffset, this->h());
	else if(use_overdue_position && !all_tasks_are_overdue)
		this->resize(parent_xpos, this->y(), bar_width + BarGroup::overdue_bar_width, this->h());
	else
		this->resize(parent_xpos + BarGroup::bar_xoffset, this->y(), bar_width, this->h());
}


//static public
int Bar::calc_height(const int height_with_yspacing) noexcept{
	return std::floor(float(height_with_yspacing) * float(bar_height_ratio_with_spacing)); 
}

int Bar::calc_height(const int bargroup_height, const int bar_count) noexcept{
	const float height_per_bar = float(bargroup_height) / float(bar_count);	
	return height_per_bar * bar_height_ratio_with_spacing;
}

int Bar::calc_height_with_yspacing(const int bargroup_height, const int bar_count) noexcept{
	const float height_per_bar = float(bargroup_height) / float(bar_count);		
	return int(height_per_bar);
}

int Bar::calc_ypos(const int parent_ypos, const int height_with_yspacing, const int item_index) noexcept{
	return parent_ypos + (height_with_yspacing * item_index);
}

int Bar::calc_bar_width(const std::chrono::days& days_remaining, const std::chrono::days& days_from_interval) noexcept{
	const float bar_group_ratio = float(days_remaining.count()) / float(days_from_interval.count());
	const float width = bar_group_ratio * float(BarGroup::bar_max_width);
	
	//just in case if days_remaining is negative
	return std::clamp(int(width), 0, BarGroup::bar_max_width);
}


//Protected
void Bar::draw(){
	const bool use_overdue_position = this->taskgroup.nearest_due_date_task().days_remaining().count() < 1;
	const bool all_tasks_are_overdue = this->taskgroup.furthest_due_date_task().days_remaining().count() < 1;

	//If all the tasks which the Bar represent is overdue, there would only be one color,
	//so no need to iterate over every task, as they are all overdue anyways.
	if(use_overdue_position && all_tasks_are_overdue){
		fl_draw_box(FL_FLAT_BOX, this->x(), this->y(), BarGroup::overdue_bar_width, this->h(), get_bar_color(this->taskgroup.furthest_due_date_task().days_remaining().count()));
	}	
	//This is when a task is overdue, but not all tasks are.
	//The xpos is at its parents' and the width is of the furthest due date plus the overdue width.
	//We reverse iterate because the sorted tasks are in nearest-to-furthest order, but we want to draw the further ones earlier than the nearer ones for overlay.
	else if(use_overdue_position && !all_tasks_are_overdue){
		for(std::int_least64_t i = this->taskgroup.tasks.size()-1; i >= 0; i--){
			const Task& task = this->taskgroup.tasks[i];
			const float of_days_from_interval = std::clamp(float(task.days_remaining().count()) / float(this->days_from_interval.count()), 0.0f, 1.0f);
			const float width = float(BarGroup::bar_max_width) * of_days_from_interval;
			fl_draw_box(FL_FLAT_BOX, this->x(), this->y(), width + BarGroup::overdue_bar_width, this->h(), get_bar_color(task.days_remaining().count()));
		}
	}
	//Start at regular bar xoffset with regular width.
	//We reverse iterate because the sorted tasks are in nearest-to-furthest order, but we want to draw the further ones earlier than the nearer ones for overlay.	
	else{
		for(std::int_least64_t i = this->taskgroup.tasks.size()-1; i >= 0; i--){
			const Task& task = this->taskgroup.tasks[i];
			const float of_days_from_interval = std::clamp(float(task.days_remaining().count()) / float(this->days_from_interval.count()), 0.0f, 1.0f);
			const float width = float(BarGroup::bar_max_width) * of_days_from_interval;
			fl_draw_box(FL_FLAT_BOX, this->x(), this->y(), width, this->h(), get_bar_color(task.days_remaining().count()));
		}
	}
	
	//Tells FLTK to draw the text only, we drew the colors manually.
	//We accomplished this by telling FLTK to treat this as a no-background widget (this->box(FL_NO_BOX)),
	//draw our colors manually, then tell FLTK to draw with its draw method.
	Fl_Button::draw();
}


int Bar::handle(const int event){
	constexpr int handled_event = 1;
	
	/*
	Tips:
	- Every drag is a mouse click and the click event is registered first. No dragging can occur if there were no clicking first.
	- A mouse release can either be from a click-only, or click-drag.
	- If you drag something and released it elsewhere, the release event is still registered as the thing you dragged, not the thing which was released on.
	- You can't click and release at different place without dragging.
	*/
	switch(event){
		//We don't respond directly when the mouse is clicked,
		//we wait for a mouse release button, as we need to differentiate a click-only from click-drag.
		case FL_PUSH:
			return handled_event;
		
		case FL_RELEASE:{
			//you can also drag and drop in the same bar and it would register as a click as well,
			//because every drag has a mouse click; we could try to discern between a drag and a click,
			//but I think this is fine.
			const bool clicked_and_released_same_bar = Fl::event_inside(this) != 0;
			if(clicked_and_released_same_bar){
				const int clicked_button = Fl::event_button();
				((BarGroup*)(this->parent()))->signal_hide_root_group_box();
				
				if(clicked_button == FL_LEFT_MOUSE)
					this->left_mouse_click_callback();
				if(clicked_button == FL_RIGHT_MOUSE)
					this->right_mouse_click_callback();
			}else{
				//the user dragged the mouse to other widget and releases the mouse button hold, pass this up.				
				((BarGroup*)(this->parent()))->handle_bar_mouse_button_release(this);
				//this widget may be deleted after the handle_drag_event, so no further access to the object.
			}
			//return is fine, it's on stack, not accessing the object.
			return handled_event;
		}		
		//Tell BarGroup that it is being dragged.
		case FL_DRAG:
			((BarGroup*)(this->parent()))->signal_bar_being_dragged();
			return handled_event;

		default:
			return Fl_Button::handle(event);
	}
}


//private
void Bar::update_label(){
	std::string label;
		//[Task[0] name] ([days] days)
	if(this->is_single_task()) 
		label = taskgroup.tasks[0].name() + " (" + std::to_string(taskgroup.tasks[0].days_remaining().count()) + " days)";
	else
		//[TaskGroup name] ([days] - [days] days)		
		label = taskgroup.group_name + " (" + std::to_string(taskgroup.nearest_due_date_task().days_remaining().count()) + "-" + std::to_string(taskgroup.furthest_due_date_task().days_remaining().count()) + " days)";
	
	this->copy_label(label.c_str());		
}

void Bar::update_color_from_days_remaining() noexcept{
	this->redraw();
}

void Bar::left_mouse_click_callback(){
	try{
		if(this->is_single_task())
			((BarGroup*)(this->parent()))->request_window_for_editing_task(this);
		else{
			((BarGroup*)(this->parent()))->display_tasks_in_task_group(this);
		}
	}
	catch(const std::bad_alloc& alloc_err){
		fl_alert("Caught memory allocation error while requesting window for editing TaskGroup. (Bar::left_mouse_click_callback())");
	}
	catch(const std::length_error& exceeded_max_alloc){
		fl_alert("Exceeded maximum memory allocation while requesting window for editing TaskGroup. (Bar::left_mouse_click_callback())");		
	}	
	catch(const std::exception& unspecified_excp){
		const std::string msg = std::string("Caught unspecified exception while requesting window for editing TaskGroup. (Bar::left_mouse_click_callback())\n")
								+ unspecified_excp.what();
		fl_alert(msg.c_str());									
	}
	catch(...){
		fl_alert("Caught unspecified throw while requesting window for editing TaskGroup. (Bar::left_mouse_click_callback())");				
	}	
}

void Bar::right_mouse_click_callback(){
	try{
		if(this->is_single_task())
			((BarGroup*)(this->parent()))->request_window_for_editing_task(this);
		else
			((BarGroup*)(this->parent()))->request_window_for_editing_group(this);
	}
	catch(const std::bad_alloc& alloc_err){
		fl_alert("Caught memory allocation error while requesting window for editing TaskGroup. (Bar::right_mouse_click_callback())");
	}
	catch(const std::length_error& exceeded_max_alloc){
		fl_alert("Exceeded maximum memory allocation while requesting window for editing TaskGroup. (Bar::right_mouse_click_callback())");		
	}	
	catch(const std::exception& unspecified_excp){
		const std::string msg = std::string("Caught unspecified exception while requesting window for editing TaskGroup. (Bar::right_mouse_click_callback())\n")
								+ unspecified_excp.what();
		fl_alert(msg.c_str());																	
	}
	catch(...){
		fl_alert("Caught unspecified throw while requesting window for editing TaskGroup. (Bar::right_mouse_click_callback())");				
	}		
}


BarConstructorArgs::BarConstructorArgs(const BarGroup* const parent, const TaskGroup& taskgroup, 
										const int bar_count, const int item_index)
:	taskgroup(taskgroup)
{
	const bool use_overdue_position = this->taskgroup.nearest_due_date_task().days_remaining().count() < 1;
	
	if(use_overdue_position){
		xpos = parent->x();
		const bool has_only_overdue_tasks = this->taskgroup.furthest_due_date_task().days_remaining().count() <= 0;
		if(has_only_overdue_tasks)
			width = BarGroup::overdue_bar_width;
		else
			width = BarGroup::overdue_bar_width + Bar::calc_bar_width(this->taskgroup.furthest_due_date_task().days_remaining(), parent->get_days_from_interval());
	}
	else{
		xpos = parent->x() + BarGroup::bar_xoffset;
		width = Bar::calc_bar_width(this->taskgroup.furthest_due_date_task().days_remaining(), parent->get_days_from_interval());
	}
	
	const int bar_height_with_yspacing = Bar::calc_height_with_yspacing(parent->h(), bar_count);
	ypos = Bar::calc_ypos(parent->y(), bar_height_with_yspacing, item_index);
	
	height = Bar::calc_height(bar_height_with_yspacing);
	this->days_from_interval = parent->get_days_from_interval();
}


Fl_Color get_bar_color(const int days_until_deadline) noexcept{
	
	constexpr Fl_Color red = 1;
	constexpr Fl_Color orange = 92;
	constexpr Fl_Color yellow = 134;
	constexpr Fl_Color green = 143;
	
	if(days_until_deadline < 2)
		return red;
	else if(days_until_deadline < 3)
		return orange;
	else if(days_until_deadline < 7)
		return yellow;
	else 
		return green;
}