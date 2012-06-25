#ifndef _PARSECONFIG_H_
#define _PARSECONFIG_H_

//taken from camE http://linuxbrit.co.uk/camE/

int    cfg_parse_file(const char *filename);
char** cfg_list_sections(void);
char** cfg_list_entries(const char *name);
char*  cfg_get_str(const char *sec, const char *ent);
int    cfg_get_int(const char *sec, const char *ent);
float  cfg_get_float(const char *sec, const char *ent);

int cfg_set_values();
int cfg_write_file();
int cfg_create_default();

enum {

    MAX_TAG_LENGTH = 64,
    MAX_VALUE_LENGTH = 192,
    MAX_SECTION_LENGTH = 100,
    MAX_LINE_LENGTH = MAX_VALUE_LENGTH + MAX_TAG_LENGTH +1
};

#endif /* _PARSECONFIG_H_ */

