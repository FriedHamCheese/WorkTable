#ifndef align_hpp
#define align_hpp

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

inline int ypos_below(const Fl_Widget* const widget) noexcept{
	return widget->y() + widget->h();
}

inline int xpos_right_of(const Fl_Widget* const widget) noexcept{
	return widget->x() + widget->w();
}

inline int ypos_below(const Fl_Widget& widget) noexcept{
	return ypos_below(&widget);
}

inline int xpos_right_of(const Fl_Widget& widget) noexcept{
	return xpos_right_of(&widget);
}

#endif