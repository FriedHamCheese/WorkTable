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

#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

BarGroup::BarGroup(const int xpos, const int ypos, const int width, const int height)
:	Fl_Group(xpos, ypos, width, height),
	task_group_id(this->not_in_any_group),			
	current_date_label(xpos_center_by_point(this->date_label_width, this->current_date_line_xpos()), 
						ypos_below(*this) - this->date_label_yraise, 
						this->date_label_width, this->date_label_height
	),
	interval_date_label(xpos_center_by_point(this->date_label_width, this->next_interval_date_line_xpos()), 
						ypos_below(*this) - this->date_label_yraise, 
						this->date_label_width, this->date_label_height
	),
	current_timescale(timescale::default_timescale),
	current_ymd(get_current_ymd()),	
	next_interval(get_next_interval(this->current_ymd, this->current_timescale)),
	unsaved_changes_made_to_tasks(false)
{
	this->end();
	
	this->color(FL_WHITE);
	this->box(FL_FLAT_BOX);	
	
	this->interval_date_label.copy_label(ymd_to_string(this->next_interval).c_str());
	this->interval_date_label.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);	
	
	this->current_date_label.copy_label(ymd_to_string(this->current_ymd).c_str());
	this->current_date_label.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
		
	try{
		this->load_tasks_to_bars();
	}
	catch(const std::bad_alloc& alloc_err) {
		fl_alert("Memory allocation error while loading tasks. (BarGroup::BarGroup(): std::bad_alloc)"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");		
	}
	catch(const std::length_error& exceeded_max_alloc) {
		fl_alert("Memory allocation error while loading tasks. (BarGroup::BarGroup(): std::length_error)"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");
	}	
	catch(const std::ios_base::failure& file_io_error) {
		fl_alert("I/O error while loading tasks from tasks.txt. (BarGroup::BarGroup())"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");
	}
	catch(const std::runtime_error& file_not_opened) {
		fl_alert("Task list file unable to be opened. (BarGroup::BarGroup())"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");		
	}
	catch(const std::exception& unspecified_excp){
		const std::string msg = std::string("Caught an unspecified exception while loading tasks. (BarGroup::BarGroup())")
								+ '\n' + unspecified_excp.what()
								+ "\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.";
		fl_alert(msg.c_str());
	}
	catch(...){
		fl_alert("Caught unspecified throw while loading tasks from file. (BarGroup::BarGroup())"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");
	}	
}

BarGroup::~BarGroup() noexcept{
	if(this->unsaved_changes_made_to_tasks){
		try{
			const int save_unsaved_changes = 1;
			const int user_option = fl_choice("There were unsaved changes made to task list. Would you like to save the changes?", "No", "Yes", 0);
		
			if(user_option == save_unsaved_changes){
				save_tasks_to_file();
				fl_alert("Tasks saved successfully.");
			}
		}
		catch(const std::bad_alloc& alloc_err){
			fl_alert("Not enough memory to save tasks to file. (BarGroup::~BarGroup(): std::bad_alloc)");
		}
		catch(const std::length_error& length_error){
			fl_alert("Not enough memory to save tasks to file. (BarGroup::~BarGroup(): std::length_error)");			
		}
		catch(const std::exception& unspecified_excp){
			const std::string msg = std::string("Caught an unspecified exception while saving tasks to file.")
									+ " (BarGroup::~BarGroup(): " + std::string(unspecified_excp.what()) + ")";
			fl_alert(msg.c_str());
		}
		catch(...){
			fl_alert("Caught an unspecified throw while saving tasks to file. (BarGroup::~BarGroup())");			
		}
	}
}


void BarGroup::add_task(const Task& task){
	this->add_bar(task);
	if(this->displaying_a_taskgroup()){
		this->paged_taskgroups[this->task_group_id].tasks.push_back(task);
	}
	
	this->unsaved_changes_made_to_tasks = true;
	this->redraw();
}

bool BarGroup::delete_bar(const int item_index){	
	if(this->bars.size() <= std::abs(item_index))
		return false;
	
	if(this->displaying_a_taskgroup()){
		if(this->paged_taskgroups[this->task_group_id].tasks.size() <= std::abs(item_index))
			return false;
	}
	
	this->remove(this->bars[item_index].get());
	
	if(this->displaying_a_taskgroup()){
		this->paged_taskgroups[this->task_group_id].tasks.erase(this->paged_taskgroups[this->task_group_id].tasks.begin() + item_index);
	}
	
	this->bars.erase(this->bars.begin() + item_index);
	this->unsaved_changes_made_to_tasks = true;
	
	this->adjust_vertical_layout();
	this->redraw();
	return true;
}

void BarGroup::modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index){
	const bool out_of_bounds_index = (item_index < 0) || (std::llabs(item_index) >= this->bars.size());
	if(out_of_bounds_index)
		throw std::invalid_argument("BarGroup::modify_task(): invalid task index.");
	bars[item_index]->update_task(task_name, due_date, this->get_days_from_interval(), this->x());		
	
	if(this->displaying_a_taskgroup()){
		this->paged_taskgroups[task_group_id].tasks[item_index] = bars[item_index]->get_single_task();
	}

	this->unsaved_changes_made_to_tasks = true;
	this->redraw();
}

bool BarGroup::modify_group(const char* const group_name, const int item_index){
	const bool out_of_bounds_index = (item_index < 0) || (std::llabs(item_index) >= this->bars.size());
	if(out_of_bounds_index) return false;
	
	bars[item_index]->update_group_name(group_name);

	this->unsaved_changes_made_to_tasks = true;
	this->redraw();	
	return true;
}


bool BarGroup::request_window_for_editing_task(const Bar* const bar){
	const int_least64_t item_index = this->get_item_index(bar);
	const bool invalid_item = item_index < 0;
	if(invalid_item) return false;
	
	((MainWindow*)(this->parent()))->show_window_for_editing_task(bar->get_single_task(), item_index);
	return true;
}

bool BarGroup::request_window_for_editing_group(const Bar* const bar){
	const int_least64_t item_index = this->get_item_index(bar);
	const bool invalid_item = item_index < 0;
	if(invalid_item) return false;
	
	((MainWindow*)(this->parent()))->show_window_for_editing_group(bar->get_taskgroup().group_name, item_index);
	return true;
}


void BarGroup::display_tasks_in_task_group(const Bar* const bar){
	std::int_least64_t bar_index = this->get_item_index(bar);
	if(bar_index == -1)
		throw std::invalid_argument("Invalid task index passed to BarGroup::display_tasks_in_task_group().");
	
	this->task_group_id = bar_index;
	
	this->paged_taskgroups.clear();
	this->paged_taskgroups.reserve(this->bars.size());
	
	for(const std::unique_ptr<Bar>& bar : this->bars){
		this->paged_taskgroups.push_back(bar->get_taskgroup());
		this->remove(bar.get());
	}
	
	this->bars.clear();
	
	const std::size_t task_count = this->paged_taskgroups[bar_index].tasks.size();
	for(std::size_t i = 0; i < task_count; ++i){
		const Task task = this->paged_taskgroups[bar_index].tasks[i];
		this->add_bar({task.name(), {task}}, task_count, i);
	}
	
	((MainWindow*)(this->parent()))->enable_taskgroup_button();
	this->redraw();
}

void BarGroup::show_taskgroups(){
	if(!(this->displaying_a_taskgroup())) return;
	this->bars.clear();
	
	const std::size_t taskgroup_count = this->paged_taskgroups.size();
	std::vector<TaskGroup> non_empty_taskgroups; 
	non_empty_taskgroups.reserve(taskgroup_count);
	
	for(const TaskGroup& taskgroup : this->paged_taskgroups){
		const bool taskgroup_not_empty = taskgroup.tasks.size() != 0;
		if(taskgroup_not_empty) 
			non_empty_taskgroups.push_back(taskgroup);
	}
	
	const std::size_t non_empty_taskgroup_count = non_empty_taskgroups.size();
	for(std::size_t i = 0; i < non_empty_taskgroup_count; i++){
		this->add_bar(non_empty_taskgroups[i], non_empty_taskgroup_count, i);
	}

	this->paged_taskgroups.clear();
	this->task_group_id = not_in_any_group;
	
	this->redraw();
}


void BarGroup::save_tasks_to_file(){
	std::vector<TaskGroup> taskgroups;
	
	if(this->task_group_id != not_in_any_group){
		taskgroups = this->paged_taskgroups;
	}else{
		taskgroups.reserve(this->bars.size());
		for(const std::unique_ptr<Bar>& bar : this->bars)
			taskgroups.emplace_back(bar->get_taskgroup());	
	}
	
	overwrite_taskfile(taskgroups);
	this->unsaved_changes_made_to_tasks = false;
}

void BarGroup::revert_to_tasks_from_file(){
	std::vector<TaskGroup> task_groups;
	
	try{
		task_groups = get_tasks();
	}
	catch(const std::ios_base::failure& file_io_error) {throw;}
	catch(const std::runtime_error& file_not_opened) {throw;}	
	
	for(std::unique_ptr<Bar>& bar_uniptr : this->bars){
		this->remove(bar_uniptr.get());
		bar_uniptr.release();
	}
	this->bars.clear();
	
	const auto taskgroup_count = task_groups.size();
	
	for(std::size_t i = 0; i < taskgroup_count; ++i)
		this->add_bar(task_groups[i], taskgroup_count, i);	

	this->unsaved_changes_made_to_tasks = false;
}


void BarGroup::handle_bar_mouse_button_release(const Bar* const clicked_bar){
	this->signal_hide_root_group_box();	
	
	const bool move_task_to_rootgroup = this->check_mouse_released_in_root_group_box() && this->displaying_a_taskgroup();
	if(move_task_to_rootgroup){
		this->paged_taskgroups.emplace_back(clicked_bar->get_taskgroup());
		const int clicked_bar_index  = this->get_item_index(clicked_bar);
		this->delete_bar(clicked_bar_index);
		this->redraw();
		return;
	}
	
	const bool possible_taskgroup_merge = !(this->displaying_a_taskgroup());
	if(possible_taskgroup_merge){
		for(const std::unique_ptr<Bar>& bar_at_rootgroup : bars)
		{
			const bool mouse_button_released_in_taskgroup = Fl::event_inside(bar_at_rootgroup.get());
			if(mouse_button_released_in_taskgroup){
				bar_at_rootgroup->merge_taskgroup(clicked_bar->get_taskgroup());
				this->delete_bar(this->get_item_index(clicked_bar));
				this->redraw();
				break;
			}
		}
	}
}

void BarGroup::signal_bar_being_dragged(){
	if(this->displaying_a_taskgroup())
		((MainWindow*)(this->parent()))->show_root_group_box();
}

void BarGroup::signal_hide_root_group_box() {
	((MainWindow*)(this->parent()))->hide_root_group_box();
}


std::chrono::days BarGroup::get_days_from_interval() const{
	return delta_days(next_interval, current_ymd);
}

//layout related code
int BarGroup::current_date_label_xpos() const{
	return current_date_label.x();
}
int BarGroup::xpos_right_of_interval_date_label() const{
	return xpos_right_of(interval_date_label);
}


bool BarGroup::has_unsaved_changes_to_tasks() const{
	return this->unsaved_changes_made_to_tasks;
}


Timescale BarGroup::zoomin_timescale(){
	const Timescale new_timescale = timescale::zoomin_timescale(this->current_timescale);	
	return this->change_timescale(new_timescale);
}

Timescale BarGroup::zoomout_timescale(){
	const Timescale new_timescale = timescale::zoomout_timescale(this->current_timescale);	
	return this->change_timescale(new_timescale);
}



//protected:
void BarGroup::draw(){	
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
void BarGroup::load_tasks_to_bars(){
	std::vector<TaskGroup> task_groups;
	
	try{
		task_groups = get_tasks();
	}
	catch(const std::ios_base::failure& file_io_error) {throw;}
	catch(const std::runtime_error& file_not_opened) {throw;}

	const auto task_count = task_groups.size();	
	for(decltype(task_groups.size()) i = 0; i < task_count; ++i)
		this->add_bar(task_groups[i], task_count, i);
}


void BarGroup::add_bar(const TaskGroup& taskgroup, const int total_items, const int item_index){
	Bar* bar;
	try{
		bar = new Bar(BarConstructorArgs(this, taskgroup, total_items, item_index));
		this->bars.emplace_back(bar);		
	}
	catch(const std::bad_alloc& alloc_err) {throw alloc_err;}
	catch(const std::length_error& exceeded_max_alloc) {throw exceeded_max_alloc;}
	
	this->add(bar);
}

void BarGroup::add_bar(const Task& task){
	//kinda verbose, but without these lines, it would be pretty confusing.
	const auto incremented_bar_count = bars.size() + 1;
	const auto incremented_item_index = incremented_bar_count - 1;
	
	try{
		this->add_bar({task.name(), {task}}, incremented_bar_count, incremented_item_index);
	}
	catch(const std::bad_alloc& alloc_err) {throw alloc_err;}
	catch(const std::length_error& exceeded_max_alloc) {throw exceeded_max_alloc;}

	this->adjust_vertical_layout();
}


int_least64_t BarGroup::get_item_index(const Bar* const bar) const{
	const auto bar_count = this->bars.size();
	
	for(decltype(this->bars.size()) i = 0; i < bar_count; ++i){
		if (bar == this->bars[i].get()) return i;
	}
	return -1;
}


void BarGroup::adjust_vertical_layout(){
	const std::size_t bar_count = this->bars.size();
	
	const int bar_height = Bar::calc_height(this->h(), bar_count);
	const int bar_height_with_yspacing = Bar::calc_height_with_yspacing(this->h(), bar_count);
	
	for(std::size_t i = 0; i < bar_count; ++i){
		const std::unique_ptr<Bar>& bar = this->bars[i];
		
		const int bar_ypos = Bar::calc_ypos(this->y(), bar_height_with_yspacing, i);
		bar->resize(bar->x(), bar_ypos, bar->w(), bar_height);
	}
}

Timescale BarGroup::change_timescale(const Timescale timescale){
	if(timescale == current_timescale) return current_timescale;
	
	try{
		this->next_interval = get_next_interval(this->current_ymd, timescale);
		this->interval_date_label.copy_label(ymd_to_string(this->next_interval).c_str());
	}
	catch(const std::invalid_argument& invalid_timescale){
		const std::string msg = std::string("Invalid timescale range (") + std::to_string(timescale::timescale_to_int(timescale)) + ") "
								+ std::string("passed to get_next_interval() (BarGroup::change_timescale())");
		fl_alert(msg.c_str());
		return current_timescale;
	}
	
	this->current_timescale = timescale;
	
	for(std::unique_ptr<Bar>& bar : bars)
		bar->update_width(get_days_from_interval(), this->x());
	
	this->redraw();
	return timescale;
}


int BarGroup::current_date_line_xpos() const{
	return this->x() + this->bar_xoffset;
}

int BarGroup::next_interval_date_line_xpos() const{
	return this->x() +this->bar_max_width + this->bar_xoffset;
}

bool BarGroup::check_mouse_released_in_root_group_box(){
	return ((MainWindow*)(this->parent()))->mouse_released_in_root_group_box();
}