#include "TaskGroupWindow.hpp"
#include "MainWindow.hpp"

#include "Task.hpp"
#include "align.hpp"
#include "task_io.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>

#include <FL/fl_ask.h>

#include <string>
#include <stdexcept>

TaskGroupWindow::TaskGroupWindow(const int width, const int height, MainWindow* const main_window)
:	Fl_Window(width, height, "Group"),
	task_name_label(0, 2, width, 30, "Group name:"
	),
	task_name_dialog(0, ypos_below(task_name_label), 
					width - 5, input_dialog_height
	),
	delete_button(5, ypos_below(task_name_dialog) + 5, 
					button_width - 5, button_height, "Delete"
	),
	save_button(xpos_right_of(delete_button), delete_button.y(), 
				button_width - 5, button_height, "Save"
	),
	main_window(main_window),
	modifying_item_index(-1)
{
	this->end();
	this->color(fl_rgb_color(240, 240, 240));
	this->save_button.callback(TaskGroupWindow::save_button_callback);
	this->delete_button.callback(TaskGroupWindow::delete_button_callback);
	
	this->task_name_label.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	this->task_name_dialog.box(FL_FLAT_BOX);

	this->delete_button.color(fl_rgb_color(220, 220, 220));
	this->save_button.color(fl_rgb_color(220, 220, 220));		
	
	this->delete_button.box(FL_FLAT_BOX);
	this->save_button.box(FL_FLAT_BOX);
}

void TaskGroupWindow::store_task(const std::string& group_name, const int item_index){
	this->copy_label(group_name.c_str());
	this->task_name_dialog.value(group_name.c_str());
	this->modifying_item_index = item_index;
}


void TaskGroupWindow::save_button_pressed(){
	this->modify_task();
}

void TaskGroupWindow::delete_button_pressed(){
	this->delete_task();
	this->hide();	
}


//static public
void TaskGroupWindow::save_button_callback(Fl_Widget* const self, void* const data){
	((TaskGroupWindow*)(self->parent()))->save_button_pressed();
}

void TaskGroupWindow::delete_button_callback(Fl_Widget* const self, void* const data){
	((TaskGroupWindow*)(self->parent()))->delete_button_pressed();
}
 

void TaskGroupWindow::delete_task(){
	if(!main_window->delete_task(modifying_item_index)){
		fl_alert("Failed to delete task."
				"\nTaskGroupWindow::delete_task(): Invalid deletion index.");
	}
}

void TaskGroupWindow::modify_task(){
	this->main_window->modify_taskgroup_name(task_name_dialog.value(), modifying_item_index);
	this->hide();
}