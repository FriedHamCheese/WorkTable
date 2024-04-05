#ifndef taskio_hpp
#define taskio_hpp

#include "Task.hpp"

#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <utility>
#include <fstream>
#include <stdexcept>

namespace task_io_internal{
	struct TaskStr{
		std::string due_date;
		std::string name;
		
		TaskStr(const std::string& due_date, const std::string& name);
	};
	
	struct TaskStrGroup{
		std::string group_name;
		std::vector<TaskStr> taskstrs;
	};
	
	using file_buffer = std::pair<std::unique_ptr<char[]>, std::size_t>;
	
	file_buffer get_raw_file(const std::string& filename);

	std::vector<std::string> buffer_to_separated_lines(const file_buffer& buffer);
	std::vector<TaskStrGroup> lines_to_TaskStrGroup(const std::vector<std::string>& lines);
	std::vector<TaskGroup> TaskStrGroups_to_TaskGroups(const std::vector<TaskStrGroup>& taskstr_groups);
}

std::vector<TaskGroup> get_tasks();

std::string int_to_2char(const unsigned num);
void overwrite_taskfile(const std::vector<Task>& tasks);

std::string task_to_str(const Task& task);

std::chrono::year_month_day str_to_ymd(const std::string& str);
std::string ymd_to_string(const std::chrono::year_month_day& ymd);

inline bool char_is_number(const char c){
	return (c >= '0') && (c <= '9');
}

#endif