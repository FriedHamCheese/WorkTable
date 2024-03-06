#include "Bar.hpp"
#include "BarGroup.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>

#include <string>

Bar::Bar(const BarConstructorArgs& args)
:	Bar(args.xpos, args.ypos, args.width, args.height, args.task_properties)
{
	
}

Bar::Bar(const int xpos, const int ypos, const int width, const int height, const Task& task_properties)
:	Fl_Button(xpos, ypos, width, height),
	task_properties(task_properties)
{
	this->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);			
	this->update_color_from_days_remaining();
	this->box(FL_FLAT_BOX);
	this->callback(Bar::bar_callback);
	
	this->update_label();
}

void Bar::update_task(const char* const task_name, const std::chrono::year_month_day& due_date, const std::chrono::days& days_from_interval, const int parent_xpos){
	this->task_properties.name(task_name);
	this->task_properties.due_date(due_date);
	this->update_label();
	
	this->update_width(days_from_interval, parent_xpos);	
	this->update_color_from_days_remaining();	
}

void Bar::update_width(const std::chrono::days& days_from_interval, const int parent_xpos){
	const bool move_to_overdue_position = this->task_properties.days_remaining().count() < 1;
	
	if(!move_to_overdue_position){
		const int bar_width = Bar::calc_bar_width(this->task_properties.days_remaining(), days_from_interval);
		this->resize(parent_xpos + BarGroup::bar_xoffset, this->y(), bar_width, this->h());
	}else{
		this->resize(parent_xpos, this->y(), BarGroup::bar_xoffset, this->h());
	}
}

std::chrono::year_month_day Bar::get_due_date() const{
	return task_properties.due_date();
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

void Bar::bar_callback(Fl_Widget* const self, void* const data) noexcept{
	((BarGroup*)(self->parent()))->request_window_for_editing_task((Bar*)(self));
}

bool Bar::due_date_is_earlier(const Bar* const lhs, const Bar* const rhs) noexcept{
	return lhs->get_due_date() < rhs->get_due_date();
}


//private
void Bar::update_label(){
	//[task name] ([days_remaining] days)
	const std::string bar_string = task_properties.name() + " (" + std::to_string(task_properties.days_remaining().count()) + " days)";
	this->copy_label(bar_string.c_str());	
}

void Bar::update_color_from_days_remaining() noexcept{
	this->color(get_bar_color(this->task_properties.days_remaining().count()));
}


BarConstructorArgs::BarConstructorArgs(const BarGroup* const parent, const Task& task_properties, 
										const int task_count, const int item_index)
:	task_properties(task_properties)
{
	const bool use_overdue_position = task_properties.days_remaining().count() < 1;
	if(use_overdue_position){
		xpos = parent->x();
		width = BarGroup::bar_xoffset;
	}else{
		xpos = parent->x() + BarGroup::bar_xoffset;
		width = Bar::calc_bar_width(task_properties.days_remaining(), parent->get_days_from_interval());
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