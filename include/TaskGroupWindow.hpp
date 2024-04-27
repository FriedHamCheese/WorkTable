/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef taskgroupwindow_hpp
#define taskgroupwindow_hpp

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include <string>

class MainWindow;

/**
A window for editing and deleting task groups.
This is a modal window, meaning it discards any interactions to other windows other than itself when it's shown.
*/
class TaskGroupWindow : public Fl_Window{
	private:
	Fl_Box task_name_label;
	Fl_Input task_name_dialog;
	Fl_Button delete_button;
	Fl_Button save_button;
	
	///conventionally this will be obtained by calling Fl_Widget::parent(), 
	///but this window cannot be a child of another window because we need this to be a separate window
	///but we still need a way for interfacing with the main window, hence the pointer
	MainWindow* const main_window;
	
	/**
	This stores the index of the Bar in BarGroup::bars which requested the window to be shown.
	The index is provided when a request is passed from BarGroup.
	*/
	int modifying_item_index;
	
	public:
	TaskGroupWindow(const int width, const int height, MainWindow* const main_window);
	
	///Sets the window label as the provided group_name, and loads group_name into this->task_name_dialog.
	void store_group(const std::string& group_name, const int item_index);
	
	///Calls this->modify_task(). This is invoked from TaskGroupWindow::save_button_callback().
	void save_button_pressed();
	///Calls this->delete_task() and hides the window. This is invoked from TaskGroupWindow::delete_button_callback().
	void delete_button_pressed();
	
	static void save_button_callback(Fl_Widget* const self, void* const data);
	static void delete_button_callback(Fl_Widget* const self, void* const data);

	static constexpr int width = 300;	///< Value for width argument for the caller constructing this object
	static constexpr int height = 110;	///< Value for height argument for the caller constructing this object

	private:
	/**
	Requests this->main_window to delete a Bar at this->modifying_item_index which represents a task group.
	If the index is invalid or a throw was caught, a window would pop up notifying the error.
	*/
	void delete_group();	
	/**
	Requests this->main_window to edit the name of a Bar at this->modifying_item_index which represents a task group.
	If the group name or index is invalid or a throw was caught, a window would pop up notifying the error.	
	*/
	void modify_group_name();
	
	static constexpr int button_width = width / 2;
	static constexpr int button_height = 35;	
};

#endif