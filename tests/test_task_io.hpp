#ifndef test_task_io_hpp
#define test_task_io_hpp

#include "task_io.hpp"

#include <vector>
#include <cassert>
#include <cstring>
#include <iostream>

namespace test_task_io_internal{
	static int _test_nested_group_warning_count = 0;
	constexpr static int _test_expected_nested_group_warning_count = 2;
	
	inline void _test_nested_group_callback(const char* const callsite_filename, const int callsite_line, 
										const std::string& first_taskgroup_name, const std::string& second_taskgroup_name)
	{_test_nested_group_warning_count++;}
	
	
	inline void test_get_raw_file__non_existent_file(){
		try{
			task_io_internal::get_raw_file("ejoijedijdoij.mpeg");
		}catch(const std::runtime_error& cant_open_file){
			return;
		}
		
		const bool cant_open_file = true;
		assert(!cant_open_file);
	}
	
	inline void test_get_raw_file(){
		constexpr const char* const expected_result = "abcdefg\nhijk";
		
		const task_io_internal::file_buffer file_buffer = task_io_internal::get_raw_file("./tests/get_raw_file_test.txt");
		
		assert(strlen(expected_result) == file_buffer.second);
		assert(strncmp(expected_result, file_buffer.first.get(), file_buffer.second) == 0);
	}
	
	inline void test_buffer_to_separated_lines(){
		constexpr const char* const buffer_input = "h\n\n\n\ni, im ok!\n\nyep";
		std::unique_ptr<char[]> unique_ptr_input(new char[strlen(buffer_input) + 1]);
		strcpy(unique_ptr_input.get(), buffer_input);
		
		const std::vector<std::string> expected_result = {
			"h",
			"i, im ok!",
			"yep"
		};
		
		const std::vector<std::string> lines = task_io_internal::buffer_to_separated_lines({std::move(unique_ptr_input), strlen(buffer_input)});
		assert(lines == expected_result);
	}
	
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
		assert(TaskStrGroup_vector == expected_result);
		assert(_test_nested_group_warning_count == 2);
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
		
		assert(taskgroups == expected_result);
	}
}


inline void test_suite_task_io(){
	test_task_io_internal::test_get_raw_file__non_existent_file();
	test_task_io_internal::test_get_raw_file();
	test_task_io_internal::test_buffer_to_separated_lines();
	
	test_task_io_internal::test_lines_to_TaskStrGroup();
	test_task_io_internal::test_TaskStrGroups_to_TaskGroups();
}

#endif