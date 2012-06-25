#ifndef QT_FUNCS_H
#define QT_FUNCS_H


#define PRINT_LCD(text, len, home, clear)
#define SHOW_GUI()
#define CHECK_EVENTS()
#define ALERT(msg)
#define GUI_LOOP()

void fill_cfg_widgets();
void print_info(const char* info, int info_type);
void vu_meter(short left, short right);


#endif
