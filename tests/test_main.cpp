#include "test_task_io.hpp"

#include <iostream>

int main(){
	try{	
		std::clog << "Initiating test. If an [ALL CLEAR] is not displayed, the test has failed.\n";
		test_suite_task_io();
		std::clog << "[ALL CLEAR]: all tests verified.\n";
		return 0;
	}
	catch(const std::exception& excp){
		std::cerr << "[Test Failed]: caught an unspecified exception.\n";
		std::cerr << "\tMessage of the exception: " << excp.what() << '\n';
	}
	catch(...){
		std::cerr << "[Test Failed]: caught an unspecified throw.\n";
	}	
	return -1;
}