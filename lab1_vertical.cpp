// modified by: Tyler Brown
// date: 8/23/2022
//
// author: Gordon Griesel
// date: Spring 2022
// purpose: get openGL working on your personal computer
//
#include <iostream>
using namespace std;
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

// some structures

class Global
{
public:
	int xres, yres;
	float w;
	float dir;
	float ydir;
	float pos[2];
	int red, green, blue;
        int frame_count;
        bool disappear;
	Global();
} g;

class X11_wrapper
{
private:
	Display *dpy;
	Window win;
	GLXContext glc;

public:
	~X11_wrapper();
	X11_wrapper();
	void set_title();
	bool getXPending();
	XEvent getXNextEvent();
	void swapBuffers();
	void reshape_window(int width, int height);
	void check_resize(XEvent *e);
	void check_mouse(XEvent *e);
	int check_keys(XEvent *e);
} x11;

// Function prototypes
void init_opengl(void);
void physics(void);
void render(void);

//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
	init_opengl();
	// Main loop
	int done = 0;
	while (!done)
	{
		// Process external events.
		while (x11.getXPending())
		{
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			x11.check_mouse(&e);
			done = x11.check_keys(&e);
		}
		physics();
		render();
		x11.swapBuffers();
		usleep(200);
	}
	return 0;
}

Global::Global()
{
	xres = 400;
	yres = 200;
	w = 20.0f;
	dir = 25.0f;
	ydir = 10.0f;
	pos[0] = 0.0f + w;
	pos[1] = g.yres / 2.0f;
	red = 160;
	green = 150;
	blue = 220;
        frame_count = 0;
        disappear = false;
}

X11_wrapper::~X11_wrapper()
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

X11_wrapper::X11_wrapper()
{
	GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
	int w = g.xres, h = g.yres;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
	{
		cout << "\n\tcannot connect to X server\n"
			 << endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL)
	{
		cout << "\n\tno appropriate visual found\n"
			 << endl;
		exit(EXIT_FAILURE);
	}
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
						InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void X11_wrapper::set_title()
{
	// Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "3350 Lab1");
}

bool X11_wrapper::getXPending()
{
	// See if there are pending events.
	return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
	// Get a pending event.
	XEvent e;
	XNextEvent(dpy, &e);
	return e;
}

void X11_wrapper::swapBuffers()
{
	glXSwapBuffers(dpy, win);
}

void X11_wrapper::reshape_window(int width, int height)
{
	// window has been resized.
	g.xres = width;
	g.yres = height;
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
	// The ConfigureNotify is sent by the
	// server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != g.xres || xce.height != g.yres)
	{
		// Window size did change.
		reshape_window(xce.width, xce.height);
	}
}
//-----------------------------------------------------------------------------

void X11_wrapper::check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;

	// Weed out non-mouse events
	if (e->type != ButtonRelease &&
		e->type != ButtonPress &&
		e->type != MotionNotify)
	{
		// This is not a mouse event that we care about.
		return;
	}
	//
	if (e->type == ButtonRelease)
	{
		return;
	}
	if (e->type == ButtonPress)
	{
		if (e->xbutton.button == 1)
		{
			// Left button was pressed.
			// int y = g.yres - e->xbutton.y;
			return;
		}
		if (e->xbutton.button == 3)
		{
			// Right button was pressed.
			return;
		}
	}
	if (e->type == MotionNotify)
	{
		// The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y)
		{
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			// Code placed here will execute whenever the mouse moves.
		}
	}
}

int X11_wrapper::check_keys(XEvent *e)
{
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyPress)
	{
		switch (key)
		{
		case XK_1:
			// Key 1 was pressed
			break;
		case XK_Escape:
			// Escape key was pressed
			return 1;
		}
	}
	return 0;
}

void init_opengl(void)
{
	// OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	// Initialize matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Set 2D mode (no perspective)
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	// Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);
}

void change_color(int bounce)
{
    // Changes box color more blue if there wasn't a bounce recorded
    if (bounce == 0)
    {
        g.red-=20;
        if (g.red < 0)
            g.red = 0;
        g.blue+=20;
        if (g.blue > 255)
            g.blue = 255;
        g.green-=20;
        if (g.green < 0)
            g.green = 0;
    }

    // Changes box color more red if there was a bounce recorded
    else
    {
        g.red+=100;
        if (g.red > 255)
            g.red = 255;
        g.blue-=20;
        if (g.blue < 0)
            g.blue = 0;
        g.green-=20;
        if (g.green < 0)
            g.green = 0;
    }
}

// Changes size of box (smaller with more frequent bounces, bigger with less frequency)
void change_size(int value)
{
        if (value == 1)
        {
                g.w-=5;
                if (g.w < 10)
                        g.w = 10;
        }

        else if (value == 0)
        {
                g.w+=5;
                if (g.w > 100)
                        g.w = 100;
        }
}

void physics()
{
	g.pos[0] += g.dir;
	g.pos[1] += g.ydir;
	if (g.pos[0] >= (g.xres - g.w))
	{
		g.pos[0] = (g.xres - g.w);
		g.dir = -g.dir;
                // resets counter, records bounce
                g.frame_count = 0;
                change_color(1);
                change_size(1);
	}
	if (g.pos[0] <= g.w)
	{
		g.pos[0] = g.w;
		g.dir = -g.dir;
                // resets counter, records bounce
                g.frame_count = 0;
                change_color(1);
                change_size(1);
	}

	// Changes y-axis direction if collision with window occurs
	if (g.pos[1] >= (g.yres - g.w))
	{
	    g.pos[1] = (g.yres - g.w);
	    g.ydir = -g.ydir;
	    // resets counter, records bounce
	    g.frame_count = 0;
	    change_color(1);
            change_size(1);
	}

	// Changes y-axis direction if collision with window occurs
	if (g.pos[1] <= g.w)
	{
	    g.pos[1] = g.w;
	    g.ydir = -g.ydir;
	    // resets counter, records bounce
	    g.frame_count = 0;
	    change_color(1);
            change_size(1);
	}

    // If frame count reaches 10, continuously turn blue every render
    if (g.frame_count > 10)
    {
        g.frame_count = 10;
        change_color(0);
        change_size(0);
    }

    // If window size becomes smaller than box, will not draw box
    if (g.pos[0] >= (g.xres - g.w) && g.pos[0] <= g.w)
    {
        g.disappear = true;
    }

    else if (g.pos[1] >= (g.yres - g.w) && g.pos[1] <= g.w)
    {
	g.disappear = true;
    }
    // Continues to render box when window size becomes larger than box   
    else
        g.disappear = false;     
}

void render()
{
	//
	glClear(GL_COLOR_BUFFER_BIT);
	// Draw box so long as window is big enough for box
        if (!g.disappear)
        {
	        glPushMatrix();
                // Changed function values to variables to automatically update
	        glColor3ub(g.red, g.green, g.blue);
	        glTranslatef(g.pos[0], g.pos[1], 0.0f);
	        glBegin(GL_QUADS);
	        glVertex2f(-g.w, -g.w);
	        glVertex2f(-g.w, g.w);
	        glVertex2f(g.w, g.w);
	        glVertex2f(g.w, -g.w);
	        glEnd();
	        glPopMatrix();
                // Update frame count every render
                g.frame_count++;
        }
}
