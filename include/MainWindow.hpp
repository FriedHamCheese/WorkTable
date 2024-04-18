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

class MainWindow : public Fl_Window{
	public:
	MainWindow(const int xpos, const int ypos, const int width, const int height, const char* window_title = 0);
	
	void add_task(const Task& task);
	bool delete_task(const int item_index);
	void modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index);
	void modify_taskgroup_name(const char* const group_name, const int item_index);
	
	void show_window_for_creating_new_task();
	void show_window_for_editing_task(const Task& task_properties, const int item_index);
	void show_window_for_editing_group(const std::string& group_name, const int item_index);
	
	void show_root_group_box();
	
	void show_taskgroups();
	
	void save_tasks_to_file();
	void revert_to_tasks_from_file();

	void zoomin_timescale();
	void zoomout_timescale();	
	
	void enable_taskgroup_button();

	static void new_task_button_callback(Fl_Widget* const self_ptr, void* const data);
	static void save_button_callback(Fl_Widget* const self_ptr, void* const data);
	static void discard_button_callback(Fl_Widget* const self_ptr, void* const data);
	static void taskgroup_button_callback(Fl_Widget* const self_ptr, void* const data);
	
	static void zoomin_button_callback(Fl_Widget* const self_ptr, void* const data);
	static void zoomout_button_callback(Fl_Widget* const self_ptr, void* const data);
	
	static constexpr int width = 1600;
	static constexpr int height = 900;
	
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
	void hide_root_group_box() {this->root_group_box.hide();}
	bool mouse_released_in_root_group_box() const {return Fl::event_inside(&(this->root_group_box));}
	
};

#endif