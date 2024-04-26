#include "task_io.hpp"

#include "Task.hpp"
#include "time_calc.hpp"

#include <FL/fl_ask.H>

#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <utility>
#include <fstream>
#include <stdexcept>

namespace task_io_internal{
	TaskStr::TaskStr(const std::string& due_date, const std::string& name)
	:	due_date(due_date), name(name)
	{
	}

	file_buffer get_raw_file(const std::string& filename){
		std::ifstream file(filename);
		
		if(!file.is_open()) 
			throw std::runtime_error(std::string("task_io.cpp: get_raw_file(): ") + filename +  " cannot be opened.");
		if(file.bad()) 
			throw std::ios_base::failure("task_io.cpp: get_raw_file(): stream not ready for reading.");

		std::unique_ptr<char[]> buffer;
		decltype(file.tellg()) filesize = 0;
		try{
			file.seekg(0, file.end);
			filesize = file.tellg();
			file.seekg(0, file.beg);
		
			buffer.reset(new char[filesize]);
			file.read(buffer.get(), filesize);
		}
		catch(const std::ios_base::failure& file_error) {throw;}		
		
		if(file.bad()) 
			throw std::ios_base::failure("task_io.cpp: get_raw_file(): filestream lost integrity. The buffer should be discarded.");	
		
		return std::make_pair(std::move(buffer), file.gcount());
	}

	std::vector<std::string> buffer_to_separated_lines(const file_buffer& buffer){
		const char* const data = buffer.first.get();
		const int character_count = buffer.second;
		
		std::vector<std::string> lines;
		lines.reserve(20);
		std::string fetch_line;

		for(auto i = 0; i < character_count; ++i){
			const char c = data[i];
			
			if(c != '\n') fetch_line += c;
			
			const bool end_of_file = ((i+1) == character_count);
			
			if((c == '\n' || end_of_file) && !fetch_line.empty()){
				lines.emplace_back(fetch_line);
				fetch_line.clear();
			}
		}
		
		lines.shrink_to_fit();
		return lines;
	}
	
	void default_nested_group_callback(const char* const callsite_filename, const int callsite_line, 
										const std::string& first_taskgroup_name, const std::string& second_taskgroup_name)
		{
		const std::string msg = std::string(callsite_filename) + ":" + std::to_string(callsite_line) + ": Detected nested task group " + second_taskgroup_name + ". Tasks will be in  " + first_taskgroup_name + ".";
		fl_alert(msg.c_str());
	}

	std::vector<TaskStrGroup> lines_to_TaskStrGroup(const std::vector<std::string>& lines, 
													void(*nested_group_callback)(const char*, const int, const std::string&, const std::string&))
	{
		//this keeps all the TaskStrGroups fetched, which can either be a single task in TaskStrGroup or a multiple.
		std::vector<TaskStrGroup> task_str_groups; 
		task_str_groups.reserve(20);
		
		TaskStrGroup fetching_taskstr_group;		
		
		bool fetching_group = false;
		const std::size_t line_count = lines.size();
		for(std::size_t line_i = 0; line_i < line_count; line_i++){
			/*
			There are 3 parts of the code:
			1. Detect group definition (line which ends with {).
			   If there is a group definition, move on from the current line to next.
			2. If the line isn't a group end character, fetch the date and name of a task, 
			   to fetching_taskstr_group which either can be fetching to a group or to an independent task.
			3. If we either reached a group end character in this line or was fetching an independent task, or reached the last line:
			   we save the fetching_taskstr_group to task_str_groups.
			*/
			const std::string& line = lines[line_i];
			
			//We check if this line has the { indicating the line is defining a new group,
			//and if the code is already is fetching the tasks of previous group member
			//if both are true, it means this line is defining another group inside of a group which we don't want.
			//We simply warn the user and move on to the next line.
			
			//1.
			const bool nested_group = (line.back() == '{') && fetching_group;
			if(nested_group){
				const std::string nested_group_name = line.substr(0, line.size() - 1);;
				nested_group_callback(__FILE__, __LINE__, fetching_taskstr_group.group_name, nested_group_name);
				continue;
			}
			
			if(line.back() == '{'){
				fetching_group = true;
				fetching_taskstr_group.group_name = line.substr(0, line.size() - 1);
				continue;
			}
			
			//2.
			if(line == "}") fetching_group = false;
			else{
				std::string date_str; date_str.reserve(10);
				std::string name_str; name_str.reserve(20);
				
				const auto line_length = line.size();
				auto date_str_end_index = line_length; //default
				
				for(std::size_t i = 0; i < line_length; ++i){
					if(line[i] == ','){
						date_str_end_index = i;
						break;
					}
					else date_str += line[i];
				}
				
				name_str += line.substr(date_str_end_index+2, line_length);				
				fetching_taskstr_group.taskstrs.emplace_back(date_str, name_str);
			}
			
			//3.
			const bool last_line = line_i + 1 == line_count;
			if(!fetching_group || last_line){
				if(fetching_taskstr_group.taskstrs.size() > 0){
					task_str_groups.push_back(fetching_taskstr_group);
					fetching_taskstr_group.taskstrs.clear();
					fetching_taskstr_group.group_name.clear();
				}
			}
		}

		task_str_groups.shrink_to_fit();
		return task_str_groups;
	}
	
