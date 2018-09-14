#include <X11/Xlib.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

// font info
// font name "fixed"

Window create_simple_window(Display* display, int width,
															int height, int x, int y)
{
	int screen_num = DefaultScreen(display);
	int win_border_width = 2;
	Window win;

	win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
														x, y, width, height, win_border_width,
														BlackPixel(display, screen_num),
														WhitePixel(display, screen_num));
	// display to screen
	XMapWindow(display, win);
	XFlush(display);
	return win;
}

GC create_gc(Display* display, Window win, int reverse_video)
{
	GC gc;
	unsigned long valuemask = 0;
	XGCValues values;
	unsigned int line_width = 2;
	int line_style = LineSolid;
	int cap_style = CapButt;
	int join_style = JoinBevel;
	int screen_num = DefaultScreen(display);

	gc = XCreateGC(display, win, valuemask, &values);
	if (gc < 0) {
		std::cerr << "XCreateGC: " << '\n';
	}

	// foreground and background colours
	if (reverse_video) {
		XSetForeground(display, gc, WhitePixel(display, screen_num));
		XSetBackground(display, gc, BlackPixel(display, screen_num));
	} else {
		XSetForeground(display, gc, BlackPixel(display, screen_num));
		XSetBackground(display, gc, WhitePixel(display, screen_num));
	}
	XSetLineAttributes(display, gc, line_width, line_style,
											cap_style, join_style);
	XSetFillStyle(display, gc, FillSolid);

	return gc;
}


int main(int argc, char** argv)
{
	Display* display;
	int screen_num;
	Window win;
	unsigned int display_width, display_height;
	unsigned int win_width, win_height;

//	unsigned int win_x, win_y;
//	unsigned int win_border_width;
	const char* display_name = getenv("DISPLAY");
	std::cout << "display env: " << display_name << '\n';
	GC gc;
	XFontStruct* font_info;
	std::string font_name = "fixed";

	display = XOpenDisplay(display_name);
	if(display == NULL){
		std::cerr << "cannot connect to x server" << '\n';
		return(1);
	}

	screen_num = DefaultScreen(display);
	display_width = DisplayWidth(display, screen_num);
	display_height = DisplayHeight(display, screen_num);

	win_width = (display_width / 3);
	win_height = (display_height / 3);

//	win_x = 0;
//	win_y = 0;
//
//	win_border_width = 0;

	win = create_simple_window(display, win_width, win_height, 0, 0);

	gc = create_gc(display, win, 0);
	XSync(display, False);

	int num_paths = 1;
	char** paths = XGetFontPath(display, &num_paths);

	font_info = XLoadQueryFont(display, font_name.c_str());
	if (!font_info){
		std::cerr << "failed loading font" << '\n';
		return 1;
	}

	XSetFont(display, gc, font_info->fid);

	{
		int x, y;
		std::string text_string ("Here is my test, let it be rendered");
		// const char* text_string = "c-style test string";
		int string_width;
		int font_height;

		font_height = font_info->ascent + font_info->descent;

		x = 0;
		y = font_height;
		std::cout << "debug"<< x << '\n' << y << '\n' << string_width << '\n' << font_height << '\n';
		XDrawString(display, win, gc, x, y, text_string.c_str(), text_string.length());
	}

	XFlush(display);



	std::this_thread::sleep_for(std::chrono::milliseconds(50000));


	XCloseDisplay(display);

	return(0);
}
