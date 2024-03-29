/*
Module : SerialPort.h
Purpose: Interface for a set of C++ wrapper classes for serial ports
Created: PJN / 31-05-1999
History: PJN / 03-06-1999 1. Fixed problem with code using CancelIo which does not exist on 95.
                          2. Fixed leaks which can occur in sample app when an exception is thrown
         PJN / 16-06-1999 1. Fixed a bug whereby CString::ReleaseBuffer was not being called in 
                             CSerialException::GetErrorMessage
         PJN / 29-09-1999 1. Fixed a simple copy and paste bug in CSerialPort::SetDTR
         PJN / 08-05-2000 1. Fixed an unreferrenced variable in CSerialPort::GetOverlappedResult in VC 6
         PJN / 10-12-2000 1. Made class destructor virtual
         PJN / 15-01-2001 1. Attach method now also allows you to specify whether the serial port
                          is being attached to in overlapped mode
                          2. Removed some ASSERTs which were unnecessary in some of the functions
                          3. Updated the Read method which uses OVERLAPPED IO to also return the bytes
                          read. This allows calls to WriteFile with a zeroed overlapped structure (This
                          is required when dealing with TAPI and serial communications)
                          4. Now includes copyright message in the source code and documentation.
         PJN / 24-03-2001 1. Added a BytesWaiting method
         PJN / 04-04-2001 1. Provided an overriden version of BytesWaiting which specifies a timeout
         PJN / 23-04-2001 1. Fixed a memory leak in DataWaiting method
         PJN / 01-05-2002 1. Fixed a problem in Open method which was failing to initialize the DCB 
                          structure incorrectly, when calling GetState. Thanks to Ben Newson for this fix.
         PJN / 29-05-2002 1. Fixed an problem where the GetProcAddress for CancelIO was using the
                          wrong calling convention
         PJN / 07-08-2002 1. Changed the declaration of CSerialPort::WaitEvent to be consistent with the
                          rest of the methods in CSerialPort which can operate in "OVERLAPPED" mode. A note
                          about the usage of this: If the method succeeds then the overlapped operation
                          has completed synchronously and there is no need to do a WaitForSingle/MultipleObjects.
                          If any other unexpected error occurs then a CSerialException will be thrown. See
                          the implementation of the CSerialPort::DataWaiting which has been rewritten to use
                          this new design pattern. Thanks to Serhiy Pavlov for spotting this inconsistency.
         PJN / 20-09-2002 1. Addition of an additional ASSERT in the internal _OnCompletion function.
                          2. Addition of an optional out parameter to the Write method which operates in 
                          overlapped mode. Thanks to Kevin Pinkerton for this addition.
         PJN / 10-04-2006 1. Updated copyright details.
                          2. Addition of a CSERIALPORT_EXT_CLASS and CSERIALPORT_EXT_API macros which makes 
                          the class easier to use in an extension dll.
                          3. Removed derivation of CSerialPort from CObject as it was not really needed.
                          4. Fixed a number of level 4 warnings in the sample app.
                          5. Reworked the overlapped IO methods to expose the LPOVERLAPPED structure to client 
                          code.
                          6. Updated the documentation to use the same style as the web site.
                          7. Did a spell check of the HTML documentation.
                          8. Updated the documentation on possible blocking in Read/Ex function. Thanks to 
                          D Kerrison for reporting this issue.
                          9. Fixed a minor issue in the sample app when the code is compiled using /Wp64
         PJN / 02-06-2006 1. Removed the bOverlapped as a member variable from the class. There was no 
                          real need for this setting, since the SDK functions will perform their own 
                          checking of how overlapped operations should
                          2. Fixed a bug in GetOverlappedResult where the code incorrectly checking against
                          the error ERROR_IO_PENDING instead of ERROR_IO_INCOMPLETE. Thanks to Sasho Darmonski
                          for reporting this bug.
                          3. Reviewed all TRACE statements for correctness.
         PJN / 05-06-2006 1. Fixed an issue with the creation of the internal event object. It was incorrectly
                          being created as an auto-reset event object instead of a manual reset event object.
                          Thanks to Sasho Darmonski for reporting this issue.
         PJN / 24-06-2006 1. Fixed some typos in the history list. Thanks to Simon Wong for reporting this.
                          2. Made the class which handles the construction of function pointers at runtime a
                          member variable of CSerialPort
                          3. Made AfxThrowSerialPortException part of the CSerialPort class. Thanks to Simon Wong
                          for reporting this.
                          4. Removed the unnecessary CSerialException destructor. Thanks to Simon Wong for 
                          reporting this.
                          5. Fixed a minor error in the TRACE text in CSerialPort::SetDefaultConfig. Again thanks
                          to Simon Wong for reporting this. 
                          6. Code now uses new C++ style casts rather than old style C casts where necessary. 
                          Again thanks to Simon Wong for reporting this.
                          7. CSerialException::GetErrorMessage now uses the strsafe functions. This does mean 
                          that the code now requires the Platform SDK if compiled using VC 6.
         PJN / 25-06-2006 1. Combined the functionality of the CSerialPortData class into the main CSerialPort class.
                          2. Renamed AfxThrowSerialPortException to ThrowSerialPortException and made the method
                          public.
         PJN / 05-11-2006 1. Minor update to stdafx.h of sample app to avoid compiler warnings in VC 2005.
                          2. Reverted the use of the strsafe.h header file. Instead now the code uses the VC 2005
                          Safe CRT and if this is not available, then we fail back to the standard CRT.
         PJN / 25-01-2007 1. Minor update to remove strsafe.h from stdafx.h of the sample app.
                          2. Updated copyright details.
         PJN / 24-12-2007 1. CSerialException::GetErrorMessage now uses the FORMAT_MESSAGE_IGNORE_INSERTS flag. For more information please see Raymond
                          Chen's blog at http://blogs.msdn.com/oldnewthing/archive/2007/11/28/6564257.aspx. Thanks to Alexey Kuznetsov for reporting this
                          issue.
                          2. Simplified the code in CSerialException::GetErrorMessage somewhat.
                          3. Optimized the CSerialException constructor code.
                          4. Code now uses newer C++ style casts instead of C style casts.
                          5. Reviewed and updated all the TRACE logging in the module
                          6. Replaced all calls to ZeroMemory with memset
         PJN / 30-12-2007 1. Updated the sample app to clean compile on VC 2008
                          2. CSerialException::GetErrorMessage now uses Checked::tcsncpy_s if compiled using VC 2005 or later.
         PJN / 18-05-2008 1. Updated copyright details.
                          2. Changed the actual values for Parity enum so that they are consistent with the Parity define values in the Windows SDK 
                          header file WinBase.h. This avoids the potential issue where you use the CSerialPort enum parity values in a call to the
                          raw Win32 API calls. Thanks to Robert Krueger for reporting this issue.
         PJN / 21-06-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
                          2. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          3. The code now only supports VC 2005 or later. 
                          4. CSerialPort::Read, Write, GetOverlappedResult & WaitEvent now throw an exception irrespective of whether the last error
                          is ERROR_IO_PENDING or not
                          5. Replaced all calls to ZeroMemory with memset
         PJN / 04-07-2008 1. Provided a version of the Open method which takes a string instead of a numeric port number value. This allows the code
                          to support some virtual serial port packages which do not use device names of the form "COM%d". Thanks to David Balazic for 
                          suggesting this addition.
         PJN / 25-01-2012 1. Updated copyright details.
                          2. Updated sample app and class to compile cleanly on VC 2010 and later.
         PJN / 28-02-2015 1. Updated sample project settings to more modern default values.
                          2. Updated copyright details.
                          3. Reworked the CSerialPort and CSerialPortException classes to optionally compile without MFC. By default 
                          the classes now use STL classes and idioms but if you define CSERIALPORT_MFC_EXTENSIONS the classes will 
                          revert back to the MFC behaviour.
                          4. Remove logic to use GetProcAddress to access CancelIO functionality.
                          5. Updated the code to clean compile on VC 2013
                          6. Added SAL annotations to all the code
                          7. Addition of a GetDefaultConfig method which takes a string
                          8. Addition of a SetDefaultConfig method which takes a string
         PJN / 26-04-2015 1. Removed unnecessary inclusion of WinError.h
                          2. Removed the CSerialPort::DataWaiting method as it depends on the port being open in overlapped mode. Instead client code
                          can simply call CSerialPort::WaitEvent directly themselves. Removing this method also means that the CSerialPort::m_hEvent
                          handle has not also been removed.
                          3. The CSerialPort::WriteEx method has been reworked to expose all the parameters of the underlying WriteFileEx API. This
                          rework also fixes a memory leak in WriteEx which can sometimes occur. This reworks also means that the 
                          CSerialPort::_OnCompletion and CSerialPort::_OnCompletion methods have been removed. Thanks to Yufeng Huang for reporting 
                          this issue.
                          4. The CSerialPort::ReadEx method has been reworked to expose all the parameters of the underlying ReadFileEx API. This
                          rework also fixes a memory leak in ReadEx which can sometimes occur. This reworks also means that the 
                          CSerialPort::_OnCompletion and CSerialPort::_OnCompletion methods have been removed. Thanks to Yufeng Huang for reporting 
                          this issue.
         PJN / 17-12-2015 1. Verified the code compiles cleanly on VC 2015.
         PJN / 22-05-2016 1. Updated copyright details.
                          2. Fixed some typos in SerialPort.h, SerialPort.cpp and SerialPort.htm where CSERIALPORT_MFC_EXTENSTIONS was being used 
                          instead of CSERIALPORT_MFC_EXTENSIONS. Thanks to Nicholas Buse for reporting this issue.
         PJN / 16-08-2017 1. Updated copyright details.
                          2. Updated declarations of CSerialException::GetErrorMessage to be consistent with CException base class implementations
                          3. Fixed up the SAL annotations on CSerialPort::GetConfig. Thanks to Wang Jinhai for reporting this issue.
         PJN / 17-12-2017 1. Replaced NULL throughout the codebase with nullptr. This means that the minimum requirement for the framework is now 
                          VC 2010.
                          2. Updated the code to compile cleanly when _ATL_NO_AUTOMATIC_NAMESPACE is defined.
                          3. Provided a CSerialPort::CancelIoEx method which encapsulates the CancelIoEx API. Thanks to Victor Derks for this 
                          suggestion.
                          4. Fixed code in CSerialPort::GetDefaultConfig as it does not have a return value. Thanks to Victor Derks for reporting
                          this bug.
                          5. Fixed code in CSerialPort::SetDefaultConfig as it does not have a return value. Thanks to Victor Derks for reporting
                          this bug.
                          6. The sal header file is now included before the fallback SAL macros are defined in SerialPort.h. Thanks to Victor Derks 
                          for reporting this issue.
                          7. Reworked the code to now be a header only implementation.
                          8. Provided a new CSerialPort2 class which is a non exception based version of CSerialPort
                          9. Provided methods in CSerialPort & CSerialPort2 which encapsulate the GetOverlappedResultEx API.
                          10. Provided a new Open method in CSerialPort & CSerialPort2 which just opens the port without explicit configuration.
                          11. Provided a new Open method in CSerialPort & CSerialPort2 which encapsulates the OpenCommPort API.

Copyright (c) 1999 - 2017 by PJ Naughter. (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


///////////////////// Macros / Structs etc ////////////////////////////////////

#pragma once

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#ifndef CSERIALPORT_EXT_CLASS
#define CSERIALPORT_EXT_CLASS
#endif //#ifndef CSERIALPORT_EXT_CLASS

#include <sal.h> //We need to pull in the sal header file before we define our own fallback versions

#ifndef _Out_writes_bytes_
#define _Out_writes_bytes_(X)
#endif //#ifndef _Out_writes_bytes_

#ifndef _Out_writes_bytes_to_opt_
#define _Out_writes_bytes_to_opt_(X,Y)
#endif //#ifndef _Out_writes_bytes_to_opt_

#ifndef _Out_writes_bytes_opt_
#define _Out_writes_bytes_opt_(X)
#endif //#ifndef _Out_writes_bytes_opt_

#ifndef _In_reads_bytes_opt_
#define _In_reads_bytes_opt_(X)
#endif //#ifndef _In_reads_bytes_opt_

#ifndef _Must_inspect_result_
#define _Must_inspect_result_
#endif //#ifndef _Must_inspect_result_

#ifndef _Return_type_success_
#define _Return_type_success_(expr)
#endif //#ifndef _Return_type_success_


////////////////////////// Includes ///////////////////////////////////////////

#ifndef CSERIALPORT_MFC_EXTENSIONS
#include <exception>
#include <string>
#endif //#ifndef CSERIALPORT_MFC_EXTENSIONS

#ifndef __ATLBASE_H__
#pragma message("To avoid this message, please put atlbase.h in your pre compiled header (normally stdafx.h)")
#include <atlbase.h>
#endif //#ifndef __ATLBASE_H__


/////////////////////////// Classes ///////////////////////////////////////////

#ifdef CSERIALPORT_MFC_EXTENSIONS
class CSERIALPORT_EXT_CLASS CSerialException : public CException
#else
class CSERIALPORT_EXT_CLASS CSerialException : public std::exception
#endif //#ifdef CSERIALPORT_MFC_EXTENSIONS
{
public:
//Constructors / Destructors
  CSerialException(_In_ DWORD dwError) : m_dwError(dwError)
  {
  }

//Methods
#ifdef CSERIALPORT_MFC_EXTENSIONS
#ifdef _DEBUG
  virtual void Dump(CDumpContext& dc) const
  {
    //Let the base class do its thing
    __super::Dump(dc);

    dc << _T("m_dwError = ") << m_dwError << _T("\n");
  }
#endif //#ifdef _DEBUG
#endif //#ifdef CSERIALPORT_MFC_EXTENSIONS

#if (_MSC_VER >= 1700)
  virtual BOOL GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError, _Out_opt_ PUINT pnHelpContext = nullptr) const
#else
  virtual BOOL GetErrorMessage(_Out_z_cap_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError, _Out_opt_ PUINT pnHelpContext = nullptr) const
#endif //#if (_MSC_VER >= 1700)
  {
    //Validate our parameters
    ATLASSERT(lpszError != nullptr);

    if (pnHelpContext != nullptr)
      *pnHelpContext = 0;

    //What will be the return value from this function (assume the worst)
    BOOL bSuccess = FALSE;
    LPTSTR lpBuffer = nullptr;
    DWORD dwReturn = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                   nullptr, m_dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
                                   reinterpret_cast<LPTSTR>(&lpBuffer), 0, nullptr);
    if (dwReturn == 0)
      *lpszError = _T('\0');
    else
    {
      bSuccess = TRUE;
      ATL::Checked::tcsncpy_s(lpszError, nMaxError, lpBuffer, _TRUNCATE);
      LocalFree(lpBuffer);
    }

    return bSuccess;
  }

#if (_MSC_VER >= 1700)
  virtual BOOL GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError, _Out_opt_ PUINT pnHelpContext = nullptr)
#else
  virtual BOOL GetErrorMessage(_Out_z_cap_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError, _Out_opt_ PUINT pnHelpContext = nullptr)
#endif //#if (_MSC_VER >= 1700)
  {
    //Call the const version of GetErrorMessage
    return const_cast<const CSerialException*>(this)->GetErrorMessage(lpszError, nMaxError, pnHelpContext);
  }

#ifdef CSERIALPORT_MFC_EXTENSIONS
  CString GetErrorMessage()
  {
    CString rVal;
    LPTSTR pstrError = rVal.GetBuffer(4096);
    GetErrorMessage(pstrError, 4096, nullptr);
    rVal.ReleaseBuffer();
    return rVal;
  }
#endif //#ifdef CSERIALPORT_MFC_EXTENSIONS

//Data members
  DWORD m_dwError;
};


//Non exception based version of the class
class CSERIALPORT_EXT_CLASS CSerialPort2
{
public:
//Enums
  enum FlowControl
  {
    NoFlowControl,
    CtsRtsFlowControl,
    CtsDtrFlowControl,
    DsrRtsFlowControl,
    DsrDtrFlowControl,
    XonXoffFlowControl
  };

  enum Parity
  {
    NoParity = 0,
    OddParity = 1,
    EvenParity = 2,
    MarkParity = 3,
    SpaceParity = 4
  };

  enum StopBits
  {
    OneStopBit,
    OnePointFiveStopBits,
    TwoStopBits
  };

//Constructors / Destructors
  CSerialPort2() : m_hComm(INVALID_HANDLE_VALUE)
  {
  }

  virtual ~CSerialPort2()
  {
    Close();
  }

//General Methods
#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)
__if_exists(OpenCommPort)
{
  BOOL Open(_In_ ULONG uPortNumber, _In_ DWORD dwDesiredAccess, _In_ DWORD dwFlagsAndAttributes)
  {
    Close(); //In case we are already open

    //Call CreateFile to open the comms port
    m_hComm = OpenCommPort(uPortNumber, dwDesiredAccess, dwFlagsAndAttributes);
    return (m_hComm != INVALID_HANDLE_VALUE);
  }
}
#endif //#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)

  BOOL Open(_In_z_ LPCTSTR pszPort, _In_ BOOL bOverlapped = FALSE)
  {
    Close(); //In case we are already open

    //Call CreateFile to open the comms port
    m_hComm = CreateFile(pszPort, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, nullptr);
    return (m_hComm != INVALID_HANDLE_VALUE);
  }

  BOOL Open(_In_ int nPort, _In_ DWORD dwBaud = 9600, _In_ Parity parity = NoParity, _In_ BYTE DataBits = 8,
            _In_ StopBits stopBits = OneStopBit, _In_ FlowControl fc = NoFlowControl, _In_ BOOL bOverlapped = FALSE)
  {
    //Form the string version of the port number
    TCHAR szPort[12];
    _stprintf_s(szPort, sizeof(szPort) / sizeof(TCHAR), _T("\\\\.\\COM%d"), nPort);

    //Delegate the work to the other version of Open
    return Open(szPort, dwBaud, parity, DataBits, stopBits, fc, bOverlapped);
  }

  BOOL Open(_In_z_ LPCTSTR pszPort, _In_ DWORD dwBaud = 9600, _In_ Parity parity = NoParity, _In_ BYTE DataBits = 8,
            _In_ StopBits stopBits = OneStopBit, _In_ FlowControl fc = NoFlowControl, _In_ BOOL bOverlapped = FALSE)
  {
    //Delegate to the other version of this method
    if (!Open(pszPort, bOverlapped))
      return FALSE;

    //Get the current state prior to changing it
    DCB dcb;
    dcb.DCBlength = sizeof(DCB);
    if (!GetState(dcb))
      return FALSE;

    //Setup the baud rate
    dcb.BaudRate = dwBaud;

    //Setup the Parity
    switch (parity)
    {
      case EvenParity:
      {
        dcb.Parity = EVENPARITY;
        break;
      }
      case MarkParity:
      {
        dcb.Parity = MARKPARITY;
        break;
      }
      case NoParity:
      {
        dcb.Parity = NOPARITY;
        break;
      }
      case OddParity:
      {
        dcb.Parity = ODDPARITY;
        break;
      }
      case SpaceParity:
      {
        dcb.Parity = SPACEPARITY;
        break;
      }
      default:
      {
        ATLASSERT(FALSE);
        break;
      }
    }

    //Setup the data bits
    dcb.ByteSize = DataBits;

    //Setup the stop bits
    switch (stopBits)
    {
      case OneStopBit:
      {
        dcb.StopBits = ONESTOPBIT;
        break;
      }
      case OnePointFiveStopBits:
      {
        dcb.StopBits = ONE5STOPBITS;
        break;
      }
      case TwoStopBits:
      {
        dcb.StopBits = TWOSTOPBITS;
        break;
      }
      default:
      {
        ATLASSERT(FALSE);
        break;
      }
    }

    //Setup the flow control
    dcb.fDsrSensitivity = FALSE;
    switch (fc)
    {
      case NoFlowControl:
      {
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        break;
      }
      case CtsRtsFlowControl:
      {
        dcb.fOutxCtsFlow = TRUE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        break;
      }
      case CtsDtrFlowControl:
      {
        dcb.fOutxCtsFlow = TRUE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        break;
      }
      case DsrRtsFlowControl:
      {
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = TRUE;
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        break;
      }
      case DsrDtrFlowControl:
      {
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = TRUE;
        dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        break;
      }
      case XonXoffFlowControl:
      {
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fOutX = TRUE;
        dcb.fInX = TRUE;
        dcb.XonChar = 0x11;
        dcb.XoffChar = 0x13;
        dcb.XoffLim = 100;
        dcb.XonLim = 100;
        break;
      }
      default:
      {
        ATLASSERT(FALSE);
        break;
      }
    }

    //Now that we have all the settings in place, make the changes
    return SetState(dcb);
  }

  void Close()
  {
    if (IsOpen())
    {
      //Close down the comms port
      CloseHandle(m_hComm);
      m_hComm = INVALID_HANDLE_VALUE;
    }
  }

  void Attach(_In_ HANDLE hComm)
  {
    Close();

    //Validate our parameters, now that the port has been closed
    ATLASSERT(m_hComm == INVALID_HANDLE_VALUE);

    m_hComm = hComm;
  }

  HANDLE Detach()
  {
    HANDLE hComm = m_hComm; //What will be the return value from this function
    m_hComm = INVALID_HANDLE_VALUE;
    return hComm;
  }

  operator HANDLE() const
  {
    return m_hComm;
  }

  BOOL IsOpen() const
  {
    return m_hComm != INVALID_HANDLE_VALUE;
  }

#ifdef CSERIALPORT_MFC_EXTENSIONS
#ifdef _DEBUG
  virtual void Dump(_In_ CDumpContext& dc) const
  {
    dc << _T("m_hComm = ") << m_hComm << _T("\n");
  }
#endif //#ifdef _DEBUG
#endif //#ifdef CSERIALPORT_MFC_EXTENSIONS

  //Reading / Writing Methods
  _Must_inspect_result_ BOOL Read(_Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(FILE) void* lpBuffer, _In_ DWORD dwNumberOfBytesToRead, _Out_ DWORD& dwBytesRead)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    dwBytesRead = 0;
    return ReadFile(m_hComm, lpBuffer, dwNumberOfBytesToRead, &dwBytesRead, nullptr);
  }

  _Must_inspect_result_ BOOL Read(_Out_writes_bytes_to_opt_(dwNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) void* lpBuffer, _In_ DWORD dwNumberOfBytesToRead, _In_ OVERLAPPED& overlapped, _Inout_opt_ DWORD* lpNumberOfBytesRead = nullptr)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return ReadFile(m_hComm, lpBuffer, dwNumberOfBytesToRead, lpNumberOfBytesRead, &overlapped);
  }

  _Must_inspect_result_ BOOL ReadEx(_Out_writes_bytes_opt_(dwNumberOfBytesToRead) __out_data_source(FILE) LPVOID lpBuffer, _In_ DWORD dwNumberOfBytesToRead, _Inout_ LPOVERLAPPED lpOverlapped, _In_ LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return ReadFileEx(m_hComm, lpBuffer, dwNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine);
  }

  BOOL Write(_In_reads_bytes_opt_(dwNumberOfBytesToWrite) const void* lpBuffer, _In_ DWORD dwNumberOfBytesToWrite, _Out_ DWORD& dwBytesWritten)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    dwBytesWritten = 0;
    return WriteFile(m_hComm, lpBuffer, dwNumberOfBytesToWrite, &dwBytesWritten, nullptr);
  }

  BOOL Write(_In_reads_bytes_opt_(dwNumberOfBytesToWrite) const void* lpBuffer, _In_ DWORD dwNumberOfBytesToWrite, _In_ OVERLAPPED& overlapped, _Out_opt_ DWORD* lpNumberOfBytesWritten = nullptr)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return WriteFile(m_hComm, lpBuffer, dwNumberOfBytesToWrite, lpNumberOfBytesWritten, &overlapped);
  }

  BOOL WriteEx(_In_reads_bytes_opt_(dwNumberOfBytesToWrite) LPCVOID lpBuffer, _In_ DWORD dwNumberOfBytesToWrite, _Inout_ LPOVERLAPPED lpOverlapped, _In_ LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return WriteFileEx(m_hComm, lpBuffer, dwNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine);
  }

  BOOL TransmitChar(_In_ char cChar)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return TransmitCommChar(m_hComm, cChar);
  }

  BOOL GetOverlappedResult(_In_ OVERLAPPED& overlapped, _Out_ DWORD& dwBytesTransferred, _In_ BOOL bWait)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return ::GetOverlappedResult(m_hComm, &overlapped, &dwBytesTransferred, bWait);
  }

__if_exists(::GetOverlappedResultEx)
{
  BOOL GetOverlappedResultEx(_In_ OVERLAPPED& overlapped, _Out_ DWORD& dwBytesTransferred, _In_ DWORD dwMilliseconds, _In_ BOOL bAlertable)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return ::GetOverlappedResultEx(m_hComm, &overlapped, &dwBytesTransferred, dwMilliseconds, bAlertable);
  }
}

  BOOL CancelIo()
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return ::CancelIo(m_hComm);
  }

#if (_WIN32_WINNT >= 0x0600)
__if_exists(::CancelIoEx)
{
  BOOL CancelIoEx(_In_opt_ LPOVERLAPPED lpOverlapped = nullptr)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return ::CancelIoEx(m_hComm, lpOverlapped);
  }
}
#endif //#if (_WIN32_WINNT >= 0x0600)

  BOOL BytesWaiting(_Out_ DWORD& dwBytesWaiting)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    //Check to see how many characters are unread
    dwBytesWaiting = 0;
    COMSTAT stat;
    if (!GetStatus(stat))
      return FALSE;
    dwBytesWaiting = stat.cbInQue;
    return TRUE;
  }

  //Configuration Methods
  _Return_type_success_(return != 0) BOOL GetConfig(_Out_ COMMCONFIG& config)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    DWORD dwSize = sizeof(COMMCONFIG);
    return GetCommConfig(m_hComm, &config, &dwSize);
  }

  static BOOL GetDefaultConfig(_In_ int nPort, _Out_ COMMCONFIG& config)
  {
    //Create the device name as a string
    TCHAR szPort[12];
    _stprintf_s(szPort, sizeof(szPort) / sizeof(TCHAR), _T("COM%d"), nPort);

    //Delegate to the other version of the method
    return GetDefaultConfig(szPort, config);
  }

  static BOOL GetDefaultConfig(_In_z_ LPCTSTR pszPort, _Out_ COMMCONFIG& config)
  {
    DWORD dwSize = sizeof(COMMCONFIG);
    return GetDefaultCommConfig(pszPort, &config, &dwSize);
  }

  BOOL SetConfig(_In_ COMMCONFIG& config)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    DWORD dwSize = sizeof(COMMCONFIG);
    return SetCommConfig(m_hComm, &config, dwSize);
  }

  static BOOL SetDefaultConfig(_In_ int nPort, _In_ COMMCONFIG& config)
  {
    //Create the device name as a string
    TCHAR szPort[12];
    _stprintf_s(szPort, sizeof(szPort) / sizeof(TCHAR), _T("COM%d"), nPort);

    //Delegate to the other version of the method
    return SetDefaultConfig(szPort, config);
  }

  static BOOL SetDefaultConfig(_In_z_ LPCTSTR pszPort, _In_ COMMCONFIG& config)
  {
    DWORD dwSize = sizeof(COMMCONFIG);
    return SetDefaultCommConfig(pszPort, &config, dwSize);
  }

//Misc RS232 Methods
  BOOL ClearBreak()
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return ClearCommBreak(m_hComm);
  }

  BOOL SetBreak()
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return SetCommBreak(m_hComm);
  }

  BOOL ClearError(_Out_ DWORD& dwErrors)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return ClearCommError(m_hComm, &dwErrors, nullptr);
  }

  BOOL GetStatus(_Out_ COMSTAT& stat)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    DWORD dwErrors = 0;
    return ClearCommError(m_hComm, &dwErrors, &stat);
  }

  BOOL GetState(_Out_ DCB& dcb)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return GetCommState(m_hComm, &dcb);
  }

  BOOL SetState(_In_ DCB& dcb)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return SetCommState(m_hComm, &dcb);
  }

  BOOL Escape(_In_ DWORD dwFunc)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return EscapeCommFunction(m_hComm, dwFunc);
  }

  BOOL ClearDTR()
  {
    return Escape(CLRDTR);
  }

  BOOL ClearRTS()
  {
    return Escape(CLRRTS);
  }

  BOOL SetDTR()
  {
    return Escape(SETDTR);
  }

  BOOL SetRTS()
  {
    return Escape(SETRTS);
  }

  BOOL SetXOFF()
  {
    return Escape(SETXOFF);
  }

  BOOL SetXON()
  {
    return Escape(SETXON);
  }

  BOOL GetProperties(_Inout_ COMMPROP& properties)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return GetCommProperties(m_hComm, &properties);
  }

  BOOL GetModemStatus(_Out_ DWORD& dwModemStatus)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return GetCommModemStatus(m_hComm, &dwModemStatus);
  }

//Timeouts
  BOOL SetTimeouts(_In_ COMMTIMEOUTS& timeouts)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return SetCommTimeouts(m_hComm, &timeouts);
  }

  BOOL GetTimeouts(_Out_ COMMTIMEOUTS& timeouts)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return GetCommTimeouts(m_hComm, &timeouts);
  }

  BOOL Set0Timeout()
  {
    COMMTIMEOUTS Timeouts;
    memset(&Timeouts, 0, sizeof(Timeouts));
    Timeouts.ReadIntervalTimeout = MAXDWORD;
    return SetTimeouts(Timeouts);
  }

  BOOL Set0WriteTimeout()
  {
    COMMTIMEOUTS Timeouts;
    GetTimeouts(Timeouts);
    Timeouts.WriteTotalTimeoutMultiplier = 0;
    Timeouts.WriteTotalTimeoutConstant = 0;
    return SetTimeouts(Timeouts);
  }

  BOOL Set0ReadTimeout()
  {
    COMMTIMEOUTS Timeouts;
    GetTimeouts(Timeouts);
    Timeouts.ReadIntervalTimeout = MAXDWORD;
    Timeouts.ReadTotalTimeoutMultiplier = 0;
    Timeouts.ReadTotalTimeoutConstant = 0;
    return SetTimeouts(Timeouts);
  }

//Event Methods
  BOOL SetMask(_In_ DWORD dwMask)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return SetCommMask(m_hComm, dwMask);
  }

  BOOL GetMask(_Out_ DWORD& dwMask)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return GetCommMask(m_hComm, &dwMask);
  }

  BOOL WaitEvent(_Inout_ DWORD& dwMask)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return WaitCommEvent(m_hComm, &dwMask, nullptr);
  }

  BOOL WaitEvent(_Inout_ DWORD& dwMask, _Inout_ OVERLAPPED& overlapped)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());
    ATLASSERT(overlapped.hEvent != nullptr);

    return WaitCommEvent(m_hComm, &dwMask, &overlapped);
  }

//Queue Methods
  BOOL Flush()
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return FlushFileBuffers(m_hComm);
  }

  BOOL Purge(_In_ DWORD dwFlags)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return PurgeComm(m_hComm, dwFlags);
  }

  BOOL TerminateOutstandingWrites()
  {
    return Purge(PURGE_TXABORT);
  }

  BOOL TerminateOutstandingReads()
  {
    return Purge(PURGE_RXABORT);
  }

  BOOL ClearWriteBuffer()
  {
    return Purge(PURGE_TXCLEAR);
  }

  BOOL ClearReadBuffer()
  {
    return Purge(PURGE_RXCLEAR);
  }

  BOOL Setup(_In_ DWORD dwInQueue, _In_ DWORD dwOutQueue)
  {
    //Validate our parameters
    ATLASSERT(IsOpen());

    return SetupComm(m_hComm, dwInQueue, dwOutQueue);
  }

protected:
//Member variables
  HANDLE m_hComm; //Handle to the comms port
};


//Exception based version of the class
class CSERIALPORT_EXT_CLASS CSerialPort : public CSerialPort2
{
public:
//General Methods
#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)
__if_exists(OpenCommPort)
{
  BOOL Open(_In_ ULONG uPortNumber, _In_ DWORD dwDesiredAccess, _In_ DWORD dwFlagsAndAttributes)
  {
    BOOL bSuccess = __super::Open(uPortNumber, dwDesiredAccess, dwFlagsAndAttributes);
    if (!bSuccess)
      ThrowSerialException();
  }
}
#endif //#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)

  BOOL Open(_In_z_ LPCTSTR pszPort, _In_ BOOL bOverlapped = FALSE)
  {
    BOOL bSuccess = __super::Open(pszPort, bOverlapped);
    if (!bSuccess)
      ThrowSerialException();
  }

  void Open(_In_ int nPort, _In_ DWORD dwBaud = 9600, _In_ Parity parity = NoParity, _In_ BYTE DataBits = 8, 
            _In_ StopBits stopBits = OneStopBit, _In_ FlowControl fc = NoFlowControl, _In_ BOOL bOverlapped = FALSE)
  {
    BOOL bSuccess = __super::Open(nPort, dwBaud, parity, DataBits, stopBits, fc, bOverlapped);
    if (!bSuccess)
      ThrowSerialException();
  }

  void Open(_In_z_ LPCTSTR pszPort, _In_ DWORD dwBaud = 9600, _In_ Parity parity = NoParity, _In_ BYTE DataBits = 8,
            _In_ StopBits stopBits = OneStopBit, _In_ FlowControl fc = NoFlowControl, _In_ BOOL bOverlapped = FALSE)
  {
    BOOL bSuccess = __super::Open(pszPort, dwBaud, parity, DataBits, stopBits, fc, bOverlapped);
    if (!bSuccess)
      ThrowSerialException();
  }

//Reading / Writing Methods
  DWORD Read(_Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(FILE) void* lpBuffer, _In_ DWORD dwNumberOfBytesToRead)
  {
    DWORD dwBytesRead = 0;
    if (!__super::Read(lpBuffer, dwNumberOfBytesToRead, dwBytesRead))
      ThrowSerialException();

    return dwBytesRead;
  }

  void Read(_Out_writes_bytes_to_opt_(dwNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) void* lpBuffer, _In_ DWORD dwNumberOfBytesToRead, _In_ OVERLAPPED& overlapped, _Inout_opt_ DWORD* lpNumberOfBytesRead = nullptr)
  {
    if (!__super::Read(lpBuffer, dwNumberOfBytesToRead, overlapped, lpNumberOfBytesRead))
      ThrowSerialException();
  }

  void ReadEx(_Out_writes_bytes_opt_(dwNumberOfBytesToRead) __out_data_source(FILE) LPVOID lpBuffer, _In_ DWORD dwNumberOfBytesToRead, _Inout_ LPOVERLAPPED lpOverlapped, _In_ LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
  {
    if (!__super::ReadEx(lpBuffer, dwNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine))
      ThrowSerialException();
  }

  DWORD Write(_In_reads_bytes_opt_(dwNumberOfBytesToWrite) const void* lpBuffer, _In_ DWORD dwNumberOfBytesToWrite)
  {
    DWORD dwBytesWritten = 0;
    if (!__super::Write(lpBuffer, dwNumberOfBytesToWrite, dwBytesWritten))
      ThrowSerialException();

    return dwBytesWritten;
  }

  void Write(_In_reads_bytes_opt_(dwNumberOfBytesToWrite) const void* lpBuffer, _In_ DWORD dwNumberOfBytesToWrite, _In_ OVERLAPPED& overlapped, _Out_opt_ DWORD* lpNumberOfBytesWritten = nullptr)
  {
    if (!__super::Write(lpBuffer, dwNumberOfBytesToWrite, overlapped, lpNumberOfBytesWritten))
      ThrowSerialException();
  }

  void WriteEx(_In_reads_bytes_opt_(dwNumberOfBytesToWrite) LPCVOID lpBuffer, _In_ DWORD dwNumberOfBytesToWrite, _Inout_ LPOVERLAPPED lpOverlapped, _In_ LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
  {
    if (!__super::WriteEx(lpBuffer, dwNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine))
      ThrowSerialException();
  }

  void TransmitChar(_In_ char cChar)
  {
    if (!__super::TransmitChar(cChar))
      ThrowSerialException();
  }

  void GetOverlappedResult(_In_ OVERLAPPED& overlapped, _Out_ DWORD& dwBytesTransferred, _In_ BOOL bWait)
  {
    if (!__super::GetOverlappedResult(overlapped, dwBytesTransferred, bWait))
      ThrowSerialException();
  }

__if_exists(::GetOverlappedResultEx)
{
  void GetOverlappedResultEx(_In_ OVERLAPPED& overlapped, _Out_ DWORD& dwBytesTransferred, _In_ DWORD dwMilliseconds, _In_ BOOL bAlertable)
  {
    if (!__super::GetOverlappedResultEx(overlapped, dwBytesTransferred, dwMilliseconds, bAlertable))
      ThrowSerialException();
  }
}

  void CancelIo()
  {
    if (!__super::CancelIo())
      ThrowSerialException();
  }

#if (_WIN32_WINNT >= 0x0600)
__if_exists(::CancelIoEx)
{
  void CancelIoEx(_In_opt_ LPOVERLAPPED lpOverlapped = nullptr)
  {
    if (!__super::CancelIoEx(lpOverlapped))
      ThrowSerialException();
  }
}
#endif //#if (_WIN32_WINNT >= 0x0600)

  DWORD BytesWaiting()
  {
    DWORD dwBytesWaiting = 0;
    if (!__super::BytesWaiting(dwBytesWaiting))
      ThrowSerialException();
    return dwBytesWaiting;
  }

//Configuration Methods
  void GetConfig(_Out_ COMMCONFIG& config)
  {
    if (!__super::GetConfig(config))
      ThrowSerialException();
  }

  static void GetDefaultConfig(_In_ int nPort, _Out_ COMMCONFIG& config)
  {
    if (!__super::GetDefaultConfig(nPort, config))
      ThrowSerialException();
  }

  static void GetDefaultConfig(_In_z_ LPCTSTR pszPort, _Out_ COMMCONFIG& config)
  {
    if (!__super::GetDefaultConfig(pszPort, config))
      ThrowSerialException();
  }

  void SetConfig(_In_ COMMCONFIG& config)
  {
    if (!__super::SetConfig(config))
      ThrowSerialException();
  }

  static void SetDefaultConfig(_In_ int nPort, _In_ COMMCONFIG& config)
  {
    if (!__super::SetDefaultConfig(nPort, config))
      ThrowSerialException();
  }

  static void SetDefaultConfig(_In_z_ LPCTSTR pszPort, _In_ COMMCONFIG& config)
  {
    if (!__super::SetDefaultConfig(pszPort, config))
      ThrowSerialException();
  }

//Misc RS232 Methods
  void ClearBreak()
  {
    if (!__super::ClearBreak())
      ThrowSerialException();
  }

  void SetBreak()
  {
    if (!__super::SetBreak())
      ThrowSerialException();
  }

  void ClearError(_Out_ DWORD& dwErrors)
  {
    if (!__super::ClearError(dwErrors))
      ThrowSerialException();
  }

  void GetStatus(_Out_ COMSTAT& stat)
  {
    if (!__super::GetStatus(stat))
      ThrowSerialException();
  }

  void GetState(_Out_ DCB& dcb)
  {
    if (!__super::GetState(dcb))
      ThrowSerialException();
  }

  void SetState(_In_ DCB& dcb)
  {
    if (!__super::SetState(dcb))
      ThrowSerialException();
  }

  void Escape(_In_ DWORD dwFunc)
  {
    if (!__super::Escape(dwFunc))
      ThrowSerialException();
  }

  void ClearDTR()
  {
    if (!__super::ClearDTR())
      ThrowSerialException();
  }

  void ClearRTS()
  {
    if (!__super::ClearRTS())
      ThrowSerialException();
  }

  void SetDTR()
  {
    if (!__super::SetDTR())
      ThrowSerialException();
  }

  void SetRTS()
  {
    if (!__super::SetRTS())
      ThrowSerialException();
  }

  void SetXOFF()
  {
    if (!__super::SetXOFF())
      ThrowSerialException();
  }

  void SetXON()
  {
    if (!__super::SetXON())
      ThrowSerialException();
  }

  void GetProperties(_Inout_ COMMPROP& properties)
  {
    if (!__super::GetProperties(properties))
      ThrowSerialException();
  }

  void GetModemStatus(_Out_ DWORD& dwModemStatus)
  {
    if (!__super::GetModemStatus(dwModemStatus))
      ThrowSerialException();
  }

//Timeouts
  void SetTimeouts(_In_ COMMTIMEOUTS& timeouts)
  {
    if (!__super::SetTimeouts(timeouts))
      ThrowSerialException();
  }

  void GetTimeouts(_Out_ COMMTIMEOUTS& timeouts)
  {
    if (!__super::GetTimeouts(timeouts))
      ThrowSerialException();
  }

  void Set0Timeout()
  {
    if (!__super::Set0Timeout())
      ThrowSerialException();
  }

  void Set0WriteTimeout()
  {
    if (!__super::Set0WriteTimeout())
      ThrowSerialException();
  }

  void Set0ReadTimeout()
  {
    if (!__super::Set0ReadTimeout())
      ThrowSerialException();
  }

//Event Methods
  void SetMask(_In_ DWORD dwMask)
  {
    if (!__super::SetMask(dwMask))
      ThrowSerialException();
  }

  void GetMask(_Out_ DWORD& dwMask)
  {
    if (!__super::GetMask(dwMask))
      ThrowSerialException();
  }

  void WaitEvent(_Inout_ DWORD& dwMask)
  {
    if (!__super::WaitEvent(dwMask))
      ThrowSerialException();
  }

  void WaitEvent(_Inout_ DWORD& dwMask, _Inout_ OVERLAPPED& overlapped)
  {
    BOOL bSuccess = __super::WaitEvent(dwMask, overlapped);
    if (!bSuccess)
      ThrowSerialException();
  }

//Queue Methods
  void Flush()
  {
    if (!__super::Flush())
      ThrowSerialException();
  }

  void Purge(_In_ DWORD dwFlags)
  {
    if (!__super::Purge(dwFlags))
      ThrowSerialException();
  }

  void TerminateOutstandingWrites()
  {
    if (!__super::TerminateOutstandingWrites())
      ThrowSerialException();
  }

  void TerminateOutstandingReads()
  {
    if (!__super::TerminateOutstandingReads())
      ThrowSerialException();
  }

  void ClearWriteBuffer()
  {
    if (!__super::ClearWriteBuffer())
      ThrowSerialException();
  }

  void ClearReadBuffer()
  {
    if (!__super::ClearReadBuffer())
      ThrowSerialException();
  }

  void Setup(_In_ DWORD dwInQueue, _In_ DWORD dwOutQueue)
  {
    if (!__super::Setup(dwInQueue, dwOutQueue))
      ThrowSerialException();
  }

//Static methods
  static void ThrowSerialException(_In_ DWORD dwError = 0)
  {
    if (dwError == 0)
      dwError = ::GetLastError();

    ATLTRACE(_T("Warning: throwing CSerialException for error %d\n"), dwError);
#ifdef CSERIALPORT_MFC_EXTENSIONS
    CSerialException* pException = new CSerialException(dwError);
    THROW(pException);
#else
    CSerialException e(dwError);
    throw e;
#endif //#ifdef CSERIALPORT_MFC_EXTENSIONS
  }
};

#endif //#ifndef __SERIALPORT_H__
