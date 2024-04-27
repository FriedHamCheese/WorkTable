/*
Copyright 2024 Pawikan Boonnaum.

This file is part of WorkTable.

WorkTable is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WorkTable is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with WorkTable. If not, see <https://www.gnu.org/licenses/>.
*/

#include "MainWindow.hpp"

#include <FL/Fl.H>
#include <FL/fl_ask.h>

int main(){
	try{
		//If you ran into a stack overflow issue, change this to pointer.
		MainWindow window(0, 0, MainWindow::width, MainWindow::height, "WorkTable 0.5.0");
		window.show();
		return Fl::run();
	}
	catch(const std::bad_alloc& alloc_err) {
		fl_alert("Uncaught memory allocation error. (main.cpp: std::bad_alloc)"
				"\nThe program will now terminate.");
	}	
	catch(const std::length_error& exceeded_max_alloc) {
		fl_alert("Uncaught maximum memory allocation error. (main.cpp: std::length_error)"
				"\nThe program will now terminate.");
	}	
	catch(const std::exception& unspecified_excp){
		const std::string msg = std::string("Caught unspecified exception. (main.cpp)\n")
								+ std::string(unspecified_excp.what())
								+ "\nThe program will now terminate.";
		fl_alert(msg.c_str());
	}
	catch(...){
		fl_alert("Caught unspecified throw. (main.cpp)"
				"\nThe program will now terminate.");
	}
	
	return -1;
}