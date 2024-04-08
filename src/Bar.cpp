#include "Bar.hpp"
#include "BarGroup.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>

#include <FL/fl_draw.h>
#include <FL/fl_ask.h>

#include <string>
#include <iostream>

Bar::Bar(const BarConstructorArgs& args)
:	Bar(args.xpos, args.ypos, args.width, args.height, args.task_group)
{
}

Bar::Bar(const int xpos, const int ypos, const int width, const int height, const Bar_TaskGroup& task_group)
:	Fl_Button(xpos, ypos, width, height),
	task_group(task_group)
{
	this->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);			
	this->update_color_from_days_remaining();
	this->box(FL_NO_BOX);
	this->callback(Bar::bar_callback);
	
	this->update_label();
}

void Bar::update_task(const char* const task_name, const std::chrono::year_month_day& due_date, const std::chrono::days& days_from_interval, const int parent_xpos){
	this->task_group.group_name = task_name;
	this->update_label();
	
	this->update_width(days_from_interval, parent_xpos);	
	this->update_color_from_days_remaining();	
}

void Bar::update_width(const std::chrono::days& days_from_interval, const int parent_xpos){
	/*
	const bool move_to_overdue_position = this->task_group.days_remaining().count() < 1;
	
	if(!move_to_overdue_position){
		const int bar_width = Bar::calc_bar_width(this->task_group.days_remaining(), days_from_interval);
		this->resize(parent_xpos + BarGroup::bar_xoffset, this->y(), bar_width, this->h());
	}else{
		this->resize(parent_xpos, this->y(), BarGroup::bar_xoffset, this->h());
	}
	*/
}

std::chrono::year_month_day Bar::get_due_date() const{
	//return task_group.due_date();
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

void Bar::bar_callback(Fl_Widget* const self, void* const data){
	try{
		((BarGroup*)(self->parent()))->request_window_for_editing_task((Bar*)(self));
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

bool Bar::due_date_is_earlier(const Bar* const lhs, const Bar* const rhs) noexcept{
	return lhs->get_due_date() < rhs->get_due_date();
}


//private
void Bar::update_label(){
	std::string label;
		//[Task name] ([days] days)
	if(this->is_single_task()) 
		label = task_group.tasks[0].name() + " (" + std::to_string(task_group.tasks[0].days_remaining().count()) + " days)";
	else
		//[TaskGroup name] ([days] - [days] days)		
		label = task_group.group_name + " (" + std::to_string(task_group.nearest_due_date_task().days_remaining().count()) + "-" + std::to_string(task_group.furthest_due_date_task().days_remaining().count()) + " days)";
	
	this->copy_label(label.c_str());		
}

void Bar::update_color_from_days_remaining() noexcept{
	this->redraw();
}

void Bar::draw(){
	TaskGroup sorted_task_group = this->task_group;
	std::sort(sorted_task_group.tasks.begin(), sorted_task_group.tasks.end(), Task::due_date_is_later);
	
	for(const Task& task : sorted_task_group.tasks){
		const float of_furthest_due_date = float(task.days_remaining().count()) / float(this->task_group.furthest_due_date_task().days_remaining().count());
		const float width = float(this->w()) * of_furthest_due_date;
		fl_draw_box(FL_FLAT_BOX, this->x(), this->y(), width, this->h(), get_bar_color(task.days_remaining().count()));
	}
	Fl_Button::draw();
}


BarConstructorArgs::BarConstructorArgs(const BarGroup* const parent, const TaskGroup& task_group, 
										const int task_count, const int item_index)
:	task_group(task_group)
{
	const bool use_overdue_position = this->task_group.furthest_due_date_task().days_remaining().count() < 1;
	
	if(use_overdue_position){
		xpos = parent->x();
		width = BarGroup::bar_xoffset;
	}else{
		xpos = parent->x() + BarGroup::bar_xoffset;
		width = Bar::calc_bar_width(this->task_group.furthest_due_date_task().days_remaining(), parent->get_days_from_interval());
	}
	
	const int bar_height_with_yspacing = Bar::calc_height_with_yspacing(parent->h(), task_count);
	ypos = Bar::calc_ypos(parent->y(), bar_height_with_yspacing, item_index);
	
	height = Bar::calc_height(bar_height_with_yspacing);
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


Bar_TaskGroup::Bar_TaskGroup(const TaskGroup& task_group)
:	TaskGroup(task_group)
{
	Task nearest_due_date_task = this->tasks[0];
	Task furthest_due_date_task = this->tasks[0];
	
	for(const Task& task : this->tasks){		
		if(task.due_date() > furthest_due_date_task.due_date()) 
			furthest_due_date_task = task;
		if(task.due_date() < nearest_due_date_task.due_date()) 
			nearest_due_date_task = task;
	}
		
	this->_nearest_due_date_task = nearest_due_date_task;
	this->_furthest_due_date_task = furthest_due_date_task;
}