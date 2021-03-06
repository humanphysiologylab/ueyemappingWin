//===========================================================================//
//                                                                           //
//  Copyright (C) 2004 - 2017                                                //
//  IDS Imaging GmbH                                                         //
//  Dimbacherstr. 6-8                                                        //
//  D-74182 Obersulm-Willsbach                                               //
//                                                                           //
//  The information in this document is subject to change without            //
//  notice and should not be construed as a commitment by IDS Imaging GmbH.  //
//  IDS Imaging GmbH does not assume any responsibility for any errors       //
//  that may appear in this document.                                        //
//                                                                           //
//  This document, or source code, is provided solely as an example          //
//  of how to utilize IDS software libraries in a sample application.        //
//  IDS Imaging GmbH does not assume any responsibility for the use or       //
//  reliability of any portion of this document or the described software.   //
//                                                                           //
//  General permission to copy or modify, but not for profit, is hereby      //
//  granted,  provided that the above copyright notice is included and       //
//  reference made to the fact that reproduction privileges were granted     //
//  by IDS Imaging GmbH.                                                     //
//                                                                           //
//  IDS cannot assume any responsibility for the use or misuse of any        //
//  portion of this software for other than its intended diagnostic purpose  //
//  in calibrating and testing IDS manufactured cameras and software.        //
//                                                                           //
//===========================================================================//

#include "stdafx.h"
#include "IdsSimpleLive.h"
#include "IdsSimpleLiveDlg.h"
#include ".\idssimplelivedlg.h"
#include <fstream>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <string>
#include <filesystem>
#include "TimerParam.h"
#include "toSave.h"
#include <chrono>
#include "ueye.h"

namespace fs = std::filesystem;

extern CIdsSimpleLiveApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIdsSimpleLiveDlg dialog

CIdsSimpleLiveDlg::CIdsSimpleLiveDlg(CWnd* pParent /*=NULL*/)
: CDialog(CIdsSimpleLiveDlg::IDD, pParent)
, m_cnNumberOfSeqMemory(3)
{
    //{{AFX_DATA_INIT(CIdsSimpleLiveDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIdsSimpleLiveDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_GAIN, editGain);
    DDX_Control(pDX, IDC_EDIT_FPS, editFps);
    DDX_Control(pDX, IDC_EDIT_LENGTH, editLength);
    DDX_Control(pDX, IDC_EDIT_PREFIX, editPrefix);
}

std::string CIdsSimpleLiveDlg::GetGainStr()
{
	CString cs;
	GetDlgItemText(IDC_EDIT_GAIN, cs);
	CT2CA converted(cs);
	std::string strStd(converted);
	return strStd;
}

std::string CIdsSimpleLiveDlg::GetFPSStr()
{
  	CString cs;
	GetDlgItemText(IDC_EDIT_FPS, cs);
	CT2CA converted(cs);
	std::string strStd(converted);
	return strStd;
}

std::string CIdsSimpleLiveDlg::GetLengthStr()
{
	CString cs;
	GetDlgItemText(IDC_EDIT_LENGTH, cs);
	CT2CA converted(cs);
	std::string strStd(converted);
	return strStd;
}

std::string CIdsSimpleLiveDlg::GetLongLengthStr()
{
    CString cs;
    GetDlgItemText(IDC_EDIT_LENGTH2, cs);
    CT2CA converted(cs);
    std::string strStd(converted);
    return strStd;
}

void CIdsSimpleLiveDlg::SetGainStr(double x)
{
	CString str;
	str.Format(_T("%g"),x);
	SetDlgItemText(IDC_EDIT_GAIN, str);
}

void CIdsSimpleLiveDlg::SetFPSStr(double x)
{
	CString str;
	str.Format(_T("%g"), x);
	SetDlgItemText(IDC_EDIT_FPS, str);
}

void CIdsSimpleLiveDlg::SetLengthStr(double x)
{
	CString str;
	str.Format(_T("%g"), x);
	SetDlgItemText(IDC_EDIT_LENGTH, str);
}

std::string CIdsSimpleLiveDlg::GetPrefixStr()
{
    CString prefix;
    GetDlgItemText(IDC_EDIT_PREFIX, prefix);
    CT2CA converted(prefix);
    std::string strStd(converted);
    return strStd;
}

void CIdsSimpleLiveDlg::SetWidth(int width)
{
	WIDTH = width;
}

void CIdsSimpleLiveDlg::SetHeight(int height)
{
	HEIGHT = height;
}

int CIdsSimpleLiveDlg::GetWidth()
{
	return WIDTH;
}

