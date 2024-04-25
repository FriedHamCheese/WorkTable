#ifndef align_hpp
#define align_hpp

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

/**
\file align.hpp
A header containing functions assisting with alignment of widgets.
*/

inline int xpos_right_of(const Fl_Widget& widget_on_the_left){
	return widget_on_the_left.x() + widget_on_the_left.w();
}

inline int xpos_left_of(const int xpos_of_widget_on_the_right, const int this_width){
	return xpos_of_widget_on_the_right - this_width;
}

inline int ypos_below(const Fl_Widget& widget_above){
	return widget_above.y() + widget_above.h();
}

///Returns the xpos of a widget which its horizontal center would be aligned vertically with xpos_point
inline int xpos_center_by_point(const int widget_width, const int xpos_point){
	return xpos_point - (widget_width/2);
}

#endif