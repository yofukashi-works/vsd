// vsdwin.cpp : �A�v���P�[�V�����p�N���X�̒�`���s���܂��B
//

#include "stdafx.h"
#include "vsdwin.h"
#include "vsdwinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVsdwinApp

BEGIN_MESSAGE_MAP(CVsdwinApp, CWinApp)
	//{{AFX_MSG_MAP(CVsdwinApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVsdwinApp �N���X�̍\�z

CVsdwinApp::CVsdwinApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}

/////////////////////////////////////////////////////////////////////////////
// �B��� CVsdwinApp �I�u�W�F�N�g

CVsdwinApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVsdwinApp �N���X�̏�����

//�����F�t�@�C����
//�߂�l�F���A�v���P�[�V������exe�t�@�C���Ɠ����f�B���N�g���ɂȂ�t���p�X
CString CVsdwinApp::GetModulePathFileName(LPCTSTR pName)
{
	//exe�t�@�C���̃t���p�X���擾
	TCHAR path[_MAX_PATH];
	::GetModuleFileName(NULL,path,sizeof path);

	//exe�t�@�C���̃p�X�𕪉�
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_splitpath(path,drive,dir,fname,ext);

	//�����̃t�@�C������exe�t�@�C���̃p�X�Ńt���p�X��
	_makepath(path,drive,dir,pName,"");

	CString str(path);
	return str;
}

BOOL CVsdwinApp::InitInstance()
{
	// �W���I�ȏ���������
	// ���������̋@�\���g�p�����A���s�t�@�C���̃T�C�Y��������������
	//  ��Έȉ��̓���̏��������[�`���̒�����s�K�v�Ȃ��̂��폜����
	//  ���������B

#ifdef _AFXDLL
	Enable3dControls();			// ���L DLL ���� MFC ���g���ꍇ�͂������R�[�����Ă��������B
#else
	Enable3dControlsStatic();	// MFC �ƐÓI�Ƀ����N����ꍇ�͂������R�[�����Ă��������B
#endif
	
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	
	// INI�t�@�C���̃p�X����ύX����
	if( m_pszProfileName ) {
		delete ((void*)m_pszProfileName);
		m_pszProfileName = new char[MAX_PATH];
		if( !m_pszProfileName ) {
			AfxMessageBox("�������s���G���[�ł��B");
			return FALSE;
		}
		CString str = GetModulePathFileName( "vsdwin.ini" );
		strcpy((LPTSTR)m_pszProfileName, str );
	}
	
	CVsdwinDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
		//       �L�q���Ă��������B
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �_�C�A���O�� <��ݾ�> �ŏ����ꂽ���̃R�[�h��
		//       �L�q���Ă��������B
	}

	// �_�C�A���O�������Ă���A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n������́A
	// �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}

BOOL CVsdwinApp::ProcessMessageFilter(int code, LPMSG lpMsg) 
{
	if(m_hAccel!= NULL){
		if(::TranslateAccelerator(m_pMainWnd -> m_hWnd, m_hAccel, lpMsg)){	// WM_KEYDOWN&WM_SYSKEYDOWN��WM_COMMAND&WM_SYSCOMMAND�ɕϊ��B
			return TRUE;													// �ϊ������b�Z�[�W�������������Ȃ��Ƃ���TRUE��Ԃ��B
//			return CWinApp::ProcessMessageFilter(code, lpMsg);				// ����̏������s�������ꍇ�͂������B
		}
	}
	return CWinApp::ProcessMessageFilter(code, lpMsg);
}

int CVsdwinApp::ExitInstance() 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	if( m_pszProfileName ) {
		delete ((void*)m_pszProfileName);
		m_pszProfileName = NULL;  // �����Y�ꂸ�ɁI�I
	}
	return CWinApp::ExitInstance();
}