int CIdsSimpleLiveDlg::GetHeight()
{
	return HEIGHT;
}

BEGIN_MESSAGE_MAP(CIdsSimpleLiveDlg, CDialog)
    //{{AFX_MSG_MAP(CIdsSimpleLiveDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
    ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
    ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
    ON_MESSAGE(IS_UEYE_MESSAGE, OnUEyeMessage)
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_LOAD_PARAMETER, OnBnClickedButtonLoadParameter)
    ON_WM_CLOSE()
	ON_STN_CLICKED(IDC_DISPLAY, &CIdsSimpleLiveDlg::OnStnClickedDisplay)
    ON_BN_CLICKED(IDC_BUTTON_LISTEN, &CIdsSimpleLiveDlg::OnBnClickedButtonListen)
    ON_BN_CLICKED(IDC_BUTTON_LOAD_PARAMETER2, &CIdsSimpleLiveDlg::OnBnClickedLongCapture)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdsSimpleLiveDlg message handlers

void CIdsSimpleLiveDlg::OnOK(void)
{
	CWnd* pWnd = GetFocus();
	if (GetDlgItem(IDOK) == pWnd)
	{
		CDialog::OnOK();
		return;
	}

	OnButtonStart();
}

BOOL CIdsSimpleLiveDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // will only be enabled if in stop mode
    //GetDlgItem(IDC_BUTTON_LOAD_PARAMETER)->EnableWindow(FALSE);   CHANGED

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE); // Set big icon
    SetIcon(m_hIcon, FALSE); // Set small icon

    // get handle to display window
    m_hWndDisplay = GetDlgItem( IDC_DISPLAY )->m_hWnd;

    m_vecMemory.reserve(m_cnNumberOfSeqMemory);
    m_vecMemory.resize(m_cnNumberOfSeqMemory);

    m_hCam = 0;
    m_nRenderMode = IS_RENDER_FIT_TO_WINDOW;
    m_nPosX = 0;
    m_nPosY = 0;
    m_nFlipHor = 0;
    m_nFlipVert = 0;

	// load parameters from config and display them
	double duration = 5000;
	double fps = 1000;
	double gain = 400;
	int width = 200;
	int height = 200;

	std::ifstream f;
	f.open("config.txt");
	f >> duration;
	f >> fps;
	f >> gain;
	f.close();

	SetLengthStr(duration);
	SetFPSStr(fps);
	SetGainStr(gain);
	SetWidth(width);
	SetHeight(height);
	
    // open a camera handle
    OpenCamera();

    return true;
}


void CIdsSimpleLiveDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    CDialog::OnSysCommand(nID, lParam);
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIdsSimpleLiveDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIdsSimpleLiveDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}


///////////////////////////////////////////////////////////////////////////////
//
// METHOD CIdsSimpleLiveDlg::OnButtonStart()
//
// DESCRIPTION: start live display and return immediately
//
///////////////////////////////////////////////////////////////////////////////
void CIdsSimpleLiveDlg::OnButtonStart()
{
    if( m_hCam == 0 )
        OpenCamera();

    if( m_hCam != 0 )
    {
		ExitCamera();
		OpenCamera();
		double duration, fps, gain;
		duration = std::stod(CIdsSimpleLiveDlg::GetLengthStr());
		fps = std::stod(CIdsSimpleLiveDlg::GetFPSStr());
		gain = std::stod(CIdsSimpleLiveDlg::GetGainStr());

		std::ofstream o;
		o.open("config.txt");
		o << duration << std::endl;
		o << fps << std::endl;
		o << gain;
		o.close();

        // Capture live video
        is_CaptureVideo( m_hCam, IS_WAIT );
		is_SetHWGainFactor(m_hCam, IS_SET_MASTER_GAIN_FACTOR, gain);
		gain = is_SetHWGainFactor(m_hCam, IS_GET_MASTER_GAIN_FACTOR, gain);
		double exposure = 1000 / fps;

		CIdsSimpleLiveDlg::SetGainStr(gain);
		is_Exposure(m_hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, &exposure, 8);       
	  }
	std::cout << "Done";
}


///////////////////////////////////////////////////////////////////////////////
//
// METHOD CIdsSimpleLiveDlg::OnButtonStop()
//
// DESCRIPTION: stop live display and return immediately
//
///////////////////////////////////////////////////////////////////////////////
void CIdsSimpleLiveDlg::OnButtonStop()
{
    // stop live immediately - force stop
    if( m_hCam != 0 )
        is_StopLiveVideo( m_hCam, IS_FORCE_VIDEO_STOP );

    // Enable button 'Load Parameters'
    //GetDlgItem(IDC_BUTTON_LOAD_PARAMETER)->EnableWindow(TRUE);  CHANGED
}

