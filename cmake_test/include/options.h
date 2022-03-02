

#ifndef __OPTIONS_H_
#define __OPTIONS_H_
#include "gflags/gflags.h"

DEFINE_bool(big_menu, true, "Include 'advanced' options in the menu listing");
DEFINE_string(languages, "english,french,german",
              "comma-separated list of languages to offer in the 'lang' menu");

#endif 
