#include "BarGroup.hpp"

#include "Bar.hpp"
#include "MainWindow.hpp"

#include "Task.hpp"
#include "align.hpp"
#include "task_io.hpp"
#include "Timescale.hpp"
#include "time_calc.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>

#include <FL/fl_draw.H>
#include <FL/fl_ask.H>

#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

BarGroup::BarGroup(const int xpos, const int ypos, const int width, const int height)
:	Fl_Group(xpos, ypos, width, height),
	current_date_label(xpos_center_by_point(this->date_label_width, this->current_date_line_xpos()), 
						ypos_below(*this) - this->date_label_yraise, 
						this->date_label_width, this->date_label_height
	),
	interval_date_label(xpos_center_by_point(this->date_label_width, this->next_interval_date_line_xpos()), 
						ypos_below(*this) - this->date_label_yraise, 
						this->date_label_width, this->date_label_height
	),
	current_ymd(get_current_ymd()),
	current_timescale(timescale::default_timescale),
	next_interval(get_next_interval(this->current_ymd, this->current_timescale))
{
	this->end();
	
	this->color(FL_WHITE);
	this->box(FL_FLAT_BOX);	
	
	this->interval_date_label.copy_label(ymd_to_string(this->next_interval).c_str());
	this->interval_date_label.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);	
	
	this->current_date_label.copy_label(ymd_to_string(this->current_ymd).c_str());
	this->current_date_label.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
		
	try{this->load_tasks_to_bars();}
	catch(const std::bad_alloc& not_enough_temp_memory){throw not_enough_temp_memory;}
	catch(const std::exception& implementation_defined_excp){throw implementation_defined_excp;}
}

BarGroup::~BarGroup() noexcept{
	try{
		std::sort(this->bars.begin(), this->bars.end(), BarGroup::bar_due_date_is_earlier);
	}catch(const std::bad_alloc& not_enough_temp_memory){
		fl_alert("Not enough memory to save task. (BarGroup::~BarGroup(): not_enough_temp_memory)");
	}
	
	try{
		std::vector<Task> tasks;
		tasks.reserve(this->bars.size());
		
		for(const std::unique_ptr<Bar>& bar : this->bars)
			tasks.emplace_back(bar->get_task_properties());	
		
		overwrite_taskfile(tasks);
	}
	catch(const std::length_error& exceeded_max_alloc) {	
		fl_alert("Not enough memory to save task. (BarGroup::~BarGroup(): exceeded_max_alloc)");
	}
	catch(const std::exception& allocator_excp) {
		fl_alert("Not enough memory to save task. (BarGroup::~BarGroup(): allocator_excp)");
	}
}

void BarGroup::load_tasks_to_bars(){
	std::vector<Task> tasks;
	
	try{
		tasks = get_tasks();
		//yep, it's "implementation defined exception" on cppreference and allocator_traits::construct on cplusplus.
		//no certainty, so base exception is used here
	}catch(const std::exception& implementation_defined_excp) {throw implementation_defined_excp;}

	const auto task_count = tasks.size();
	
	try{
		std::sort(tasks.begin(), tasks.end(), Task::due_date_is_earlier);
	}catch(const std::bad_alloc& not_enough_temp_memory) {throw not_enough_temp_memory;}
	
	for(decltype(tasks.size()) i = 0; i < task_count; ++i)
		this->add_bar(tasks[i], task_count, i);
}


void BarGroup::add_task(const Task& task){
	try{
		this->add_bar(task);
	}catch(const std::bad_alloc& alloc_err){throw alloc_err;}

	this->redraw();
}

bool BarGroup::delete_task(const int item_index){
	try{
		this->remove(this->bars.at(item_index).get());
	}catch(const std::out_of_range& invalid_item_index) {return false;}
	
	this->bars.erase(this->bars.begin() + item_index);
	this->adjust_vertical_layout();
	this->redraw();
	
	return true;
}

bool BarGroup::modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index){
	if(std::abs(item_index) >= bars.size()) return false;
	
	//can write a try-catch for update task
	bars[item_index]->update_task(task_name, due_date, this->get_days_from_interval());
	bars[item_index]->redraw();
	return true;
}