	std::vector<TaskGroup> TaskStrGroups_to_TaskGroups(const std::vector<TaskStrGroup>& taskstr_groups){
		std::vector<TaskGroup> taskgroups; 
		taskgroups.reserve(taskstr_groups.size());
		
		const std::chrono::year_month_day current_date = get_current_ymd();
		
		//A loop iterating over each TaskStrGroup to convert it to TaskGroup and appends it to the return vector.
		for(const TaskStrGroup& current_taskstr_group : taskstr_groups){
			TaskGroup fetching_taskgroup;
			fetching_taskgroup.group_name = current_taskstr_group.group_name;
			
			//A loop iterating over each TaskStr in TaskStrGroup which converts it to Task and appends it to the TaskGroup.
			for(const TaskStr& current_taskstr : current_taskstr_group.taskstrs){
				try{
					const std::chrono::year_month_day due_date = str_to_ymd(current_taskstr.due_date);
					fetching_taskgroup.tasks.emplace_back(due_date, current_taskstr.name, current_date);
				}
				catch(std::invalid_argument& invalid_ymd){
					const std::string msg = std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": Task " + current_taskstr.name + " due to invalid due date.";
					fl_alert(msg.c_str());
				}
			}
			
			taskgroups.push_back(fetching_taskgroup);
		}

		return taskgroups;
	}
}//namespace task_io_internal

std::vector<TaskGroup> get_tasks(void(*nested_group_callback)(const char*, const int, const std::string&, const std::string&)){
	task_io_internal::file_buffer raw_file_data;
	try{
		raw_file_data = task_io_internal::get_raw_file("tasks.txt");
	}
	catch(const std::ios_base::failure& file_io_error) {throw;}
	catch(const std::runtime_error& file_not_opened) {throw;}

	const std::vector<std::string> lines = task_io_internal::buffer_to_separated_lines(raw_file_data);
	const std::vector<task_io_internal::TaskStrGroup> taskstr_groups = task_io_internal::lines_to_TaskStrGroup(lines, nested_group_callback);

	return task_io_internal::TaskStrGroups_to_TaskGroups(taskstr_groups);
}


std::string int_to_2char(const unsigned num){
	const std::string num_str = std::to_string(num);
	//anything less than 10 is written with only 1 number, so we put a 0 in the front.
	if(num < 10) return "0" + num_str;
	else return num_str;
}

void overwrite_taskfile(const std::vector<TaskGroup>& taskgroups){
	std::string buffer; 
	buffer.reserve(400);
	
	for(const TaskGroup& taskgroup : taskgroups){
		if(taskgroup.tasks.size() == 1){
			buffer += task_to_str(taskgroup.tasks[0]) + '\n';
		}
		else{
			buffer += taskgroup.group_name + "{\n";
			for(const Task& task : taskgroup.tasks){
				buffer += task_to_str(task) + '\n';
			}
			buffer += "}\n";
		}
	}
	
	const int resave_requested = 0;
	int user_decision = resave_requested;
	do{
		std::ofstream file("tasks.txt", std::ofstream::out);
		file.write(buffer.c_str(), buffer.size());
		
		//file.bad() trips when writing an empty file, so a buffer size check is added.
		if(file.bad() && buffer.size() != 0){
			user_decision = fl_choice("Anomaly detected while saving task. Try resaving?", "Resave", "Keep anomaly", 0);
		}else break;
		
	}while(user_decision == resave_requested);
}


std::string task_to_str(const Task& task){
	const std::string year_str = std::to_string(int(task.due_date().year()));
	const std::string month_str = int_to_2char(unsigned(task.due_date().month()));
	const std::string day_str = int_to_2char(unsigned(task.due_date().day()));
	const std::string ymd_str = year_str + "/" + month_str + "/" + day_str;	
	
	return ymd_str + ", " + task.name();
}

std::chrono::year_month_day str_to_ymd(const std::string& str){
	const auto str_length = str.size();
	std::string buffer; 
	
	buffer.reserve(5);
	
	//includes 4 characters for yyyy, 2 for slashes, 1 for month and 1 for day.
	constexpr decltype(str_length) min_str_length = 8;
	if(str_length < min_str_length) throw std::invalid_argument("string of ymd is too short. Minimum is 8 characters.");
	
	decltype(str.size()) i = 0;
	
	//fetching year
	for(; i < 4; ++i){
		const char& c = str[i];
		
		if(char_is_number(c)) buffer += c;
		else throw std::invalid_argument("ymd must only contain numbers or forward slash '/'.");
	}
	const auto year = std::chrono::year(std::stoi(buffer));
	buffer.clear();	
	
	
	//+1 to skip '/' between year and month
	for(i += 1; i < 7; ++i){
		const char c = str[i];
		
		if(char_is_number(c)) buffer += c;
		else if(c == '/') break;
		else throw std::invalid_argument("ymd must only contain numbers or forward slash '/'.");
	}
	if(buffer.empty()) throw std::invalid_argument("Month section not provided.");
	const auto month = std::chrono::month(std::stoi(buffer));
	buffer.clear();	
	
	
	//+1 to skip '/' between month and day
	for(i += 1; i < str_length; ++i){
		const char c = str[i];
		
		if(char_is_number(c)) buffer += c;
		else throw std::invalid_argument("Days must only contain numbers.");
	}
	const auto day = std::chrono::day(std::stoi(buffer));
	buffer.clear();
	
	
	const auto ymd = std::chrono::year_month_day(year, month, day);
	if(ymd.ok()) return ymd;
	else throw std::invalid_argument("Invalid ymd.");
}

std::string ymd_to_string(const std::chrono::year_month_day& ymd){
	const auto year = int(ymd.year());
	const auto month = unsigned(ymd.month());
	const auto day = unsigned(ymd.day());
	
	return std::to_string(year) + '/' + std::to_string(month) + '/' + std::to_string(day);
}