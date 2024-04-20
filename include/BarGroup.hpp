#ifndef bargroup_hpp
#define bargroup_hpp

#include "Bar.hpp"
#include "Task.hpp"
#include "Timescale.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>

#include <vector>
#include <memory>
#include <cstdint>

class BarGroup : public Fl_Group{
	public:
	BarGroup(const int xpos, const int ypos, const int width, const int height);
	virtual ~BarGroup();
		
	void add_task(const Task& task);
	bool delete_task(const int item_index);
	void modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index);
	void modify_group(const char* const task_name, const int item_index);

	bool request_window_for_editing_task(const Bar* const bar);
	bool request_window_for_editing_group(const Bar* const bar);
	
	void display_tasks_in_task_group(const Bar* const bar);
	void show_taskgroups();
	
	void save_tasks_to_file();
	void revert_to_tasks_from_file();
	
	void handle_bar_mouse_button_release(const Bar* const clicked_bar);
	void signal_bar_being_dragged();
	void signal_hide_root_group_box();
	
	std::chrono::days get_days_from_interval() const;
	
	//layout specific code
	int current_date_label_xpos() const;
	int xpos_right_of_interval_date_label() const;
	
	bool has_unsaved_changes_to_tasks() const;

	Timescale zoomin_timescale();
	Timescale zoomout_timescale();
	
	static constexpr int width = 1400;
	static constexpr int height = 700;
	static constexpr int bar_xoffset = 50;
	static constexpr int overdue_bar_width = bar_xoffset;
	static constexpr int bar_max_width = width - 2*bar_xoffset;
	
	protected:
	void draw() override;
		
	private:
	void load_tasks_to_bars();		
	
	void add_bar(const TaskGroup& taskgroup, const int total_items, const int item_index);
	void add_bar(const Task& task);
		
	int_least64_t get_item_index(const Bar* const bar) const;
	
	void adjust_vertical_layout();
	Timescale change_timescale(const Timescale timescale);
		
	int current_date_line_xpos() const;
	int next_interval_date_line_xpos() const;
	
	bool displaying_a_taskgroup() const{return this->task_group_id != this->not_in_any_group;}
	bool check_mouse_released_in_root_group_box();
	
	private:
	std::vector<std::unique_ptr<Bar>> bars;
	
	std::vector<TaskGroup> paged_taskgroups;
	std::int_least64_t task_group_id;
	static constexpr const std::int_least64_t not_in_any_group = -1;
	
	Fl_Box current_date_label;
	Fl_Box interval_date_label;
		
	Timescale current_timescale;
	
	const std::chrono::year_month_day current_ymd;	
	std::chrono::year_month_day next_interval;

	bool unsaved_changes_made_to_tasks;
		
	static constexpr int date_label_yraise = 20;
	static constexpr int date_label_width = 70;
	static constexpr int date_label_height = 15;
};

#endif