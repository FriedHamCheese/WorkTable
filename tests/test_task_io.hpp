#ifndef test_task_io_hpp
#define test_task_io_hpp

#include "task_io.hpp"

#include <vector>
#include <cassert>
#include <iostream>

namespace test_task_io_internal{
	static int _test_nested_group_warning_count = 0;
	constexpr static int _test_expected_nested_group_warning_count = 2;
	
	inline void _test_nested_group_callback(const char* const callsite_filename, const int callsite_line, 
										const std::string& first_taskgroup_name, const std::string& second_taskgroup_name)
	{_test_nested_group_warning_count++;}	
	
	
	inline void test_lines_to_TaskStrGroup(){
		const std::vector<task_io_internal::TaskStrGroup> expected_result{
			{"", {{"2025/05/03", "cookies"}}},
			{"", {{"2025/05/02", "waffles"}}},
			{"group", {{"2025/05/05", "coffee"}, {"2025/06/07", "tea"}}},
			{"", {{"2025/05/01", "fries"}}},
		};
		
		const task_io_internal::file_buffer file_buffer = task_io_internal::get_raw_file("./tests/nested_grouping_test.txt");
		const std::vector<std::string> lines = task_io_internal::buffer_to_separated_lines(file_buffer);
		const std::vector<task_io_internal::TaskStrGroup> TaskStrGroup_vector = task_io_internal::lines_to_TaskStrGroup(lines, _test_nested_group_callback);
		assert(_test_nested_group_warning_count == _test_expected_nested_group_warning_count);
		
		const size_t TaskStrGroup_count = TaskStrGroup_vector.size();
		for(std::size_t taskstr_group_i = 0; taskstr_group_i < TaskStrGroup_count; taskstr_group_i++){
			const task_io_internal::TaskStrGroup& expected_taskstr_group = expected_result[taskstr_group_i];
			const task_io_internal::TaskStrGroup& current_taskstr_group = TaskStrGroup_vector[taskstr_group_i];
			
			assert(expected_taskstr_group.group_name == current_taskstr_group.group_name);
			
			const size_t taskstr_count = current_taskstr_group.taskstrs.size();
			
			for(std::size_t taskstr_i = 0; taskstr_i < taskstr_count; taskstr_i++){
				assert(expected_taskstr_group.taskstrs[taskstr_i].due_date == current_taskstr_group.taskstrs[taskstr_i].due_date);
				assert(expected_taskstr_group.taskstrs[taskstr_i].name == current_taskstr_group.taskstrs[taskstr_i].name);					
			}
		}
	}
	
	inline void test_TaskStrGroups_to_TaskGroups(){
		const std::vector<TaskGroup> expected_result{
			{"", {{std::chrono::year_month_day(std::chrono::year(2025), std::chrono::May, std::chrono::day(3)), "cookies"}}},
			{"", {{std::chrono::year_month_day(std::chrono::year(2025), std::chrono::May, std::chrono::day(2)), "waffles"}}},
			{"group", {{std::chrono::year_month_day(std::chrono::year(2025), std::chrono::May, std::chrono::day(5)), "coffee"}, {std::chrono::year_month_day(std::chrono::year(2025), std::chrono::June, std::chrono::day(7)), "tea"}}},
			{"", {{std::chrono::year_month_day(std::chrono::year(2025), std::chrono::May, std::chrono::day(1)), "fries"}}},
		};
		
		const task_io_internal::file_buffer file_buffer = task_io_internal::get_raw_file("./tests/grouping_test.txt");
		const std::vector<std::string> lines = task_io_internal::buffer_to_separated_lines(file_buffer);
		const std::vector<task_io_internal::TaskStrGroup> TaskStrGroup_vector = task_io_internal::lines_to_TaskStrGroup(lines);
		const std::vector<TaskGroup> taskgroups = task_io_internal::TaskStrGroups_to_TaskGroups(TaskStrGroup_vector);
		
		const std::size_t taskgroup_count = taskgroups.size();
		assert(taskgroup_count == expected_result.size());
		
		for(std::size_t taskgroup_i = 0; taskgroup_i < taskgroup_count; taskgroup_i++){
			const TaskGroup& expected_taskgroup = expected_result[taskgroup_i];
			const TaskGroup& current_taskgroup = taskgroups[taskgroup_i];
			
			assert(expected_taskgroup.group_name == current_taskgroup.group_name);
			const std::size_t task_count = current_taskgroup.tasks.size();
			assert(expected_taskgroup.tasks.size() == task_count);
			
			for(std::size_t task_i = 0; task_i < task_count; task_i++){
				assert(expected_taskgroup.tasks[task_i].name() == current_taskgroup.tasks[task_i].name());
				assert(expected_taskgroup.tasks[task_i].due_date() == current_taskgroup.tasks[task_i].due_date());
				assert(expected_taskgroup.tasks[task_i].days_remaining() == current_taskgroup.tasks[task_i].days_remaining());				
			}
		}
	}
}


inline void test_suite_task_io(){
	test_task_io_internal::test_lines_to_TaskStrGroup();
	test_task_io_internal::test_TaskStrGroups_to_TaskGroups();
}

#endif