/* (31-Dec-2017) [Tab/Indent: 4/4][Line/Box: 80/74]               (FontEx.cpp) *
********************************************************************************
|                                                                              |
|                         robert.c.tausworthe@ieee.org                         |
|                                                                              |
===============================================================================*

    Contents Description: Implementation of an extention of the CFont class
    using the Win32++ Windows interface classes, Copyright (c) 2005-2017 David 
    Nash, under permissions granted therein. Extensions include font size, 
    color, font dialog option flags, and coordinate mapping mode.

 	Caveats: These materials are available under the same provisions as found 
	in the Win32++ copyright.txt notice.

    Programming Notes:
        The programming standards roughly follow those established by the 
    1997-1999 Jet Propulsion Laboratory Network Planning and Preparation 
    Subsystem project for C++ programming.

*******************************************************************************/

#include "stdafx.h"
#include "FontEx.h"

/*============================================================================*/
    CFontEx::
CFontEx()                                                                   /*

    Create an extended font object.
*-----------------------------------------------------------------------------*/
{
    txcolor = RGB(0, 0, 0);
    flags = 0;
    width = 0;
    height = 0;
}

/*============================================================================*/
    CFontEx::
~CFontEx()                                                                  /*

    Destructor.
*-----------------------------------------------------------------------------*/
{
}

/*============================================================================*/
    void CFontEx::
Choose(CDC& dc, LPCTSTR wintitle /* =  NULL */)                             /*

        Select the object font typeface, characteristics, color, and size in
    the device contex dc. The font background color is presumed to be
    supplied by the application. Label the choice box using wintitle.
*-----------------------------------------------------------------------------*/
{
          // load the current font
    LOGFONT lf;
    font.GetObject(sizeof(LOGFONT), &lf);
          // open the dialog
    CFontExDialog fd;
    if (wintitle != NULL)
        fd.SetBoxTitle(wintitle);
    CHOOSEFONT cf = fd.GetParameters();
      // display effects and color boxes, and use logfont provided, as
      // well as any other initializations in flags
    cf.Flags |= CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | flags;
    cf.lpLogFont = &lf;
    cf.rgbColors = txcolor;
    fd.SetParameters(cf);
    if(fd.DoModal())
    {
          // bring choice elements into this view
        font.CreateFontIndirect(fd.GetLogFont());
            txcolor = fd.GetColor();
        GetSize(dc);
    }
}

/*============================================================================*/
    void CFontEx::
GetSize(CDC& dc)                                                            /*

    Record the width and height of the object font in the device context dc,
    in client device units.
*-----------------------------------------------------------------------------*/
{
      // select the current font into the device context: save the old
    HFONT old_font = (HFONT)dc.SelectObject(font);
      // measure the font width and height
    TEXTMETRIC tm;
    dc.GetTextMetrics(tm);
    width  = tm.tmAveCharWidth;
    height = tm.tmHeight + tm.tmExternalLeading;
          // restore entry font
    dc.SelectObject(old_font);
}

/*============================================================================*/
    void CFontEx::
Serialize(CArchive &ar)                                                     /*

    Save and restore the current CFontEx object in the archive ar,
    maintaining compatability between ANSI and UNICODE versions of archived
    objects. The difference between the two character representations lies
    in the lengths of the face name strings in the two modes.
*-----------------------------------------------------------------------------*/
{
        LOGFONT lf;
      // get the size of the LOGFONT, sans face name
    UINT lfTopLength = sizeof(LOGFONTA) - LF_FACESIZE*sizeof(CHAR);
      // f will archive this top part
    ArchiveObject f(&lf, lfTopLength);
        if (ar.IsStoring()) // storing
        {
            font.GetObject(sizeof(LOGFONT), &lf);
          // store the face name separately: ar recognizes the char mode
            CString face = lf.lfFaceName;
            ar << f;    // store the top part
            ar<< face;  // store the face name part
        ar << txcolor;  // store the rest
            ar << flags;
            ar << width;
            ar << height;
    }
        else    // recovering
        {
            CString face;
            ar >> f;    // recover the top part
            ar >> face; // recover the face name and put it in the LOGFONT
            memcpy(lf.lfFaceName, face.c_str(),
            (face.GetLength() + 1) * sizeof(TCHAR));
            font.CreateFontIndirect((const LOGFONT&)lf);
                 // recover the rest
                ar >> txcolor;
                ar >> flags;
            ar >> width;
            ar >> height;
        }
}

/*============================================================================*/
        void CFontEx::
SetDefault(CDC& dc)                                                         /*

        Create the default app view font.
*-----------------------------------------------------------------------------*/
{
      // compute the logical font height for the nDefaultFontSize
    long lfHeight = -MulDiv(nDefaultFontSize, dc.GetDeviceCaps(LOGPIXELSY),
        72);

        int nHeight = lfHeight;     // logical height of font
        int nWidth = 0;         // logical average character width
        int nEscapement = 0;        // angle of escapement
        int nOrientation = 0;           // base-line orientation angle
        int fnWeight = FW_REGULAR;  // font weight
        DWORD fdwItalic = 0;            // italic attribute flag
        DWORD fdwUnderline = 0;         // underline attribute flag
        DWORD fdwStrikeOut = 0;         // strikeout attribute flag
        DWORD fdwCharSet = 0;           // character set identifier
        DWORD fdwOutputPrecision = 0;   // output precision
        DWORD fdwClipPrecision = 0;     // clipping precision
        DWORD fdwQuality = 0;           // output quality
        DWORD fdwPitchAndFamily = 0;    // pitch and family
        LPCTSTR lpszFace = lpszFaceDefault; // pointer to typeface name string

    HFONT hf = CreateFont
            (
                nHeight,                // logical height of font
                nWidth,                 // logical average character width
                nEscapement,            // angle of escapement
                nOrientation,           // base-line orientation angle
                fnWeight,               // font weight
                fdwItalic,              // italic attribute flag
                fdwUnderline,           // underline attribute flag
                fdwStrikeOut,           // strikeout attribute flag
                fdwCharSet,             // character set identifier
                fdwOutputPrecision,     // output precision
                fdwClipPrecision,       // clipping precision
                fdwQuality,             // output quality
                fdwPitchAndFamily,      // pitch and family
                lpszFace                // pointer to typeface name string
            );

    if(hf)
    {
        font.DeleteObject();
        font = CFont(hf);
        GetSize(dc);
        txcolor = rgbDefaultTxColor;
    }
    else
        throw  CUserException(_T("Font creation error."));
}
