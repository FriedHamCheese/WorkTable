#ifndef align_hpp
#define align_hpp

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

inline int xpos_right_of(const Fl_Widget& widget) noexcept{
	return widget.x() + widget.w();
}

inline int xpos_center_of(const Fl_Widget& widget) noexcept{
	return widget.x() + (widget.w() / 2);
}

inline int ypos_below(const Fl_Widget& widget) noexcept{
	return widget.y() + widget.h();
}

#endif