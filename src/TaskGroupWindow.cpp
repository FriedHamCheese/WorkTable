/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

#include "TaskGroupWindow.hpp"
#include "MainWindow.hpp"

#include "align.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>

#include <FL/fl_ask.h>

#include <string>
#include <exception>

TaskGroupWindow::TaskGroupWindow(const int width, const int height, MainWindow* const main_window)
:	Fl_Window(width, height, "Group"),
	task_name_label(0, 2, width, 30, "Group name:"
	),
	task_name_dialog(5, ypos_below(task_name_label), 
					width - 10, 30
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

void TaskGroupWindow::store_group(const std::string& group_name, const int item_index){
	this->copy_label(group_name.c_str());
	this->task_name_dialog.value(group_name.c_str());
	this->modifying_item_index = item_index;
}


void TaskGroupWindow::save_button_pressed(){
	this->modify_group_name();
}

void TaskGroupWindow::delete_button_pressed(){
	this->delete_group();
	this->hide();	
}


//static public
void TaskGroupWindow::save_button_callback(Fl_Widget* const self, void* const data){
	((TaskGroupWindow*)(self->parent()))->save_button_pressed();
}

void TaskGroupWindow::delete_button_callback(Fl_Widget* const self, void* const data){
	((TaskGroupWindow*)(self->parent()))->delete_button_pressed();
}
 

void TaskGroupWindow::delete_group(){
	try{
		if(!main_window->delete_bar(modifying_item_index)){
			fl_alert("Failed to delete task."
					"\nTaskGroupWindow::delete_group(): Invalid deletion index.");
		}
	}catch(const std::exception& excp){
		const std::string msg = std::string("TaskGroupWindow::delete_group(): an exception was thrown while deleting task.")
								+ "\nException message: " + excp.what();
		fl_alert(msg.c_str());
	}catch(...){
		fl_alert("TaskGroupWindow::delete_group(): caught an unspecified throw while deleting task.");
	}
}

void TaskGroupWindow::modify_group_name(){
	try{
		if(!(this->main_window->modify_taskgroup_name(task_name_dialog.value(), modifying_item_index))){
			fl_alert("TaskGroupWindow::modify_group_name(): invalid item index.");
		}
	}
	catch(const std::exception& excp){
		const std::string msg = std::string("TaskGroupWindow::modify_group_name(): an exception was thrown while editing group name.")
								+ "\nException message: " + excp.what();
		fl_alert(msg.c_str());
	}
	catch(...){
		const std::string msg = "TaskGroupWindow::modify_group_name(): caught an unspecified throw while editing group name.";
		fl_alert(msg.c_str());
	}
	
	this->hide();
}