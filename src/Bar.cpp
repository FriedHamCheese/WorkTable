#include "Bar.hpp"
#include "BarGroup.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>

#include <FL/fl_draw.h>
#include <FL/fl_ask.h>

#include <string>
#include <iostream>

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
	this->box(FL_NO_BOX);
	
	this->update_label();
}

void Bar::merge_taskgroup(const TaskGroup& other_taskgroup){
	const bool has_no_taskgroup_name = this->is_single_task();
	if(has_no_taskgroup_name) this->taskgroup.group_name = this->taskgroup.tasks[0].name();
	this->taskgroup.merge_taskgroup(other_taskgroup);
	this->update_width(this->days_from_interval, (this->parent())->x());
}

void Bar::update_task(const char* const task_name, const std::chrono::year_month_day& due_date, const std::chrono::days& days_from_interval, const int parent_xpos){
	this->taskgroup.group_name = task_name;
	
	const Task task {due_date, task_name};
	this->taskgroup.set_task_at(0, task);
	
	this->update_label();
	
	this->update_width(days_from_interval, parent_xpos);	
	this->update_color_from_days_remaining();	
}

void Bar::update_group_name(const char* const group_name){
	this->taskgroup.group_name = group_name;
	this->update_label();
}

void Bar::update_width(const std::chrono::days& days_from_interval, const int parent_xpos){
	const int bar_width = Bar::calc_bar_width(this->taskgroup.furthest_due_date_task().days_remaining(), days_from_interval);
	this->days_from_interval = days_from_interval;
	
	const bool use_overdue_position = this->taskgroup.nearest_due_date_task().days_remaining().count() < 1;
	const bool all_tasks_are_overdue = bar_width <= 0;
	
	if(use_overdue_position && all_tasks_are_overdue)
		this->resize(parent_xpos, this->y(), BarGroup::bar_xoffset, this->h());
	else if(use_overdue_position && !all_tasks_are_overdue)
		this->resize(parent_xpos, this->y(), bar_width + BarGroup::bar_xoffset, this->h());
	else
		this->resize(parent_xpos + BarGroup::bar_xoffset, this->y(), bar_width, this->h());
}


//static public
int Bar::calc_height(const int height_with_yspacing) noexcept{
	return height_with_yspacing * bar_height_ratio_with_spacing; 
}

int Bar::calc_height(const int bar_group_height, const int task_count) noexcept{
	const float height_per_bar = float(bar_group_height) / float(task_count);	
	return height_per_bar * bar_height_ratio_with_spacing;
}

