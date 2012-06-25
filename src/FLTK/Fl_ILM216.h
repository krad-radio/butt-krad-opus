//
// "$Id: Fl_ILM216.h,v 1.5 2003/02/26 00:40:22 easysw Exp $"
//
// ILM-216 LCD emulation widget header file for flcdsim.
//
// Copyright 2003 by Michael Sweet.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

//modified by Daniel Noethen

#ifndef FL_ILM216_H
#define FL_ILM216_H

//
// Include necessary headers...
//

#include <FL/Fl_Widget.H>
#include <FL/Fl_Bitmap.H>
#include <FL/Fl_Pixmap.H>

//
// Backlight color...
//

#define FL_NOLIGHT    (Fl_Color)76
#define FL_BACKLIGHT    (Fl_Color)85

//
// ILM-216 emulation widget...
//

class Fl_ILM216 : public Fl_Widget
{
  public:

  enum { CURSOR_NONE = 0, CURSOR_UNDERLINE, CURSOR_BLINK };

  private:

  bool         backlight_;            // Backlight enabled?
  uchar        buttons_;            // Current button state
  uchar        chars_[32];            // Characters on-screen
  int          cursor_pos_;            // Cursor position
  bool         cursor_state_;            // Cursor state (blinking)
  int          cursor_type_;            // Cursor type
  Fl_Bitmap    *font_[256];            // Images for characters
  Fl_Bitmap    *outline_[256];            // Outline images for characters
  Fl_Pixmap    *rec_;                  //blinks if recording
  Fl_Pixmap    *rec_dark_;                  //blinks if recording
  Fl_Pixmap    *rec_armed_;                  //blinks if recording
  Fl_Pixmap    *conn_;                  //blinks if streaming 
  Fl_Pixmap    *conn_dark_;                  //blinks if streaming 
  uchar        fdata_[224][96];        // Bitmap data (20x32)
  uchar        odata_[224][96];        // Outline data (20x32)
  uchar        prev_char_;            // Previous character

  void        draw();
  void        load_char(uchar ch, const uchar *data);
  void        load_font(void);

  public:

  Fl_ILM216(int X, int Y, int W, int H, const char *L = 0);
  ~Fl_ILM216();

  virtual int handle(int);

  bool    backlight() const { return backlight_; }
  void    backlight(bool b) { backlight_ = b; redraw(); }
  uchar   buttons() const { return buttons_; }
  void    buttons(uchar b) { buttons_ = b; }
  void    clear() { for (int i = 0; i < 32; i ++) chars_[i] = ' '; home(); redraw();}
  int     cursor_pos() const { return cursor_pos_; }
  void    cursor_pos(int p) { cursor_pos_ = p; }
  void    home() { cursor_pos(0); }
  int     print(const uchar *in, int inbytes);
};

#endif // !Fl_ILM216_h

//
// End of "$Id: Fl_ILM216.h,v 1.5 2003/02/26 00:40:22 easysw Exp $".
//

