//////////////////////////////////////////////
// View.cpp
//  Definitions for the CView class

#include "stdafx.h"
#include "view.h"
#include "FastGDIApp.h"
#include "resource.h"


CView::CView()
{
}

CView::~CView()
{
}

BOOL CView::LoadFileImage(LPCTSTR filename)
{
    // Only bitmap images (bmp files) can be loaded
    
    m_image.DeleteObject();
    CSize totalSize;

    if (filename)
    {
        m_image.LoadImage(filename, LR_LOADFROMFILE);
        if (!m_image.GetHandle())
        {
            CString str("Failed to load file:  ");
            str += filename;
            MessageBox(str, _T("File Load Error"), MB_ICONWARNING);
        }
    }

    if (m_image.GetHandle())
    {
        // Set the image scroll size
        totalSize.cx = GetImageRect().Width();
        totalSize.cy = GetImageRect().Height();
    }
    else
    {
        // Disable scrolling
        totalSize = CSize(0, 0);
        Invalidate();
    }

    SetScrollSizes(totalSize);
    return (m_image.GetHandle()!= 0);
}

BOOL CView::SaveFileImage(LPCTSTR fileName)
 {
     CFile file;
     BOOL bResult = FALSE;
     try
     {
         file.Open(fileName, OPEN_ALWAYS);
     
        // Create our LPBITMAPINFO object
        CBitmapInfoPtr pbmi(m_image);

        // Create the reference DC for GetDIBits to use
        CMemDC memDC(NULL);

        // Use GetDIBits to create a DIB from our DDB, and extract the colour data
        memDC.GetDIBits(m_image, 0, pbmi->bmiHeader.biHeight, NULL, pbmi, DIB_RGB_COLORS);
        std::vector<byte> vBits(pbmi->bmiHeader.biSizeImage, 0);
        byte* lpvBits = &vBits.front();

        memDC.GetDIBits(m_image, 0, pbmi->bmiHeader.biHeight, lpvBits, pbmi, DIB_RGB_COLORS);

        LPBITMAPINFOHEADER pbmih = &pbmi->bmiHeader;
        BITMAPFILEHEADER hdr;
        ZeroMemory(&hdr, sizeof(BITMAPFILEHEADER));
        hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
        hdr.bfSize = static_cast<DWORD>(sizeof(BITMAPFILEHEADER) + pbmih->biSize + pbmih->biClrUsed * sizeof(RGBQUAD) + pbmih->biSizeImage);
        hdr.bfOffBits = static_cast<DWORD>(sizeof(BITMAPFILEHEADER) + pbmih->biSize + pbmih->biClrUsed * sizeof (RGBQUAD));

        file.Write(&hdr, sizeof(BITMAPFILEHEADER));
        file.Write(pbmih, sizeof(BITMAPINFOHEADER) + pbmih->biClrUsed * sizeof (RGBQUAD));
        file.Write(lpvBits, pbmih->biSizeImage);

        if (file.GetLength() == sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pbmih->biClrUsed * sizeof (RGBQUAD) + pbmih->biSizeImage)
            bResult = TRUE;
     }

     catch (const CFileException& e)
     {
         CString str = CString("Failed to save file: ") + e.GetFilePath();
         MessageBox(str, AtoT(e.what()), MB_OK);
     }

    return bResult;
}

CRect CView::GetImageRect()
{
    BITMAP bm;
    m_image.GetObject(sizeof(BITMAP), &bm);

    CRect rc;
    rc.right = bm.bmWidth;
    rc.bottom = bm.bmHeight;

    return rc;
}

void CView::OnDraw(CDC& dc)
{
    if (m_image.GetHandle())
    {
        dc.SelectObject(m_image);
    }
    else
    {
        // There is no image, so display a hint to get one
        CRect rc = GetClientRect();
        dc.DrawText(_T("Use the Menu or ToolBar to open a Bitmap File"), -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

LRESULT CView::OnDropFiles(UINT msg, WPARAM wparam, LPARAM lparam)
{
    UNREFERENCED_PARAMETER(msg);
    UNREFERENCED_PARAMETER(lparam);

    HDROP hDrop = (HDROP)wparam;
    UINT length = DragQueryFile(hDrop, 0, 0, 0);

    if (length > 0)
    {
        CString fileName;
        DragQueryFile(hDrop, 0, fileName.GetBuffer(length), length+1);
        fileName.ReleaseBuffer();
        DragFinish(hDrop);

        CMainFrame& Frame = GetFrameApp().GetMainFrame();

        if ( !Frame.LoadFile(fileName) )    
        {
            TRACE ("Failed to load "); TRACE(fileName); TRACE("\n");
            SetScrollSizes(CSize(0,0));
            Invalidate();
        }
    }
    
    return 0;
}

void CView::OnInitialUpdate()
{
    // OnInitialUpdate is called after the window is created
    TRACE("View window created\n");

    // Support Drag and Drop on this window
    DragAcceptFiles(TRUE);
}

void CView::PreCreate(CREATESTRUCT& cs)
{
    // Set the Window Class name
    cs.lpszClass = _T("View");

    cs.style = WS_CHILD | WS_HSCROLL | WS_VSCROLL ;

    // Set the extended style
    cs.dwExStyle = WS_EX_CLIENTEDGE;
}

LRESULT CView::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DROPFILES:          return OnDropFiles(msg, wparam, lparam);
    }

    // Pass unhandled messages on for default processing
    return WndProcDefault(msg, wparam, lparam);
}

