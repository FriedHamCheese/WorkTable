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
	virtual ~BarGroup() noexcept;
	
	void load_tasks_to_bars();
	
	void add_task(const Task& task);
	bool delete_task(const int item_index);
	bool modify_task(const char* const task_name, const std::chrono::year_month_day& due_date, const int item_index);

	bool request_window_for_editing_task(const Bar* const bar) const noexcept;
	
	std::chrono::days get_days_from_interval() const noexcept;
	
	//layout specific code
	int current_date_label_xpos() const noexcept;
	int xpos_right_of_interval_date_label() const noexcept;

	Timescale zoomin_timescale() noexcept;
	Timescale zoomout_timescale() noexcept;
	
	static constexpr int width = 1400;
	static constexpr int height = 700;
	static constexpr int bar_xoffset = 50;
	static constexpr int bar_max_width = width - bar_xoffset;
	
	protected:
	void draw() noexcept override;
		
	private:
	void add_bar(const Task& task, const int total_items, const int item_index);
	void add_bar(const Task& task);
	
	int_least64_t get_item_index(const Bar* const bar) const noexcept;
	
	void adjust_vertical_layout() noexcept;
	Timescale change_timescale(const Timescale timescale) noexcept;
		
	int current_date_line_xpos() const noexcept;
	int next_interval_date_line_xpos() const noexcept;
	
	std::vector<std::unique_ptr<Bar>> bars;
	Fl_Box current_date_label;
	Fl_Box interval_date_label;
	
	const std::chrono::year_month_day current_ymd;
	
	Timescale current_timescale;
	std::chrono::year_month_day next_interval;	
	
	static bool bar_due_date_is_earlier(const std::unique_ptr<Bar>& lhs, const std::unique_ptr<Bar>& rhs) noexcept;
	
	static constexpr int date_label_yraise = 20;
	static constexpr int date_label_width = 70;
	static constexpr int date_label_height = 15;
};

#endif