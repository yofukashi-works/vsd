// vsdwin.h : VSDWIN �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#if !defined(AFX_VSDWIN_H__AE6F6DC6_DF23_4B49_9D4C_D69FF5393BAA__INCLUDED_)
#define AFX_VSDWIN_H__AE6F6DC6_DF23_4B49_9D4C_D69FF5393BAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CVsdwinApp:
// ���̃N���X�̓���̒�`�Ɋւ��Ă� vsdwin.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CVsdwinApp : public CWinApp
{
public:
	CVsdwinApp();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CVsdwinApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����

	//{{AFX_MSG(CVsdwinApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	HACCEL m_hAccel;
	CString GetModulePathFileName(LPCTSTR pName);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_VSDWIN_H__AE6F6DC6_DF23_4B49_9D4C_D69FF5393BAA__INCLUDED_)
