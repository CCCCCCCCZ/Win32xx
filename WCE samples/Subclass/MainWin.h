///////////////////////////////
// MainWin.h


#ifndef MAINWIN_H
#define MAINWIN_H


#include "Button.h"


class CMainWin : public CWnd
{
public:
    CMainWin();
    ~CMainWin() {}

protected:
    LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam);
    int OnCreate(CREATESTRUCT& cs);

private:
    CButton m_button;
};



#endif //MAINWIN_H

