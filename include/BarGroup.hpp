#ifndef bargroup_hpp
#define bargroup_hpp


#include "Bar.hpp"
#include "Task.hpp"
#include "Timescale.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include <vector>
#include <memory>

class BarGroup : public Fl_Group{
	public:
	BarGroup(const int xpos, const int ypos, const int width, const int height);
	virtual ~BarGroup() noexcept override;
	
	void load_tasks();
	
	void add_task(const Task& task);
	void delete_task(const int item_index);
	void modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index);

	void show_window_for_editing_task(const Bar* const bar) const;
	auto get_item_index(const Bar* const bar) const;
	std::chrono::days get_days_from_interval() const noexcept;
	
	int current_date_label_xpos() const;
	int xpos_right_of_next_interval_label() const;

	Timescale zoomin_timescale() noexcept;
	Timescale zoomout_timescale() noexcept;
	
	static constexpr int xoffset = 50;
	static constexpr int yoffset = 100;
	static constexpr int width = 1400;
	static constexpr int height = 700;
	static constexpr int bar_xoffset = 50;
	static constexpr int bar_max_width = width - bar_xoffset;
	
	protected:
	void draw() override;
		
	private:
	void add_bar(const Task& task, const int total_items, const int item_index);
	void add_bar(const Task& task);
	void adjust_vertical_layout();
	Timescale change_timescale(const Timescale timescale) noexcept;
	
	std::vector<std::unique_ptr<Bar>> bars;
	Fl_Box current_date;
	Fl_Box next_interval_date;
	
	const std::chrono::year_month_day current_ymd;
	
	Timescale current_timescale;
	std::chrono::year_month_day next_interval;	
	
	static bool due_date_is_earlier(const std::unique_ptr<Bar>& lhs, const std::unique_ptr<Bar>& rhs);
	
	static constexpr int interval_label_yraise = 20;
	static constexpr int interval_label_width = 70;
	static constexpr int interval_label_height = 15;
	
};

#endif