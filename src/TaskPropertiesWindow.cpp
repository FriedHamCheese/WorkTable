#include "TaskPropertiesWindow.hpp"
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

TaskPropertiesWindow::TaskPropertiesWindow(const int width, const int height, MainWindow* const main_window)
:	Fl_Window(width, height, "Task"),
	task_name_label(0, 2, width, 30, "Task name:"
	),
	task_name_dialog(0, ypos_below(task_name_label), 
					width - 5, input_dialog_height
	),
	due_date_label(0, ypos_below(task_name_dialog) + 5, 
					width, 30, "Due date:"
	),	
	due_date_dialog(0, ypos_below(due_date_label), 
					width - 5, input_dialog_height
	),
	warning_message(5, ypos_below(due_date_dialog),
					width - 5, 25
	),
	delete_button(5, ypos_below(warning_message), 
					button_width - 5, button_height, "Discard"
	),
	save_button(xpos_right_of(delete_button), delete_button.y(), 
				button_width - 5, button_height, "Create"
	),
	main_window(main_window),
	modifying_item_index(-1),
	current_mode(TaskPropertiesWindow::Mode::UnInit)
{
	this->end();
	this->color(fl_rgb_color(240, 240, 240));
	this->save_button.callback(TaskPropertiesWindow::save_button_callback);
	this->delete_button.callback(TaskPropertiesWindow::delete_button_callback);
	
	this->task_name_label.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	this->due_date_label.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	
	this->task_name_dialog.box(FL_FLAT_BOX);
	this->due_date_dialog.box(FL_FLAT_BOX);	

	this->warning_message.labelsize(12);

	this->delete_button.color(fl_rgb_color(220, 220, 220));
	this->save_button.color(fl_rgb_color(220, 220, 220));		
	
	this->delete_button.box(FL_FLAT_BOX);
	this->save_button.box(FL_FLAT_BOX);
	
	
	this->warning_message.box(FL_NO_BOX);
	this->warning_message.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
}

void TaskPropertiesWindow::set_window_for_creating_task(){
	const char* const empty_dialog = nullptr;
	this->task_name_dialog.value(empty_dialog);
	this->due_date_dialog.value(empty_dialog);
	this->label("Create a new task.");
	this->delete_button.label("Cancel");
	this->save_button.label("Create");	
	this->warning_message.label("");	
	
	this->current_mode = TaskPropertiesWindow::Mode::CreateNewTask;
}

void TaskPropertiesWindow::store_task(const Task& task, const int item_index){
	this->copy_label(task.name().c_str());
	this->task_name_dialog.value(task.name().c_str());
	this->due_date_dialog.value(ymd_to_string(task.due_date()).c_str());
	this->modifying_item_index = item_index;
	this->current_mode = TaskPropertiesWindow::Mode::EditTask;
	this->delete_button.label("Delete");
	this->save_button.label("Save");	
}


void TaskPropertiesWindow::save_button_pressed(){
	switch(current_mode){
		case TaskPropertiesWindow::Mode::CreateNewTask:
		this->add_task();
		break;

		case TaskPropertiesWindow::Mode::EditTask:
		this->modify_task();
		break;
		
		default:{
			const std::string msg = "TaskPropertiesWindow::save_button_pressed(): Mode case " + std::to_string(int(this->current_mode)) + " was not handled in switch.";
			fl_alert(msg.c_str());
		}
	}
}

void TaskPropertiesWindow::delete_button_pressed(){
	//only delete if the task exists and is being edited, not freshly created and not existing
	if(this->current_mode == TaskPropertiesWindow::Mode::EditTask) this->delete_task();
	
	//hide the window, regardless of mode
	this->hide();	
}


//static public
void TaskPropertiesWindow::save_button_callback(Fl_Widget* const self, void* const data){
	((TaskPropertiesWindow*)(self->parent()))->save_button_pressed();
}

void TaskPropertiesWindow::delete_button_callback(Fl_Widget* const self, void* const data){
	((TaskPropertiesWindow*)(self->parent()))->delete_button_pressed();
}
 

//private
void TaskPropertiesWindow::add_task(){
	try{
		this->warning_message.label("");	
		
		const bool task_has_name = strcmp(this->task_name_dialog.value(), "") != 0;
		if(!task_has_name){
			this->warning_message.label("Please name the task.");
			return;
		}
		
		try{
			const std::chrono::year_month_day new_due_date = str_to_ymd(std::string(this->due_date_dialog.value()));
		
			this->main_window->add_task(Task(new_due_date, std::string(task_name_dialog.value())));
			this->hide();
		}
		catch(const std::invalid_argument& invalid_ymd_str){
			this->warning_message.copy_label(invalid_ymd_str.what());
		}
	}
	catch(const std::exception& excp){
		const std::string msg = std::string("TaskPropertiesWindow::add_task(): an exception was thrown while adding task.")
								+ "\nException message: " + excp.what();
	}
	catch(...){
		fl_alert("TaskPropertiesWindow::add_task(): caught an unspecified throw while adding task.");		
	}
}

void TaskPropertiesWindow::delete_task(){
	try{
		if(!main_window->delete_bar(modifying_item_index)){
			fl_alert("Failed to delete task."
					"\nTaskPropertiesWindow::delete_task(): Invalid deletion index.");
		}
	}
	catch(const std::exception& excp){
		const std::string msg = std::string("TaskPropertiesWindow::delete_task(): an exception was thrown while deleting task.")
								+ "\nException message: " + excp.what();
		fl_alert(msg.c_str());
	}
	catch(...){
		fl_alert("TaskPropertiesWindow::delete_task(): caught an unspecified throw while deleting task.");
	}
}

void TaskPropertiesWindow::modify_task(){
	try{
		this->warning_message.label("");
		
		const bool task_name_is_empty = strlen(this->task_name_dialog.value()) <= 0;
		if(task_name_is_empty){
			this->warning_message.label("Please name the task.");
			return;
		}
		
		std::chrono::year_month_day new_due_date;
		try{
			new_due_date = str_to_ymd(std::string(due_date_dialog.value()));
		}
		catch(const std::invalid_argument& invalid_ymd_str){
			this->warning_message.copy_label(invalid_ymd_str.what());
			return;	
		}

		try{
			this->main_window->modify_task(task_name_dialog.value(), new_due_date, modifying_item_index);
			this->hide();
		}
		catch(const std::invalid_argument& invalid_index){
			fl_alert("Failed to save changes."
					"\nTaskPropertiesWindow::modify_task(): Invalid task index.");
		}
	}
	catch(const std::exception& excp){
			const std::string msg = std::string("TaskPropertiesWindow::modify_task(): an exception was thrown while editing task.")
									+ "\nException message: " + excp.what();
			fl_alert(msg.c_str());
	}
	catch(...){
		fl_alert("TaskPropertiesWindow::modify_task(): caught an unspecified throw while editing task.");
	}
}