///////////////////////////////////////////////////////////////////////////////
//
// METHOD CIdsSimpleLiveDlg::OnBnClickedButtonLoadParameter()
//
// DESCRIPTION: - loads parameters from an ini file
//              - reallocates the memory
//
///////////////////////////////////////////////////////////////////////////////

void CIdsSimpleLiveDlg::OnBnClickedButtonLoadParameter()
{
    if ( m_hCam == 0 )
        OpenCamera();

	if (m_hCam != 0)
	{
        CreateDirectoryA("output", NULL);
        double duration;
		double fps;
		double gain;

		duration = std::stod(CIdsSimpleLiveDlg::GetLengthStr());
		fps = std::stod(CIdsSimpleLiveDlg::GetFPSStr());
		gain = std::stod(CIdsSimpleLiveDlg::GetGainStr());

		std::ofstream o;
		o.open("config.txt");
		o << duration << std::endl;
		o << fps << std::endl;
		o << gain;
		o.close();

		is_SetFrameRate(m_hCam, fps, &fps);

		//CIdsSimpleLiveDlg::SetLengthStr(duration);
		CIdsSimpleLiveDlg::SetFPSStr(fps);
		//CIdsSimpleLiveDlg::SetGainStr(gain);

		int frameCount = duration * fps / 1000;
		int frameCount1 = 1 + 1.1 * duration * fps / 1000;
		double duration1 = 1.1 * duration;

		std::ofstream f;
		f.open("output.txt");
		std::time_t stamp = std::time(nullptr);
		std::string namestr = std::ctime(&stamp);
		namestr.erase(std::remove(namestr.begin(), namestr.end(), ' '), namestr.end());
		namestr.erase(std::remove(namestr.begin(), namestr.end(), ':'), namestr.end());
		namestr.erase(std::remove(namestr.begin(), namestr.end(), '\n'), namestr.end());
        std::string month = "00";
        if (namestr.substr(3, 3) == "Jan") {
            month = "01";
        }
        else if (namestr.substr(3, 3) == "Feb") {
            month = "02";
        }
        else if (namestr.substr(3, 3) == "Mar") {
            month = "03";
        }
        else if (namestr.substr(3, 3) == "Apr") {
            month = "04";
        }
        else if (namestr.substr(3, 3) == "May") {
            month = "05";
        }
        else if (namestr.substr(3, 3) == "Jun") {
            month = "06";
        }
        else if (namestr.substr(3, 3) == "Jul") {
            month = "07";
        }
        else if (namestr.substr(3, 3) == "Aug") {
            month = "08";
        }
        else if (namestr.substr(3, 3) == "Sep") {
            month = "09";
        }
        else if (namestr.substr(3, 3) == "Oct") {
            month = "10";
        }
        else if (namestr.substr(3, 3) == "Nov") {
            month = "11";
        }
        else if (namestr.substr(3, 3) == "Dec") {
            month = "12";
        }
        namestr = namestr.substr(6,2) + "_" + month + "_" + namestr.substr(14,4);
		f << namestr + ".bin";
		f.close();

		is_StopLiveVideo(m_hCam, IS_DONT_WAIT);

		char** ppcImageMem = new char* [frameCount1];
		INT* pid = new INT[frameCount1];
		INT width = GetWidth();
		INT height = GetHeight();
		INT depth = 10;

		for (int i = 0; i < frameCount1; i++)
		{
			is_AllocImageMem(m_hCam, width, height, depth, &(ppcImageMem[i]), &(pid[i]));
			is_AddToSequence(m_hCam, ppcImageMem[i], pid[i]);
			is_SetAllocatedImageMem(m_hCam, width, height, depth, ppcImageMem[i], &(pid[i]));
		}

		int diff;

		bool go_on = 1;

		SYSTEMTIME starttime;
		SYSTEMTIME time;
		GetSystemTime(&starttime);

		is_SetFrameRate(m_hCam, fps, &fps);
		double exposure = 0;

		//is_SetHWGainFactor(m_hCam, IS_SET_MASTER_GAIN_FACTOR, gain);
		is_Exposure(m_hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, &exposure, 8);

		is_CaptureVideo(m_hCam, IS_DONT_WAIT);
		while (go_on)
		{
			GetSystemTime(&time);
            diff = (time.wHour - starttime.wHour) * 3600.0 * 1.0e3 + (time.wMinute - starttime.wMinute) * 60.0 * 1.0e3 + (time.wSecond - starttime.wSecond) * 1.0e3 + (time.wMilliseconds - starttime.wMilliseconds);
			if (diff > duration1)
			{
				go_on = 0;
			}
			else if (diff < 0)
			{
				go_on = 0;
			}
		}

		is_StopLiveVideo(m_hCam, IS_DONT_WAIT);
		int size = (width * int((depth + 7) / 8)) * height;
        int filecount = 0;
        std::string path = "output/";
        for (const auto& entry : fs::directory_iterator(path))
            filecount++;

        

		int intwidth = CIdsSimpleLiveDlg::WIDTH;
		int intheight = CIdsSimpleLiveDlg::HEIGHT;
        std::wstring dir = L"output";
        std::string prefix = CIdsSimpleLiveDlg::GetPrefixStr();
        std::ofstream binary_stream("output/" + std::to_string(filecount) + "_" + prefix + "_" + namestr + ".bin", std::ios::binary);
        binary_stream.write(reinterpret_cast<char const*>(&intwidth), 8);
        binary_stream.write(reinterpret_cast<char const*>(&intheight), 8);
        binary_stream.write(reinterpret_cast<char const*>(&fps), 8);
        
        for (int i = 0; i < frameCount; i++)
            {
               
                
                binary_stream.write(reinterpret_cast<char const*>(ppcImageMem[i]), size);
                
            }
        binary_stream.close();
		FreeImageMems();
		OnButtonStart();
    }
}