bool BarGroup::request_window_for_editing_task(const Bar* const bar) const noexcept{
	try{
		const int item_index = this->get_item_index(bar);
		((MainWindow*)(this->parent()))->show_window_for_editing_task(bar->get_task_properties(), item_index);
		return true;
	}
	catch(const std::invalid_argument& item_index_not_found){return false;}
}


std::chrono::days BarGroup::get_days_from_interval() const noexcept{
	return delta_days(next_interval, current_ymd);
}


//layout related code
int BarGroup::current_date_label_xpos() const noexcept{
	return current_date_label.x();
}
int BarGroup::xpos_right_of_interval_date_label() const noexcept{
	return xpos_right_of(interval_date_label);
}


Timescale BarGroup::zoomin_timescale() noexcept{
	const Timescale new_timescale = timescale::zoomin_timescale(this->current_timescale);	
	return change_timescale(new_timescale);
}

Timescale BarGroup::zoomout_timescale() noexcept{
	const Timescale new_timescale = timescale::zoomout_timescale(this->current_timescale);	
	return change_timescale(new_timescale);
}



//protected:
void BarGroup::draw() noexcept{
	Fl_Group::draw();
	
	const int left_line_xpos = this->current_date_line_xpos();
	const int right_line_xpos = this->next_interval_date_line_xpos();
	
	fl_line(left_line_xpos, this->y(), 
			left_line_xpos, ypos_below(*this) - 30
	);
	fl_line(right_line_xpos, this->y(), 
			right_line_xpos, ypos_below(*this) - 30
	);
}



//private:
void BarGroup::add_bar(const Task& task, const int total_items, const int item_index){
	Bar* bar;
	try{
		bar = new Bar(BarConstructorArgs(this, task, total_items, item_index));
		this->bars.emplace_back(bar);		
	}catch(const std::exception& alloc_err) {
		throw new std::bad_alloc();
	}
	
	this->add(bar);
}

void BarGroup::add_bar(const Task& task){
	//kinda verbose, but without these lines, it would be pretty confusing.
	const auto incremented_bar_count = bars.size() + 1;
	const auto incremented_item_index = incremented_bar_count - 1;
	
	try{
		this->add_bar(task, incremented_bar_count, incremented_item_index);
	}catch(const std::bad_alloc& alloc_err) {throw alloc_err;}

	this->adjust_vertical_layout();
}


int_least64_t BarGroup::get_item_index(const Bar* const bar) const noexcept{
	const auto bar_count = this->bars.size();
	
	for(decltype(this->bars.size()) i = 0; i < bar_count; ++i){
		if (bar == this->bars[i].get()) return i;
	}
	return -1;
}


void BarGroup::adjust_vertical_layout() noexcept{
	const auto bar_count = this->bars.size();
	
	const int bar_height = Bar::calc_height(this->h(), bar_count);
	const int bar_height_with_yspacing = Bar::calc_height_with_yspacing(this->h(), bar_count);
	
	for(decltype(this->bars.size()) i = 0; i < bar_count; ++i){
		const std::unique_ptr<Bar>& bar = this->bars[i];
		
		const int bar_ypos = Bar::calc_ypos(this->y(), bar_height_with_yspacing, i);
		bar->resize(bar->x(), bar_ypos, bar->w(), bar_height);
	}
}

Timescale BarGroup::change_timescale(const Timescale timescale) noexcept{
	if(timescale == current_timescale) return current_timescale;
	
	this->current_timescale = timescale;
	this->next_interval = get_next_interval(this->current_ymd, timescale);
	
	for(std::unique_ptr<Bar>& bar : bars)
		bar->update_width(get_days_from_interval());
	
	return timescale;
}


int BarGroup::current_date_line_xpos() const noexcept{
	return this->x() + this->bar_xoffset;
}

int BarGroup::next_interval_date_line_xpos() const noexcept{
	return this->x() +this->bar_max_width;
}


bool BarGroup::bar_due_date_is_earlier(const std::unique_ptr<Bar>& lhs, const std::unique_ptr<Bar>& rhs) noexcept{
	return Bar::due_date_is_earlier(lhs.get(), rhs.get());
}