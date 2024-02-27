#include "MainWindow.hpp"
#include "BarGroup.hpp"
#include "TaskPropertiesWindow.hpp"

#include "align.hpp"
#include "Task.hpp"

#include<FL/Fl.H>
#include<FL/Fl_Box.H>
#include<FL/Fl_Button.H>
#include<FL/Fl_Window.H>
#include<FL/fl_ask.H>

MainWindow::MainWindow(const int xpos, const int ypos, const int width, const int height, const char* window_title)
:	Fl_Window(xpos, ypos, width, height, window_title),
	task_properties_window(TaskPropertiesWindow::width, TaskPropertiesWindow::height, this
	),
	bar_group(BarGroup::xoffset, BarGroup::yoffset, BarGroup::width, BarGroup::height
	),
	new_task_button(bar_group.current_date_label_xpos(), ypos_below(bar_group), 
					MainWindow::button_width, MainWindow::button_height, "@filenew"
	),
	zoomin_button(
		xpos_right_of(bar_group) - MainWindow::button_width, ypos_below(bar_group), 
		MainWindow::button_width, MainWindow::button_height, "+"
	),
	zoomout_button(
		zoomin_button.x() - button_width, ypos_below(bar_group),
		MainWindow::button_width, MainWindow::button_height, "-"
	),
	timescale_text_box(
		zoomout_button.x() - MainWindow::timescale_text_box_width - 10, 
		ypos_below(bar_group),
		MainWindow::timescale_text_box_width, 
		MainWindow::timescale_text_box_height
	)
{
	this->zoomout_button.position(bar_group.xpos_right_of_next_interval_label() - MainWindow::button_width, this->zoomout_button.y());
	this->zoomin_button.position(this->zoomout_button.x() - button_width, this->zoomin_button.y());
	this->timescale_text_box.position(this->zoomin_button.x() - timescale_text_box_width - 10, this->timescale_text_box.y());
	this->redraw();
	
	this->new_task_button.labelcolor(FL_GRAY);	
	
	this->timescale_text_box.align(FL_ALIGN_INSIDE | FL_ALIGN_RIGHT);
	this->timescale_text_box.label(timescale::get_timescale_str(timescale::default_timescale));

	this->new_task_button.callback(MainWindow::new_task_button_callback);	
	this->zoomin_button.callback(MainWindow::zoomin_button_callback);
	this->zoomout_button.callback(MainWindow::zoomout_button_callback);
	
	this->end();
	this->add(this->bar_group);
	this->add(this->new_task_button);
	this->add(this->zoomin_button);
	this->add(this->zoomout_button);
	this->add(this->timescale_text_box);
	
	this->color(FL_WHITE);
	
	try{
		this->bar_group.load_tasks();
	}catch(const std::runtime_error& excp){
		fl_alert(excp.what());
		std::terminate();
	}
}

//public
void MainWindow::add_task(const Task& task){
	this->bar_group.add_task(task);
}

void MainWindow::delete_task(const int item_index){
	this->bar_group.delete_task(item_index);
}

void MainWindow::modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index){
	this->bar_group.modify_task(task_name, due_date, item_index);
}


void MainWindow::show_window_for_creating_new_task(){
	this->task_properties_window.set_window_for_creating_task();
	this->task_properties_window.show();
}

void MainWindow::show_window_for_editing_task(const Task& task_properties, const int item_index){
	this->task_properties_window.store_task(task_properties, item_index);
	this->task_properties_window.show();
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

void MainWindow::zoomin_button_callback(Fl_Widget* const self_ptr, void* const data){
	((MainWindow*)(self_ptr->parent()))->zoomin_timescale();
}

void MainWindow::zoomout_button_callback(Fl_Widget* const self_ptr, void* const data){
	((MainWindow*)(self_ptr->parent()))->zoomout_timescale();
}