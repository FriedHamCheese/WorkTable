/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

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

/**
\file task_io.hpp
A module for reading and writing to task file;
as well as utilities for converting types related to Task to std::string equivalent.
*/

///\todo might just make it task_io and put everything in, or remove this entirely in 0.6
namespace task_io_internal{
	/**
	A string equivalent of Task. 
	This is used when fetching tasks from the task file and then be converted to a Task.
	*/
	struct TaskStr{
		std::string due_date;
		std::string name;
		
		TaskStr(const std::string& due_date, const std::string& name);
	};
	
	///Returns true if TaskStr::due_date and TaskStr::name of both are the same.
	inline bool operator==(const TaskStr& lhs, const TaskStr& rhs){
		return (lhs.due_date == rhs.due_date) && (lhs.name == rhs.name);
	}
	///Returns true if TaskStr::due_date or TaskStr::name of both are not the same.	
	inline bool operator!=(const TaskStr& lhs, const TaskStr& rhs){
		return !(lhs == rhs);
	}

	/**
	A string equivalent of TaskGroup.
	This is used when fetching tasks and groups from the task file and then be converted to a TaskGroup.	
	*/
	struct TaskStrGroup{
		std::string group_name;
		std::vector<TaskStr> taskstrs;
	};
	
	///Returns true if TaskStrGroup::group_name and TaskStrGroup::taskstrs of both are the same.
	inline bool operator==(const TaskStrGroup& lhs, const TaskStrGroup& rhs){
		return (lhs.group_name == rhs.group_name) && (lhs.taskstrs == rhs.taskstrs);
	}
	///Returns true if TaskStrGroup::group_name or TaskStrGroup::taskstrs of both are not the same.	
	inline bool operator!=(const TaskStrGroup& lhs, const TaskStrGroup& rhs){
		return !(lhs == rhs);
	}

	
	using file_buffer = std::pair<std::unique_ptr<char[]>, std::size_t>;
	
	///Reads entire contents of filename and returns {characters (no null terminator), character count}.
	///May throw std::ios_base_failure for unknown I/O error, or std::runtime_error if the file could not be opened.
	file_buffer get_raw_file(const std::string& filename);

	///The error callback for buffer_to_separated_lines. This gets called and pops up a window if a nested group is detected.
	///\todo make callsite_filename and callsite_line be a single std::string.
	void default_nested_group_callback(const char* const callsite_filename, const int callsite_line, 
										const std::string& first_taskgroup_name, const std::string& second_taskgroup_name);
	
	///Separates lines from the given buffer of characters. The newline character is the separator.
	///The return vector does not contain any line which is empty.
	std::vector<std::string> buffer_to_separated_lines(const file_buffer& buffer);
	
	/**
	Constructs a TaskStr from the provided line in "yyyy/(m)m/(d)d, task name" format.
	
	There are a few things that can go wrong and the behaviour of the function are as follows:	
	- A task with the due date in an improper format but still has a comma and space after it, would not be seen as an error. As this function only fetches the string and the TaskStr only keeps the copy of the string.
	- A task with the due date in valid format but has invalid due date would also not be an error.
	- A task without the comma after due date would have no name. And likely would be an invalid due date because the due date string would be fetched to the end of the line.
	- A task without the space after the comma would lose the first character of the task name.	
	*/
	TaskStr line_to_TaskStr(const std::string& line);
	
	/**
	Parses raw lines to TaskStrGroup, its default callback will pop up a window if an instance of nested group is present in task file. 
	
	There are a few things that can go wrong and the behaviour of the function are as follows:
	- The behaviours of line_to_TaskStr(const std::string& line) when fetching a string and converting it to a TaskStr.
	
	- A valid group without name would result in a valid group with no group name.
	- If the function was not fetching for tasks in a group and the line is a single }, the line means nothing.
	- A group without its scope ending (has no }) would result in every task after the group definition to be in the group.
	- The function does not care if the end scope character matches. Once it reaches one, the function ends the fetching of tasks of a group.
	- If the code already was fetching for tasks of a group and the current line defines another group, the code simply warns the nesting of a group, and moves on to fetch tasks to the former group.
	
	We chose to display a window rather than throwing an exception, simply because it is a small error.
	*/
	std::vector<TaskStrGroup> lines_to_TaskStrGroup(const std::vector<std::string>& lines, 
													void(*nested_group_callback)(const char*, const int, const std::string&, const std::string&) = default_nested_group_callback);
	///Converts multiple TaskStrGroup to TaskGroup. 
	///
	///May display a window if the due date of a task is invalid, and will continue on.
	std::vector<TaskGroup> TaskStrGroups_to_TaskGroups(const std::vector<TaskStrGroup>& taskstr_groups);
}

///The function that simplifies the entire task file reading and conversion process.
///
///May throw std::ios_base_failure for unknown I/O error, or std::runtime_error if the file could not be opened.
///And may display a window if the a group is a nested group, or a task has an invalid due date.
std::vector<TaskGroup> get_tasks(void(*nested_group_callback)(const char*, const int, const std::string&, const std::string&) = task_io_internal::default_nested_group_callback);

///Returns a 2 character string equivalent of the argument. So 0-9 would be 0x.
///If the argument has more than 2 digits, it returns a string equivalent with more than 2 characters.
std::string int_to_2char(const unsigned num);

///The function for saving the provided array of TaskGroup to task file.
///
///It may display a window if an error occurs while saving to task file, and will give the user a decision to either:
///ignore the anomaly or try writing to the file again.
void overwrite_taskfile(const std::vector<TaskGroup>& taskgroups);

///Converts a Task object to the format used in task file.
std::string task_to_str(const Task& task);

/**
Converts the string which is in yyyy/(m)m/(d)d format to std::chrono::year_month_day.
The digits in parenthesis are optional, for example 05 and 5 would still be May, same for day.
It can also take 2 digit month but 1 digit day, or the other way around as well.

It may throw std::invalid_argument with different messages depending on what went wrong.
\todo perhaps enum for different error points instead?
*/
std::chrono::year_month_day str_to_ymd(const std::string& str);

///Converts the argument to std::string representation in  yyyy/(m)m/(d)d format, 
///where the digits in parenthesis would be there if need to, i.e., May would result in 5, not 05.
std::string ymd_to_string(const std::chrono::year_month_day& ymd);

///Returns true if the character is between ASCII '0' to '9'.
inline bool char_is_number(const char c){
	return (c >= '0') && (c <= '9');
}

#endif