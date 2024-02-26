#ifndef taskpropertieswindow_hpp
#define taskpropertieswindow_hpp

#include "Task.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>

class MainWindow;

class TaskPropertiesWindow : public Fl_Window{
	public:
	TaskPropertiesWindow(const int width, const int height, MainWindow* const main_window);

	void set_window_for_creating_task();
	void store_task(const Task& task, const int item_index);
	
	void save_button_pressed();
	void delete_button_pressed();
	
	static void save_button_callback(Fl_Widget* const self, void* const data);
	static void delete_button_callback(Fl_Widget* const self, void* const data);

	static constexpr int width = 300;
	static constexpr int height = 190;

	private:
	void add_task();
	void delete_task();	
	void modify_task();
	
	Fl_Box task_name_label;
	Fl_Input task_name_dialog;
	Fl_Box due_date_label;
	Fl_Input due_date_dialog;
	Fl_Box warning_message;
	Fl_Button delete_button;
	Fl_Button save_button;
	
	//conventionally this will be obtained by calling Fl_Widget::parent(), 
	//but this window cannot be a child of another window because we need this to be a separate window
	//but we still need a way for interfacing with the main window, hence the pointer
	MainWindow* const main_window;
	
	int modifying_item_index;
	
	enum class Mode{
		UnInit, CreateNewTask, EditTask
	};
	
	Mode current_mode;
	
	static constexpr int input_dialog_width = 150;
	static constexpr int input_dialog_height = 30;
	
	static constexpr int button_width = width / 2;
	static constexpr int button_height = 35;	
};

#endif