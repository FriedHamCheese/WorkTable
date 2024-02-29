#include "task_io.hpp"

#include "Task.hpp"
#include "time_calc.hpp"

#include <FL/fl_ask.H>

#include <memory>
#include <vector>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <utility>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace task_io_internal{
	TaskStr::TaskStr(const std::string& due_date, const std::string& name)
	:	due_date(due_date), name(name)
	{
	}

	std::pair<std::unique_ptr<char[]>, int> get_raw_file(const std::string& filename){
		std::ifstream file(filename);
		
		if(!file.is_open()) throw std::runtime_error(std::string("task_io.cpp: get_raw_file(): ") + filename +  " cannot be opened.");
		if(file.bad()) throw std::runtime_error("task_io.cpp: get_raw_file(): stream not ready for reading.");
		
		file.seekg(0, file.end);
		const int filesize = file.tellg();
		file.seekg(0, file.beg);
		
		std::unique_ptr<char[]> buffer(new char[filesize]);
		file.read(buffer.get(), filesize);
		
		if(file.bad()) throw std::runtime_error("task_io.cpp: get_raw_file(): filestream lost integrity. The buffer should be discarded.");	
		
		return std::make_pair(std::move(buffer), file.gcount());
	}

	std::vector<std::string> buffer_to_separated_lines(const std::pair<std::unique_ptr<char[]>, int>& buffer){
		const char* const data = buffer.first.get();
		const int character_count = buffer.second;
		
		std::vector<std::string> lines; lines.reserve(20);
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

	std::vector<TaskStr> lines_to_taskstrs(const std::vector<std::string>& lines){
		//from line to TaskStr
		std::vector<TaskStr> tasks_str; tasks_str.reserve(20);
		
		for(const std::string& line : lines){
			std::string date_str; date_str.reserve(10);
			std::string name_str; name_str.reserve(20);
			
			const auto line_length = line.size();
			auto date_str_end_index = line_length; //default
			
			for(decltype(line.size()) i = 0; i < line_length; ++i){
				if(line[i] == ','){
					date_str_end_index = i;
					break;
				}
				else date_str += line[i];
			}
					
			//+2 for skipping comma and space
			for(auto i = date_str_end_index+2; i < line_length; ++i){
				name_str += line[i];
			}
			
			tasks_str.emplace_back(date_str, name_str);
		}
		tasks_str.shrink_to_fit();
		return tasks_str;
	}

	std::vector<Task> taskstrs_to_tasks(const std::vector<TaskStr>& taskstrs){
		std::vector<Task> tasks; tasks.reserve(taskstrs.size());
		
		const std::chrono::year_month_day current_ymd = get_current_ymd();
		
		for(const TaskStr& task_str : taskstrs){
			const std::optional<std::chrono::year_month_day> task_ymd = str_to_ymd(task_str.due_date);
			
			const bool valid_due_date = task_ymd.has_value();
			if(valid_due_date){
				tasks.emplace_back(task_ymd.value(), task_str.name, current_ymd);
			}else{
				//[name] ... (date).
				//...
				const std::string msg = task_str.name + " not loaded due to invalid due date" + " (" + task_str.due_date + ")."
										+ "\n(task_io.cpp: taskstrs_to_tasks())";
				fl_alert(msg.c_str());
			}
		}
		return tasks;
	}
}

std::vector<Task> get_tasks(){
	try{
		const std::vector<std::string> lines = task_io_internal::buffer_to_separated_lines(task_io_internal::get_raw_file("tasks.txt"));
		const std::vector<task_io_internal::TaskStr> taskstrs = task_io_internal::lines_to_taskstrs(lines);
	
		return task_io_internal::taskstrs_to_tasks(taskstrs);
	}
	catch(std::exception& excp){throw excp;}
}

//returns string version of number with always 2 digits.
std::string int_to_2char(const unsigned num){
	const std::string num_str = std::to_string(num);
	//anything less than 10 is written with only 1 number, so we put a 0 in the front.
	if(num < 10) return "0" + num_str;
	else return num_str;
}

void overwrite_taskfile(const std::vector<Task>& tasks){
	std::string buffer; buffer.reserve(400);
	
	for(const Task& task : tasks){
		buffer += task_to_str(task) + '\n';
	}
	
	//remove last trailing \n
	buffer.erase(buffer.end() - 1);
	buffer.shrink_to_fit();
	
	int user_decision = 0;
	do{
		std::ofstream file("tasks.txt", std::ofstream::out);

		file.write(buffer.c_str(), buffer.size());
		
		if(file.bad()){
			user_decision = fl_choice("Anomaly detected while saving task. Try resaving?", "Yes", "Keep anomaly", 0);
		}else break;
		
	}while(user_decision == 0);
}


std::string task_to_str(const Task& task){
	const std::string year_str = std::to_string(int(task.due_date().year()));
	const std::string month_str = int_to_2char(unsigned(task.due_date().month()));
	const std::string day_str = int_to_2char(unsigned(task.due_date().day()));
	const std::string ymd_str = year_str + "/" + month_str + "/" + day_str;	
	
	return ymd_str + ", " + task.name();
}


//parse string in yyyy/(m)m/(d)d format, to std::chrono::year_month_day
std::optional<std::chrono::year_month_day> str_to_ymd(const std::string& str){
	const auto str_length = str.size();
	std::string buffer; buffer.reserve(5);
	
	constexpr decltype(str_length) min_str_length = 8;
	if(str_length < min_str_length) return std::nullopt;
	
	decltype(str.size()) i = 0;
	
	//fetching year
	for(; i < 4; ++i){
		const char& c = str[i];
		
		if(char_is_number(c)) buffer += c;
		else return std::nullopt;
	}
	const auto year = std::chrono::year(std::stoi(buffer));
	buffer.clear();	
	
	
	//+1 to skip '/' between year and month
	for(i += 1; i < 7; ++i){
		const char c = str[i];
		
		if(char_is_number(c)) buffer += c;
		else if(c == '/') break;
		else return std::nullopt;
	}
	const auto month = std::chrono::month(std::stoi(buffer));
	buffer.clear();	
	
	
	//+1 to skip '/' between month and day
	for(i += 1; i < str_length; ++i){
		const char c = str[i];
		
		if(char_is_number(c)) buffer += c;
		else return std::nullopt;
	}
	const auto day = std::chrono::day(std::stoi(buffer));
	buffer.clear();
	
	
	const auto ymd = std::chrono::year_month_day(year, month, day);
	if(ymd.ok()) return ymd;
	else return std::nullopt;
	
}

std::string ymd_to_string(const std::chrono::year_month_day& ymd){
	const auto year = int(ymd.year());
	const auto month = unsigned(ymd.month());
	const auto day = unsigned(ymd.day());
	
	return std::to_string(year) + '/' + std::to_string(month) + '/' + std::to_string(day);
}