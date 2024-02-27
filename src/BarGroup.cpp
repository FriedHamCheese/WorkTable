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
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include <FL/fl_draw.H>
#include <FL/fl_ask.H>

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>
#include <algorithm>

BarGroup::BarGroup(const int xpos, const int ypos, const int width, const int height)
:	Fl_Group(xpos, ypos, width, height),
	current_date(xpos + 30, ypos_below(this) - 20, 50, 10),
	next_interval_date(xpos + bar_max_width - 25, ypos_below(this) - 20, 50, 10),
	current_ymd(get_current_ymd()),
	current_timescale(timescale::default_timescale),
	next_interval(get_next_interval(current_ymd, current_timescale))
{
	this->color(FL_WHITE);
	this->box(FL_FLAT_BOX);	
	
	this->next_interval_date.copy_label(ymd_to_string(this->next_interval).c_str());
	this->next_interval_date.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);	
	
	this->current_date.copy_label(ymd_to_string(this->current_ymd).c_str());
	this->current_date.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
	
	this->end();
}

BarGroup::~BarGroup() noexcept{
	std::sort(this->bars.begin(), this->bars.end(), BarGroup::due_date_is_earlier);
	std::vector<Task> tasks;
	tasks.reserve(this->bars.size());
	
	for(const std::unique_ptr<Bar>& bar : this->bars)
		tasks.emplace_back(bar->get_task_properties());
	
	overwrite_taskfile(tasks);
	std::clog << "Written to task file." << std::endl;
}

void BarGroup::load_tasks(){
	std::vector<Task> tasks = get_tasks();
	const auto task_count = tasks.size();
	std::sort(tasks.begin(), tasks.end(), Task::due_date_is_earlier);
	
	for(decltype(tasks.size()) i = 0; i < task_count; ++i)
		this->add_bar(tasks[i], task_count, i);
}

void BarGroup::add_task(const Task& task){
	this->add_bar(task);
	this->redraw();
}

void BarGroup::delete_task(const int item_index){
	try{
		this->remove(this->bars.at(item_index).get());
		this->bars.erase(this->bars.begin() + item_index);

		this->adjust_vertical_layout();
		this->redraw();
	}
	catch(const std::out_of_range& excp){
		throw std::invalid_argument(std::string("BarGroup.cpp: delete_task(): invalid item index for deletion") 
									+ " (" + std::to_string(item_index) + " )\n");		
	}
}

void BarGroup::modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index){
	try{
		bars.at(item_index)->update_task(task_name, due_date, this->get_days_from_interval());
		bars.at(item_index)->redraw();		
	}
	catch(const std::out_of_range& excp){
		throw std::invalid_argument(std::string("BarGroup.cpp: modify_task(): invalid item index for editing") 
									+ " (" + std::to_string(item_index) + " )\n");		
	}
}

auto BarGroup::get_item_index(const Bar* const bar) const{
	const auto bar_count = this->bars.size();
	
	for(decltype(this->bars.size()) i = 0; i < bar_count; ++i){
		if (bar == this->bars[i].get()) return i;
	}
	
	throw std::invalid_argument("Pointer passed to BarGroup::get_item_index does not exist in BarGroup::bars.");
}

std::chrono::days BarGroup::get_days_from_interval() const noexcept{
	return delta_days(next_interval, current_ymd);
}


void BarGroup::show_window_for_editing_task(const Bar* const bar) const{
	try{
		((MainWindow*)(this->parent()))->show_window_for_editing_task(bar->get_task_properties(), this->get_item_index(bar));
	}
	catch(const std::invalid_argument& item_index_not_found){
		const std::string alert_message = std::string(item_index_not_found.what())
										+ "\nProgram can still continue as normal.";
		fl_alert(alert_message.c_str());
	}
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
void BarGroup::draw(){
	Fl_Group::draw();
	
	const int left_line_xpos = this->x() + this->bar_xoffset;
	const int right_line_xpos = this->x() +this->bar_max_width;
	
	fl_line(left_line_xpos,
			this->y(), 
			left_line_xpos, 
			ypos_below(this) - 30
	);
	fl_line(right_line_xpos, 
			this->y(), 
			right_line_xpos,
			ypos_below(this) - 30
	);
}


//private:
void BarGroup::add_bar(const Task& task, const int total_items, const int item_index){
	auto bar = new Bar(BarConstructorArgs(this, task, total_items, item_index));
	this->bars.emplace_back(bar);
	this->add(bar);
}

void BarGroup::add_bar(const Task& task){
	//kinda verbose, but without these lines, it would be pretty confusing.
	const auto incremented_bar_count = bars.size() + 1;
	const auto incremented_item_index = incremented_bar_count - 1;
	
	this->add_bar(task, incremented_bar_count, incremented_item_index);
	this->adjust_vertical_layout();
}

void BarGroup::adjust_vertical_layout(){
	const auto bar_count = this->bars.size();
	
	const int ypos = this->y();
	const int bar_height = Bar::calc_height(this->h(), bar_count);
	const int bar_height_with_yspacing = Bar::calc_height_with_yspacing(this->h(), bar_count);
	
	for(decltype(this->bars.size()) i = 0; i < bar_count; ++i){
		const std::unique_ptr<Bar>& bar = this->bars[i];
		const int bar_ypos = Bar::calc_ypos(ypos, bar_height_with_yspacing, i);
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

bool BarGroup::due_date_is_earlier(const std::unique_ptr<Bar>& lhs, const std::unique_ptr<Bar>& rhs){
	return Bar::due_date_is_earlier(lhs.get(), rhs.get());
}