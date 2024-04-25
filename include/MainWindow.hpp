#ifndef mainwindow_hpp
#define mainwindow_hpp

#include "Task.hpp"
#include "BarGroup.hpp"

#include "TaskGroupWindow.hpp"
#include "TaskPropertiesWindow.hpp"

#include<FL/Fl.H>
#include<FL/Fl_Box.H>
#include<FL/Fl_Button.H>
#include<FL/Fl_Window.H>

/**
The main window of the program.
It contains most of the upfront buttons, timescale buttons and label, BarGroup, and owns TaskPropertiesWindow and TaskGroupWindow for manipulating tasks and groups respectively.
It also passes requests between the buttons, BarGroup, or the two windows.
*/
class MainWindow : public Fl_Window{
	public:
	MainWindow(const int xpos, const int ypos, const int width, const int height, const char* window_title = 0);
	
	///Passes add task requests from this->task_properties_window to this->bar_group.
	void add_task(const Task& task); 	
	///Passes deletion requests of a Bar at given index which represents a task or a group. 
	///Passes the request from this->task_properties_window or this->task_group_window to this->bar_group.	
	bool delete_bar(const int item_index); 
	///Passes task edit requests from this->task_properties_window to this->bar_group.
	///
	///May rethrow std::invalid_argument if the provided index refers to an invalid task.
	void modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index);
	///Passes task group name edit requests from this->task_group_window to this->bar_group.
	bool modify_taskgroup_name(const char* const group_name, const int item_index);
	
	///Passes requests from this->new_task_button to show this->task_properties_window for creating a new task.
	void show_window_for_creating_new_task();
	/**
	Passes requests from this->bar_group to show this->task_properties_window for editing the task.
	\param[in] task_properties The Task object which the Bar requesting the window represents.
	\param[in] item_index An index of the Bar in this->bar_group::bars which requested the window. 
	*/
	void show_window_for_editing_task(const Task& task_properties, const int item_index);
	/**
	Passes requests from this->bar_group to show this->task_group_window for editing the task group name.
	\param[in] group_name The name of the task group which the Bar requesting the window represents.
	\param[in] item_index An index of the Bar in this->bar_group::bars which requested the window. 
	*/	
	void show_window_for_editing_group(const std::string& group_name, const int item_index);
	
	///Shows this->root_group_box. This is called from this->bar_group when in group view and a Bar is being dragged.
	void show_root_group_box();
	
	///Passes requests from this->taskgroup_button to this->bar_group to return to root view. This deactivates this->task_group_button as well.
	///
	///This function catches throws and shows an error window if caught.
	void show_taskgroups();
	
	///Passes save requests from MainWindow::save_button_callback() to this->bar_group.
	///
	///This function catches throws and shows an error window if caught.
	void save_tasks_to_file();
	///Passes task revert requests from MainWindow::discard_button_callback() to this->bar_group.	
	///
	///This function catches throws and shows an error window if caught.	
	void revert_to_tasks_from_file();

	///Passes requests from MainWindow::zoomin_button_callback() to this->bar_group to narrow down (zoom in) the timescale. This redraws MainWindow.
	///
	///This function catches throws and shows an error window if caught.	
	void zoomin_timescale();
	///Passes requests from MainWindow::zoomout_button_callback() to this->bar_group to widen (zoom out) the timescale.	This redraws MainWindow.
	///
	///This function catches throws and shows an error window if caught.	
	void zoomout_timescale();	
	
	///Activates this->task_group_button when requested by this->bar_group.
	void enable_taskgroup_button();

	static void new_task_button_callback(Fl_Widget* const self_ptr, void* const data);
	static void save_button_callback(Fl_Widget* const self_ptr, void* const data);
	static void discard_button_callback(Fl_Widget* const self_ptr, void* const data);
	static void taskgroup_button_callback(Fl_Widget* const self_ptr, void* const data);
	
	static void zoomin_button_callback(Fl_Widget* const self_ptr, void* const data);
	static void zoomout_button_callback(Fl_Widget* const self_ptr, void* const data);
	
	static constexpr int width = 1600;	///< Value for width argument for the caller constructing this object.
	static constexpr int height = 900;	///< Value for height argument for the caller constructing this object.
	
	private:
	TaskPropertiesWindow task_properties_window;
	TaskGroupWindow taskgroup_window;
	
	BarGroup bar_group;
	Fl_Button new_task_button;
	Fl_Button save_button;
	Fl_Button discard_button;
	Fl_Button taskgroup_button;
	Fl_Button zoomout_button;
	Fl_Button zoomin_button;	
	Fl_Box timescale_text_box;
	
	Fl_Box root_group_box;
	
	static constexpr int button_width = 25;
	static constexpr int button_height = 25;
	static constexpr int timescale_text_box_width = 100;
	static constexpr int timescale_text_box_height = 25;
	
	public:
	///Hides this->root_group_box when requested by BarGroup.
	void hide_root_group_box() {this->root_group_box.hide();}
	///Returns true if a mouse drag was released in this->root_group_box.
	bool mouse_released_in_root_group_box() const {return Fl::event_inside(&(this->root_group_box));}
	
};

#endif