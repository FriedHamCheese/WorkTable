#include "test_task_io.hpp"

#include <iostream>

int main(){
	std::clog << "Initiating test. If an [ALL CLEAR] is not displayed, the test has failed.\n";
	
	test_suite_task_io();
	
	std::clog << "[ALL CLEAR]: all tests verified.\n";
	return 0;
}