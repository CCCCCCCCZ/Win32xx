///////////////////////////////////////////////////////
// Mainfrm.h
//  Declaration of the CMainFrame class

#ifndef MAINFRM_H
#define MAINFRM_H


#include "View.h"
#include "Doc.h"


class CMainFrame : public CFrame
{
public:
    CMainFrame();
    virtual ~CMainFrame();

    CDoc& GetDoc() { return m_doc; }
    void LoadFile(LPCTSTR fileName);

    LRESULT OnDropFile(WPARAM wparam);

    void OnFileExit();
    void OnFileMRU(WPARAM wparam);
    void OnFileNew();
    void OnFileOpen();
    void OnFileSave();
    void OnFileSaveAs();
    void OnFilePrint();
    void OnPenColor();

protected:
    virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam);
    virtual int OnCreate(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void SetupToolBar();
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

private:
    CView m_view;
    CDoc m_doc;
    CString m_pathName;

};

#endif //MAINFRM_H

