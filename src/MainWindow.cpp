#include "MainWindow.hpp"
#include "BarGroup.hpp"
#include "TaskPropertiesWindow.hpp"

#include "align.hpp"
#include "Task.hpp"

#include<FL/Fl.H>
#include<FL/Fl_Box.H>
#include<FL/Fl_Button.H>
#include<FL/Fl_Window.H>

#include <FL/fl_ask.h>

MainWindow::MainWindow(const int xpos, const int ypos, const int width, const int height, const char* window_title)
:	Fl_Window(xpos, ypos, width, height, window_title),
	task_properties_window(TaskPropertiesWindow::width, TaskPropertiesWindow::height, this
	),
	bar_group(50, 100, BarGroup::width, BarGroup::height
	),
	new_task_button(bar_group.current_date_label_xpos(), ypos_below(bar_group), 
					MainWindow::button_width, MainWindow::button_height, "@filenew"
	),
	save_button(xpos_right_of(new_task_button) + 10, ypos_below(bar_group), 
				MainWindow::button_width, MainWindow::button_height, "@filesave"
	),	
	discard_button(xpos_right_of(save_button), ypos_below(bar_group), 
					MainWindow::button_width, MainWindow::button_height, "@undo"
	),
	zoomout_button(
		bar_group.xpos_right_of_interval_date_label() - MainWindow::button_width, ypos_below(bar_group),
		MainWindow::button_width, MainWindow::button_height, "-"
	),
	zoomin_button(
		zoomout_button.x() - button_width, ypos_below(bar_group), 
		MainWindow::button_width, MainWindow::button_height, "+"
	),	
	timescale_text_box(
		zoomin_button.x() - MainWindow::timescale_text_box_width - 10, 
		ypos_below(bar_group),
		MainWindow::timescale_text_box_width, 
		MainWindow::timescale_text_box_height
	)
{	
	this->new_task_button.labelcolor(FL_GRAY);	
	this->save_button.labelcolor(fl_rgb_color(48, 48, 48));	
	this->discard_button.labelcolor(fl_rgb_color(48, 48, 48));	
	
	this->timescale_text_box.align(FL_ALIGN_INSIDE | FL_ALIGN_RIGHT);
	this->timescale_text_box.label(timescale::get_timescale_str(timescale::default_timescale));

	this->new_task_button.callback(MainWindow::new_task_button_callback);	
	
	this->save_button.callback(MainWindow::save_button_callback);	
	this->discard_button.callback(MainWindow::discard_button_callback);	
	
	this->zoomin_button.callback(MainWindow::zoomin_button_callback);
	this->zoomout_button.callback(MainWindow::zoomout_button_callback);
	
	//this need some investigation why this requires manual end() and adds()
	this->end();
	this->add(this->bar_group);
	this->add(this->new_task_button);
	this->add(this->save_button);
	this->add(this->discard_button);	
	this->add(this->zoomin_button);
	this->add(this->zoomout_button);
	this->add(this->timescale_text_box);
	
	this->color(FL_WHITE);
}

//public
void MainWindow::add_task(const Task& task){
	try{
		this->bar_group.add_task(task);
	}
	catch(const std::bad_alloc& alloc_err) {throw alloc_err;}
	catch(const std::length_error& exceeded_max_alloc) {throw exceeded_max_alloc;}	
}

bool MainWindow::delete_task(const int item_index){
	return this->bar_group.delete_task(item_index);
}

void MainWindow::modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index){
	try{
		this->bar_group.modify_task(task_name, due_date, item_index);
	}
	catch(const std::bad_alloc& alloc_err) {throw alloc_err;}
	catch(const std::length_error& exceeded_max_alloc) {throw exceeded_max_alloc;}	
}


void MainWindow::show_window_for_creating_new_task(){
	this->task_properties_window.set_window_for_creating_task();
	this->task_properties_window.show();
}

void MainWindow::show_window_for_editing_task(const Task& task_properties, const int item_index){
	this->task_properties_window.store_task(task_properties, item_index);
	this->task_properties_window.show();
}


void MainWindow::save_tasks_to_file() noexcept{
	try{
		this->bar_group.save_tasks_to_file();
	}
	catch(const std::length_error& exceeded_max_alloc){
		fl_alert("Not enough memory to save tasks to file. (MainWindow::save_tasks_to_file(): std::length_error)"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");	
	}
	catch(const std::bad_alloc& alloc_err){
		fl_alert("Not enough memory to save tasks to file. (MainWindow::save_tasks_to_file(): std::bad_alloc)"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");			
	}
	catch(const std::exception& unspecified_excp){
		const std::string msg = std::string("Uncaught exception while saving tasks to file. (MainWindow::save_tasks_to_file())"
											"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.\n")
								+ std::string(unspecified_excp.what());
		fl_alert(msg.c_str());
	}
}

void MainWindow::revert_to_tasks_from_file() noexcept{
	const int user_decision_revert = 1;
	const int user_decision = fl_choice("Revert tasks back to task list in file?", "Cancel", "Revert", 0);
	if (user_decision != user_decision_revert) return;
	
	try{
		this->bar_group.revert_to_tasks_from_file();
	}
	catch(const std::bad_alloc& alloc_err) {
		fl_alert("Memory allocation error while reverting changes to tasks. (MainWindow::revert_to_tasks_from_file(): std::bad_alloc)"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");		
	}	
	catch(const std::length_error& exceeded_max_alloc) {
		fl_alert("Memory allocation error while reverting changes to tasks. (MainWindow::revert_to_tasks_from_file(): std::length_error)"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");
	}	
	catch(const std::ios_base::failure& file_io_error) {
		fl_alert("I/O error while reading task list to revert changes. (MainWindow::revert_to_tasks_from_file())"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");
	}
	catch(const std::runtime_error& file_not_opened) {
		fl_alert("Task list file unable to be opened. (MainWindow::revert_to_tasks_from_file())"
				"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.");		
	}
	catch(const std::exception& unspecified_excp){
		const std::string msg = std::string("Uncaught exception while reverting changes to tasks. (MainWindow::revert_to_tasks_from_file())"
											"\nIt is recommended to exit the program without saving in order to not overwrite task list with faulty data.\n")
								+ std::string(unspecified_excp.what());
		fl_alert(msg.c_str());
	}
}


void MainWindow::zoomin_timescale(){
	const Timescale new_timescale = this->bar_group.zoomin_timescale();	
	
	this->timescale_text_box.label(timescale::get_timescale_str(new_timescale));	
	this->redraw();
}

void MainWindow::zoomout_timescale(){
	const Timescale new_timescale = this->bar_group.zoomout_timescale();	
	
	this->timescale_text_box.label(timescale::get_timescale_str(new_timescale));
	this->redraw();	
}


//static public
void MainWindow::new_task_button_callback(Fl_Widget* const self_ptr, void* const data){
	((MainWindow*)(self_ptr->parent()))->show_window_for_creating_new_task();
}

void MainWindow::save_button_callback(Fl_Widget* const self_ptr, void* const data){
	((MainWindow*)(self_ptr->parent()))->save_tasks_to_file();
}

void MainWindow::discard_button_callback(Fl_Widget* const self_ptr, void* const data){
	((MainWindow*)(self_ptr->parent()))->revert_to_tasks_from_file();
}

void MainWindow::zoomin_button_callback(Fl_Widget* const self_ptr, void* const data){
	((MainWindow*)(self_ptr->parent()))->zoomin_timescale();
}

void MainWindow::zoomout_button_callback(Fl_Widget* const self_ptr, void* const data){
	((MainWindow*)(self_ptr->parent()))->zoomout_timescale();
}