int Bar::calc_height_with_yspacing(const int bar_group_height, const int task_count) noexcept{
	const float height_per_bar = float(bar_group_height) / float(task_count);		
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

void Bar::left_mouse_click_callback(){
	try{
		if(this->is_single_task())
			((BarGroup*)(this->parent()))->request_window_for_editing_task(this);
		else{
			((BarGroup*)(this->parent()))->display_tasks_in_task_group(this);
		}
	}
	catch(const std::bad_alloc& alloc_err){
		fl_alert("Caught memory allocation error while requesting window for editing TaskGroup. (Bar::bar_callback())");
	}
	catch(const std::length_error& exceeded_max_alloc){
		fl_alert("Exceeded maximum memory allocation while requesting window for editing TaskGroup. (Bar::bar_callback())");		
	}	
	catch(const std::exception& unspecified_excp){
		const std::string msg = std::string("Caught unspecified exception while requesting window for editing TaskGroup. (Bar::bar_callback())\n")
								+ unspecified_excp.what();
	}
	catch(...){
		fl_alert("Caught unspecified throw while requesting window for editing TaskGroup. (Bar::bar_callback())");				
	}	
}

void Bar::right_mouse_click_callback(){
	try{
		if(!(this->is_single_task()))
			((BarGroup*)(this->parent()))->request_window_for_editing_group(this);
	}
	catch(const std::bad_alloc& alloc_err){
		fl_alert("Caught memory allocation error while requesting window for editing TaskGroup. (Bar::bar_callback())");
	}
	catch(const std::length_error& exceeded_max_alloc){
		fl_alert("Exceeded maximum memory allocation while requesting window for editing TaskGroup. (Bar::bar_callback())");		
	}	
	catch(const std::exception& unspecified_excp){
		const std::string msg = std::string("Caught unspecified exception while requesting window for editing TaskGroup. (Bar::bar_callback())\n")
								+ unspecified_excp.what();
	}
	catch(...){
		fl_alert("Caught unspecified throw while requesting window for editing TaskGroup. (Bar::bar_callback())");				
	}		
}

int Bar::handle(const int event){
	constexpr int handled_event = 1;

	switch(event){
		case FL_PUSH:{
			return handled_event;
		}
		case FL_RELEASE:{
			const bool clicked_and_released_same_button = Fl::event_inside(this) != 0;
			const int clicked_button = Fl::event_button();
			if(clicked_and_released_same_button){
				if(clicked_button == FL_LEFT_MOUSE)
					left_mouse_click_callback();
				if(clicked_button == FL_RIGHT_MOUSE)
					right_mouse_click_callback();
			}else{
				//the user dragged the mouse to other widget and releases the mouse button hold, pass this up.				
				((BarGroup*)(this->parent()))->handle_drag_event(this);
				//this widget may be deleted after the handle_drag_event, so no further access to the object.
			}
			//return is fine, it's on stack, not accessing the object.
			return handled_event;
		}		
		case FL_DRAG:{
			return handled_event;
		}		
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

void Bar::draw(){
	const bool use_overdue_position = this->taskgroup.nearest_due_date_task().days_remaining().count() < 1;
	const bool all_tasks_are_overdue = this->taskgroup.furthest_due_date_task().days_remaining().count() < 1;
	if(use_overdue_position && all_tasks_are_overdue){
		fl_draw_box(FL_FLAT_BOX, this->x(), this->y(), BarGroup::bar_xoffset, this->h(), get_bar_color(this->taskgroup.tasks[0].days_remaining().count()));
	}	
	else if(use_overdue_position && !all_tasks_are_overdue){
		for(std::int_least64_t i = this->taskgroup.tasks.size()-1; i >= 0; i--){
			const Task& task = this->taskgroup.tasks[i];
			const float of_days_from_interval = std::clamp(float(task.days_remaining().count()) / float(this->days_from_interval.count()), 0.0f, 1.0f);
			const float width = float(BarGroup::bar_max_width) * of_days_from_interval;
			fl_draw_box(FL_FLAT_BOX, this->x(), this->y(), width + BarGroup::bar_xoffset, this->h(), get_bar_color(task.days_remaining().count()));
		}
	}
	else{
		for(std::int_least64_t i = this->taskgroup.tasks.size()-1; i >= 0; i--){
			const Task& task = this->taskgroup.tasks[i];
			const float of_days_from_interval = std::clamp(float(task.days_remaining().count()) / float(this->days_from_interval.count()), 0.0f, 1.0f);
			const float width = float(BarGroup::bar_max_width) * of_days_from_interval;
			fl_draw_box(FL_FLAT_BOX, this->x(), this->y(), width, this->h(), get_bar_color(task.days_remaining().count()));
		}
	}
	Fl_Button::draw();
}


BarConstructorArgs::BarConstructorArgs(const BarGroup* const parent, const TaskGroup& taskgroup, 
										const int task_count, const int item_index)
:	taskgroup(taskgroup)
{
	const bool use_overdue_position = this->taskgroup.nearest_due_date_task().days_remaining().count() < 1;
	
	if(use_overdue_position){
		xpos = parent->x();
		if(this->taskgroup.furthest_due_date_task().days_remaining().count() > 0){
			width = BarGroup::bar_xoffset + Bar::calc_bar_width(this->taskgroup.furthest_due_date_task().days_remaining(), parent->get_days_from_interval());
		}else{
			width = BarGroup::bar_xoffset;
		}
	}else{
		xpos = parent->x() + BarGroup::bar_xoffset;
		width = Bar::calc_bar_width(this->taskgroup.furthest_due_date_task().days_remaining(), parent->get_days_from_interval());
	}
	
	const int bar_height_with_yspacing = Bar::calc_height_with_yspacing(parent->h(), task_count);
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

void Bar_TaskGroup::set_task_at(const std::size_t index, const Task& other){
	this->tasks[index] = other;
	this->sort_and_reassign();
}

void Bar_TaskGroup::delete_task_at(const std::size_t index){
	const bool is_nearest_due_date_task = index == 0;
	const bool is_furthest_due_date_task = index == this->tasks.size();
	
	if(is_nearest_due_date_task)
		this->_nearest_due_date_task = this->tasks[1];
	if(is_furthest_due_date_task)
		this->_nearest_due_date_task = this->tasks[this->tasks.size() - 1];
	
	this->tasks.erase(this->tasks.begin() + index);
}