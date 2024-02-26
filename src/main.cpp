#include "MainWindow.hpp"
#include <FL/Fl.H>

//grouped tasks

int main(){
	MainWindow window(0, 0, MainWindow::width, MainWindow::height, "WorkTable 0.29");
	window.show();
	
	return Fl::run();
}