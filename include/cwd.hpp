// -*- C++ -*-
#ifndef _CWD_H_cm20180119_
#define _CWD_H_cm20180119_

#include <unistd.h>

class cwd {
public:
  cwd(const char* nwd) {
    getcwd(_owd, FILENAME_MAX);
    sprintf(_nwd, "%s/%s", _owd, nwd);
    chdir(_nwd);
  }
  ~cwd() {
    chdir(_owd);
  }
private:
  char _owd[FILENAME_MAX];
  char _nwd[FILENAME_MAX];
} ;

#endif // _CWD_H_cm20180119_
