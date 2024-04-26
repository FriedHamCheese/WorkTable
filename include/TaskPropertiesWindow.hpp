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

/**
A window for adding, editing or deleting a task.
This is a modal window, meaning it discards any interactions to other windows other than itself when it's shown.
*/
class TaskPropertiesWindow : public Fl_Window{
	public:
	TaskPropertiesWindow(const int width, const int height, MainWindow* const main_window);

	/**
	Prepare itself for creating a new task by: changing its window title, clearing out the input fields, 
	changing button labels, and sets this->current_mode to TaskPropertiesWindow::Mode::CreateNewTask.
	*/
	void set_window_for_creating_task();
	/**
	Prepare itself for modifying or deleting an existing task by: changing its window title, 
	loading the input fields with data from provided task argument, chaning button labels, 
	and sets this->current_mode to TaskPropertiesWindow::Mode::EditTask.
	\param[in] task A task which is represented by a Bar in BarGroup.
	\param[in] item_index An index of the Bar in BarGroup::bars requesting this window for editing or deleting its task.
	*/
	void store_task(const Task& task, const int item_index);
	
	/**
	This either calls this->add_task() or this->modify_task() depending if this->current_mode is TaskPropertiesWindow::Mode::CreateNewTask or TaskPropertiesWindow::Mode::EditTask respectively.
	
	If this->current_mode is not either, a warning window pops up.
	*/
	void save_button_pressed();
	/**
	This is called by TaskPropertiesWindow::delete_button_callback(). 
	It calls this->delete_task() if this->current_mode is TaskPropertiesWindow::Mode::EditTask, which indicates the task does exist, and can be deleted.
	It finally hides the window regardless of the mode. This simplifies this->delete_button being a cancel button and an actual delete button,
	where both button results in the window being closed.
	*/
	void delete_button_pressed();
	
	static void save_button_callback(Fl_Widget* const self, void* const data);
	static void delete_button_callback(Fl_Widget* const self, void* const data);

	static constexpr int width = 300;	///< Value for width argument for the caller constructing this object
	static constexpr int height = 190;	///< Value for height argument for the caller constructing this object

	private:
	/**
	Send a task add request to this->main_window after validating the task name and due date field.
	
	If this->modifying_item_index is invalid or a throw was caught, a warning window would pop up.
	*/
	void add_task();
	/**
	Send a task deletion request for the task at this->modifying_item_index to this->main_window.
	
	If this->modifying_item_index is invalid or a throw was caught, a warning window would pop up.
	*/
	void delete_task();	
	/**
	Sends a task edit request for the task at this->modifying_item_index to this->main_window after validating the task name and due date field.
	If either is invalid, warn with this->warning_message.
	If throw was caught, a warning window would pop up.	
	*/
	void modify_task();
	
	Fl_Box task_name_label;
	Fl_Input task_name_dialog;
	Fl_Box due_date_label;
	Fl_Input due_date_dialog;
	Fl_Box warning_message;
	Fl_Button delete_button;
	Fl_Button save_button;
	
	///conventionally this will be obtained by calling Fl_Widget::parent(), 
	///but this window cannot be a child of another window because we need this to be a separate window
	///but we still need a way for interfacing with the main window, hence the pointer
	MainWindow* const main_window;
	
	/**
	This stores the index of a bar in BarGroup::bars which represent a task. 
	A valid index will be provided by requests from this->main_window, which is provided by BarGroup.
	*/
	int modifying_item_index;
	
	enum class Mode{
		UnInit, CreateNewTask, EditTask
	};
	
	/**
	The window can either be in task creating mode or in editing mode.
	The behavior of the buttons change depending on what mode the window is in; along with the values in input fields and window title.
	*/
	Mode current_mode;
	
	static constexpr int input_dialog_width = 150;
	static constexpr int input_dialog_height = 30;
	
	static constexpr int button_width = width / 2;
	static constexpr int button_height = 35;	
};

#endif