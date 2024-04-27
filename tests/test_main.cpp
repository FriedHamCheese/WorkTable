/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

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