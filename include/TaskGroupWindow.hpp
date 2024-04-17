#ifndef taskgroupwindow_hpp
#define taskgroupwindow_hpp

#include "Task.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>

class MainWindow;

class TaskGroupWindow : public Fl_Window{
	public:
	TaskGroupWindow(const int width, const int height, MainWindow* const main_window);

	void store_task(const std::string& group_name, const int item_index);
	
	void save_button_pressed();
	void delete_button_pressed();
	
	static void save_button_callback(Fl_Widget* const self, void* const data);
	static void delete_button_callback(Fl_Widget* const self, void* const data);

	static constexpr int width = 300;
	static constexpr int height = 100;

	private:
	void delete_task();	
	void modify_task();
	
	Fl_Box task_name_label;
	Fl_Input task_name_dialog;
	Fl_Button delete_button;
	Fl_Button save_button;
	
	//conventionally this will be obtained by calling Fl_Widget::parent(), 
	//but this window cannot be a child of another window because we need this to be a separate window
	//but we still need a way for interfacing with the main window, hence the pointer
	MainWindow* const main_window;
	
	int modifying_item_index;
		
	static constexpr int input_dialog_width = 150;
	static constexpr int input_dialog_height = 30;
	
	static constexpr int button_width = width / 2;
	static constexpr int button_height = 35;	
};

#endif