///////////////////////////////////////////////////////////////////////////////
//
// METHOD CIdsSimpleLiveDlg::OnButtonExit()
//
// DESCRIPTION: - stop live display
//              - frees used image memory
//              - exit the camera
//              - quit application
//
///////////////////////////////////////////////////////////////////////////////
void CIdsSimpleLiveDlg::OnButtonExit()
{
    ExitCamera();
    PostQuitMessage( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
// METHOD CIdsSimpleLiveDlg::OpenCamera()
//
// DESCRIPTION: - Opens a handle to a connected camera
//
///////////////////////////////////////////////////////////////////////////////
bool CIdsSimpleLiveDlg::OpenCamera()
{
    INT nRet = IS_NO_SUCCESS;
    ExitCamera();

    // init camera (open next available camera)
    m_hCam = (HIDS) 0;
    nRet = InitCamera(&m_hCam, m_hWndDisplay);
    if (nRet == IS_SUCCESS)
    {
        // Get sensor info
        is_GetSensorInfo(m_hCam, &m_sInfo);

        GetMaxImageSize(&m_nSizeX, &m_nSizeY);


        UpdateData(TRUE);
        nRet = InitDisplayMode();

        if (nRet == IS_SUCCESS)
        {
            // Enable Messages
            is_EnableMessage(m_hCam, IS_DEVICE_REMOVED, GetSafeHwnd());
            is_EnableMessage(m_hCam, IS_DEVICE_RECONNECTED, GetSafeHwnd());
            is_EnableMessage(m_hCam, IS_FRAME, GetSafeHwnd());

            // start live video
            is_CaptureVideo( m_hCam, IS_WAIT );

			int duration = 2500;
			double fps;
			int gain;

			std::ifstream f;
			f.open("config.txt");
			f >> duration;
			f >> fps;
			f >> gain;
			f.close();

			double defaultFps = 30;

			is_SetFrameRate(m_hCam, defaultFps, &defaultFps);

			double exposure = 1000/fps;

			is_SetHWGainFactor(m_hCam, IS_SET_MASTER_GAIN_FACTOR, gain);
			is_Exposure(m_hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, &exposure, 8);


        }
        else
            AfxMessageBox(TEXT("Initializing the display mode failed!"), MB_ICONWARNING );

        return true;
    }
    else
    {
        AfxMessageBox(TEXT("No uEye camera could be opened !"), MB_ICONWARNING );
        return false;
    }
}



///////////////////////////////////////////////////////////////////////////////
//
// METHOD CIdsSimpleLiveDlg::OnUEyeMessage()
//
// DESCRIPTION: - handles the messages from the uEye camera
//              - messages must be enabled using is_EnableMessage()
//
///////////////////////////////////////////////////////////////////////////////
LRESULT CIdsSimpleLiveDlg::OnUEyeMessage( WPARAM wParam, LPARAM lParam )
{
    switch ( wParam )
    {
    case IS_DEVICE_REMOVED:
        Beep( 400, 50 );
        break;
    case IS_DEVICE_RECONNECTED:
        Beep( 400, 50 );
        break;
    case IS_FRAME:
        {
            char* pLast = NULL;
            INT lMemoryId = 0;
            INT lSequenceId = 0;

            if (IS_SUCCESS == GetLastMem(&pLast, lMemoryId, lSequenceId))
            {
                INT nRet = is_LockSeqBuf(m_hCam, IS_IGNORE_PARAMETER, pLast);

                if (IS_SUCCESS == nRet)
                {
                    is_RenderBitmap(m_hCam, lMemoryId, m_hWndDisplay, m_nRenderMode);
                    is_UnlockSeqBuf(m_hCam, IS_IGNORE_PARAMETER, pLast);
                }
            }
        }

        break;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// METHOD CIdsSimpleLiveDlg::ExitCamera()
//
// DESCRIPTION: - exits the instance of the camera
//
///////////////////////////////////////////////////////////////////////////////
void CIdsSimpleLiveDlg::ExitCamera()
{
    if( m_hCam != 0 )
    {
        // Disable messages
        is_EnableMessage( m_hCam, IS_FRAME, NULL );

        // Stop live video
        is_StopLiveVideo( m_hCam, IS_WAIT );

        ClearSequence();
        FreeImageMems();

        // Close camera
        is_ExitCamera( m_hCam );
        m_hCam = NULL;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// METHOD CIdsSimpleLiveDlg::InitDisplayMode()
//
// DESCRIPTION: - initializes the display mode
//
///////////////////////////////////////////////////////////////////////////////
int CIdsSimpleLiveDlg::InitDisplayMode()
{
    INT nRet = IS_NO_SUCCESS;

    if (m_hCam == NULL)
        return IS_NO_SUCCESS;

    ClearSequence();
    FreeImageMems();

    // Set display mode to DIB
    nRet = is_SetDisplayMode(m_hCam, IS_SET_DM_DIB);
    
    m_nColorMode = IS_CM_MONO10;
    m_nBitsPerPixel = 10;
    
	int width = GetWidth();
	int height = GetHeight();
    
	nRet = AllocImageMems(width, height, m_nBitsPerPixel);

    if (nRet == IS_SUCCESS)
    {
        InitSequence();

        // set the desired color mode
        is_SetColorMode(m_hCam, m_nColorMode);

        // set the image size to capture
        //IS_SIZE_2D imageSize;
        //imageSize.s32Width = m_nSizeX;
        //imageSize.s32Width = WIDTH;
        //imageSize.s32Height = m_nSizeY;
        //imageSize.s32Height = HEIGHT;

		IS_RECT rectAOI;

		rectAOI.s32Height = height;
		rectAOI.s32Width = width;
		rectAOI.s32X = (m_nSizeX - width)/2;
		rectAOI.s32Y = (m_nSizeY - height)/2;

		//is_SetSubSampling(m_hCam, IS_SUBSAMPLING_2X_VERTICAL | IS_SUBSAMPLING_2X_HORIZONTAL);

        is_AOI(m_hCam, IS_AOI_IMAGE_SET_AOI, (void*)&rectAOI, sizeof(rectAOI));

		//is_SetSubSampling(m_hCam, IS_SUBSAMPLING_2X_VERTICAL | IS_SUBSAMPLING_2X_HORIZONTAL);
    }
    else
    {
        AfxMessageBox(TEXT("Memory allocation failed!"), MB_ICONWARNING);
    }

    return nRet;
}


void CIdsSimpleLiveDlg::OnClose()
{
    ExitCamera();
    CDialog::OnClose();
}



INT CIdsSimpleLiveDlg::InitCamera (HIDS *hCam, HWND hWnd)
{
    INT nRet = is_InitCamera (hCam, hWnd);
    /************************************************************************************************/
    /*                                                                                              */
    /*  If the camera returns with "IS_STARTER_FW_UPLOAD_NEEDED", an upload of a new firmware       */
    /*  is necessary. This upload can take several seconds. We recommend to check the required      */
    /*  time with the function is_GetDuration().                                                    */
    /*                                                                                              */
    /*  In this case, the camera can only be opened if the flag "IS_ALLOW_STARTER_FW_UPLOAD"        */
    /*  is "OR"-ed to m_hCam. This flag allows an automatic upload of the firmware.                 */
    /*                                                                                              */
    /************************************************************************************************/
    if (nRet == IS_STARTER_FW_UPLOAD_NEEDED)
    {
        // Time for the firmware upload = 25 seconds by default
        INT nUploadTime = 25000;
        is_GetDuration (*hCam, IS_STARTER_FW_UPLOAD, &nUploadTime);

        CString Str1, Str2, Str3;
        Str1 = "This camera requires a new firmware. The upload will take about";
        Str2 = "seconds. Please wait ...";
        Str3.Format (L"%s %d %s", Str1, nUploadTime / 1000, Str2);
        AfxMessageBox (Str3, MB_ICONWARNING);

        // Set mouse to hourglass
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

        // Try again to open the camera. This time we allow the automatic upload of the firmware by
        // specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
        *hCam = (HIDS) (((INT)*hCam) | IS_ALLOW_STARTER_FW_UPLOAD);
        nRet = is_InitCamera (hCam, hWnd);
    }

    return nRet;
}


void CIdsSimpleLiveDlg::GetMaxImageSize(INT *pnSizeX, INT *pnSizeY)
{
    // Check if the camera supports an arbitrary AOI
    // Only the ueye xs does not support an arbitrary AOI
    INT nAOISupported = 0;
    BOOL bAOISupported = TRUE;
    if (is_ImageFormat(m_hCam,
        IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED,
        (void*)&nAOISupported,
        sizeof(nAOISupported)) == IS_SUCCESS)
    {
        bAOISupported = (nAOISupported != 0);
    }

    if (bAOISupported)
    {
        // All other sensors
        // Get maximum image size
        SENSORINFO sInfo;
        is_GetSensorInfo (m_hCam, &sInfo);
        *pnSizeX = sInfo.nMaxWidth;
        *pnSizeY = sInfo.nMaxHeight;
    }
    else
    {
        // Only ueye xs
        // Get image size of the current format
        IS_SIZE_2D imageSize;
        is_AOI(m_hCam, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

        *pnSizeX = imageSize.s32Width;
        *pnSizeY = imageSize.s32Height;
    }
}

INT CIdsSimpleLiveDlg::AllocImageMems(INT nSizeX, INT nSizeY, INT nBitsPerPixel)
{
    INT nRet = IS_SUCCESS;

    for (MemoryVector::iterator iter = m_vecMemory.begin(); iter != m_vecMemory.end(); ++iter)
    {
        nRet = is_AllocImageMem(m_hCam, nSizeX, nSizeY, nBitsPerPixel, &(iter->pcImageMemory), &(iter->lMemoryId));

        if (IS_SUCCESS != nRet)
        {
            FreeImageMems();
            break;
        }
    }

    return nRet;
}

INT CIdsSimpleLiveDlg::FreeImageMems(void)
{
    INT nRet = IS_SUCCESS;

    // Free the allocated buffer
    for (MemoryVector::iterator iter = m_vecMemory.begin(); iter != m_vecMemory.end(); ++iter)
    {
        if (NULL != iter->pcImageMemory)
        {
            nRet = is_FreeImageMem(m_hCam, iter->pcImageMemory, iter->lMemoryId);
        }

        iter->pcImageMemory = NULL;
        iter->lMemoryId = 0;
        iter->lSequenceId = 0;
    }

    return nRet;
}

INT CIdsSimpleLiveDlg::InitSequence(void)
{
    INT nRet = IS_SUCCESS;

    int i = 0;
    for (MemoryVector::iterator iter = m_vecMemory.begin(); iter != m_vecMemory.end(); ++iter, i++)
    {
        nRet = is_AddToSequence(m_hCam, iter->pcImageMemory, iter->lMemoryId);

        if (IS_SUCCESS != nRet)
        {
            ClearSequence();
            break;
        }

        iter->lSequenceId = i + 1;
    }

    return nRet;
}

INT CIdsSimpleLiveDlg::ClearSequence(void)
{
    return is_ClearSequence(m_hCam);
}

INT CIdsSimpleLiveDlg::GetLastMem(char** ppLastMem, INT& lMemoryId, INT& lSequenceId)
{
    INT nRet = IS_NO_SUCCESS;

    int dummy = 0;
    char *pLast = NULL;
    char *pMem = NULL;

    nRet = is_GetActSeqBuf(m_hCam, &dummy, &pMem, &pLast);

    if ((IS_SUCCESS == nRet) && (NULL != pLast))
    {
        nRet = IS_NO_SUCCESS;

        for (MemoryVector::iterator iter = m_vecMemory.begin(); iter != m_vecMemory.end(); ++iter)
        {
            if (pLast == iter->pcImageMemory)
            {
                *ppLastMem = iter->pcImageMemory;
                lMemoryId = iter->lMemoryId;
                lSequenceId = iter->lSequenceId;
                nRet = IS_SUCCESS;

                break;
            }
        }
    }

    return nRet;
}

void CIdsSimpleLiveDlg::OnStnClickedDisplay()
{
	// TODO: Add your control notification handler code here
}


void CIdsSimpleLiveDlg::OnBnClickedButtonListen()
{
    int go_on = 1;
    int state;
    UINT nCommand = IS_IO_CMD_GPIOS_GET_CONFIGURATION;

    IO_GPIO_CONFIGURATION gpioConfiguration;

    gpioConfiguration.u32Gpio = IO_GPIO_1;
    gpioConfiguration.u32Configuration = IS_GPIO_OUTPUT;
    //gpioConfiguration.u32State = 0;
    INT nRet = IS_SUCCESS;
    while (go_on)
    {
        nRet = is_IO(m_hCam, nCommand, (void*)&gpioConfiguration, sizeof(gpioConfiguration));
        if (gpioConfiguration.u32State == 0)
        {
            go_on = 0;
        }
        
    }
    OnBnClickedButtonLoadParameter();
}

UINT WriteThread(LPVOID pParam)
{
    toSave* pSave = (toSave*)pParam;
    std::wstring dir = L"output";
    //std::string prefix = CIdsSimpleLiveDlg::GetPrefixStr();
    std::ofstream binary_stream(pSave->final_namestr, std::ios::binary);
    binary_stream.write(reinterpret_cast<char const*>(&(pSave->width)), 8);
    binary_stream.write(reinterpret_cast<char const*>(&(pSave->height)), 8);
    binary_stream.write(reinterpret_cast<char const*>(&(pSave->fps)), 8);
    int depth = 10;
    int size = (pSave->width * int((depth + 7) / 8)) * pSave->height;
    for (int i = pSave->start; i < pSave->length; i++)
    {
        binary_stream.write(reinterpret_cast<char const*>(pSave->ppcImageMem[i]), size);
    }
    binary_stream.close();
    return 0;   // thread completed successfully
}

// inside a different function in the program
//pNewObject = new CMyObject;
//AfxBeginThread(MyThreadProc, pNewObject);

UINT TimerThread(LPVOID pParam)
{
    TimerParam* pTimer = (TimerParam*)pParam;
    SYSTEMTIME time;
    SYSTEMTIME starttime;
    GetSystemTime(&starttime);
    //auto start = std::chrono::steady_clock::now();
    //auto end = std::chrono::steady_clock::now();

    //std::chrono::milliseconds<double> elapsed_seconds;

    int go_on = 1;
    double diff = 0;
    while (go_on) //first time wait for 1.5*duration
    {
        GetSystemTime(&time);
        diff = (time.wHour - starttime.wHour) * 60.0 + (time.wMinute - starttime.wMinute) * 60.0 + (time.wSecond - starttime.wSecond) * 1.0e3 + (time.wMilliseconds - starttime.wMilliseconds);
        if (diff > 1.5*(pTimer->duration))
        {
            go_on = 0;
        }
        else if (diff < 0)
        {
            go_on = 0;
        }
    }
    AfxBeginThread(WriteThread, pTimer->writeParamFirst);
    while (go_on) //second time wait for 1 duration
    {
        GetSystemTime(&time);
        diff = (time.wHour - starttime.wHour) * 60.0 + (time.wMinute - starttime.wMinute) * 60.0 + (time.wSecond - starttime.wSecond) * 1.0e3 + (time.wMilliseconds - starttime.wMilliseconds);
        if (diff > 1.5 * (pTimer->duration))
        {
            go_on = 0;
        }
        else if (diff < 0)
        {
            go_on = 0;
        }
    }
    AfxBeginThread(WriteThread, pTimer->writeParamSecond);
    for (int i = 1; i < pTimer->count; i++) {
        go_on = 1;
        while (go_on)
        {
            GetSystemTime(&time);
            diff = (time.wHour - starttime.wHour) * 60.0 + (time.wMinute - starttime.wMinute) * 60.0 + (time.wSecond - starttime.wSecond) * 1.0e3 + (time.wMilliseconds - starttime.wMilliseconds);
            if (diff > pTimer->duration)
            {
                go_on = 0;
            }
            else if (diff < 0)
            {
                go_on = 0;
            }
        }
        AfxBeginThread(WriteThread, pTimer->writeParamFirst);
        go_on = 1;
        while (go_on)
        {
            GetSystemTime(&time);
            diff = (time.wHour - starttime.wHour) * 60.0 + (time.wMinute - starttime.wMinute) * 60.0 + (time.wSecond - starttime.wSecond) * 1.0e3 + (time.wMilliseconds - starttime.wMilliseconds);
            if (diff > pTimer->duration)
            {
                go_on = 0;
            }
            else if (diff < 0)
            {
                go_on = 0;
            }
        }
        AfxBeginThread(WriteThread, pTimer->writeParamSecond);
    }
    //FreeImageMems();
    //OnButtonStart();
    return 0;
}

void CIdsSimpleLiveDlg::OnBnClickedLongCapture()
{
    if (m_hCam == 0)
        OpenCamera();

    if (m_hCam != 0)
    {
        CreateDirectoryA("output", NULL);
        double duration;
        double fps;
        double gain;

        duration = std::stod(CIdsSimpleLiveDlg::GetLongLengthStr());
        fps = std::stod(CIdsSimpleLiveDlg::GetFPSStr());
        gain = std::stod(CIdsSimpleLiveDlg::GetGainStr());

        is_SetFrameRate(m_hCam, fps, &fps);

        //CIdsSimpleLiveDlg::SetLengthStr(duration);
        CIdsSimpleLiveDlg::SetFPSStr(fps);
        //CIdsSimpleLiveDlg::SetGainStr(gain);

        //int i = 0; //buffer counter

        int bufferFrameCount = 500;

        int buffCount = (duration * fps + bufferFrameCount - 1) / bufferFrameCount; //quick ceiling divide

        float halfBufferDuration = (bufferFrameCount / static_cast<float>(2)) * (1000 /fps);

        char** buffer = new char* [bufferFrameCount];
        INT* pid = new INT[bufferFrameCount];
        INT width = GetWidth();
        INT height = GetHeight();
        INT depth = 10;

        for (int i = 0; i < bufferFrameCount; i++)
        {
            is_AllocImageMem(m_hCam, width, height, depth, &(buffer[i]), &(pid[i]));
            is_AddToSequence(m_hCam, buffer[i], pid[i]);
            is_SetAllocatedImageMem(m_hCam, width, height, depth, buffer[i], &(pid[i]));
        }

        std::ofstream f;
        f.open("output.txt");
        std::time_t stamp = std::time(nullptr);
        std::string namestr = std::ctime(&stamp);
        namestr.erase(std::remove(namestr.begin(), namestr.end(), ' '), namestr.end());
        namestr.erase(std::remove(namestr.begin(), namestr.end(), ':'), namestr.end());
        namestr.erase(std::remove(namestr.begin(), namestr.end(), '\n'), namestr.end());
        std::string month = "00";
        if (namestr.substr(3, 3) == "Jan") {
            month = "01";
        }
        else if (namestr.substr(3, 3) == "Feb") {
            month = "02";
        }
        else if (namestr.substr(3, 3) == "Mar") {
            month = "03";
        }
        else if (namestr.substr(3, 3) == "Apr") {
            month = "04";
        }
        else if (namestr.substr(3, 3) == "May") {
            month = "05";
        }
        else if (namestr.substr(3, 3) == "Jun") {
            month = "06";
        }
        else if (namestr.substr(3, 3) == "Jul") {
            month = "07";
        }
        else if (namestr.substr(3, 3) == "Aug") {
            month = "08";
        }
        else if (namestr.substr(3, 3) == "Sep") {
            month = "09";
        }
        else if (namestr.substr(3, 3) == "Oct") {
            month = "10";
        }
        else if (namestr.substr(3, 3) == "Nov") {
            month = "11";
        }
        else if (namestr.substr(3, 3) == "Dec") {
            month = "12";
        }
        namestr = "output/" + namestr.substr(6, 2) + "_" + month + "_" + namestr.substr(14, 4) + ".bin";
        f << namestr;
        f.close();

        is_StopLiveVideo(m_hCam, IS_DONT_WAIT);

        is_SetFrameRate(m_hCam, fps, &fps);
        double exposure = 0;

        //is_SetHWGainFactor(m_hCam, IS_SET_MASTER_GAIN_FACTOR, gain);
        is_Exposure(m_hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, &exposure, 8);
        toSave* writeParam_first = new toSave(buffer, 0, bufferFrameCount / 2, width, height, fps, namestr);
        toSave* writeParam_second = new toSave(buffer, bufferFrameCount / 2, bufferFrameCount, width, height, fps, namestr);
        TimerParam* ptimerParam = new TimerParam(halfBufferDuration, buffCount, writeParam_first, writeParam_second);

        is_CaptureVideo(m_hCam, IS_DONT_WAIT);
        AfxBeginThread(TimerThread, ptimerParam);
        is_StopLiveVideo(m_hCam, IS_DONT_WAIT);
        //AfxBeginThread(WriteThread, pTimer->writeParamSecond);
    }
}
