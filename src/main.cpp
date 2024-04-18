#include "MainWindow.hpp"

#include <FL/Fl.H>
#include <FL/fl_ask.h>

int main(){
	try{
		MainWindow window(0, 0, MainWindow::width, MainWindow::height, "WorkTable 0.4ish - group-gestures branch");
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