#ifndef FL_MY_DOUBLE_WINDOW_H
#define FL_MY_DOUBLE_WINDOW_H

#include <FL/Fl_Double_Window.H>
#include <FL/x.H>

#ifdef _WIN32
 #include <windows.h>
#elif __APPLE__
 #include <Carbon/Carbon.h>
#else
 #include <X11/Xlib.h>
#endif

class Fl_My_Double_Window : public Fl_Double_Window {
    public:
        void stay_on_top(int ontop) {
#ifdef _WIN32
            SetWindowPos(fl_xid(this), ontop ? HWND_TOPMOST : HWND_NOTOPMOST,
                    0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER);
#elif __APPLE__
            WindowClass win_class;

            GetWindowClass(fl_xid(this), &win_class);

            if(ontop && win_class == kDocumentWindowClass)
                SetWindowClass(fl_xid(this), kUtilityWindowClass);
            if(!ontop && win_class == kUtilityWindowClass)
                SetWindowClass(fl_xid(this), kDocumentWindowClass);
#else //UNIX
            XEvent e;

            Atom WM_STATE = XInternAtom(fl_display, "_NET_WM_STATE", 0);
            Atom WM_STATE_ABOVE = XInternAtom(fl_display, "_NET_WM_STATE_ABOVE", 0);

            e.xclient.type = ClientMessage;
            e.xclient.window = fl_xid(this);
            e.xclient.message_type = WM_STATE;
            e.xclient.format = 32;
            e.xclient.data.l[0] = (long)ontop;
            e.xclient.data.l[1] = (long)WM_STATE_ABOVE;
            e.xclient.data.l[2] = (long)0;
            e.xclient.data.l[3] = (long)0;
            e.xclient.data.l[4] = (long)0;

            XSendEvent(fl_display, RootWindow(fl_display, fl_screen), 0,
                    SubstructureNotifyMask|SubstructureRedirectMask, &e);
            XFlush(fl_display);
#endif
        }

        Fl_My_Double_Window(int W, int H, const char *l = 0)
            : Fl_Double_Window(W,H,l) {}

        Fl_My_Double_Window(int X, int Y, int W, int H, const char *l = 0)
            : Fl_Double_Window(X,Y,W,H,l) {}

        ~Fl_My_Double_Window() { hide(); }
};

#endif //FL_MY_DOUBLE_WINDOW_H
