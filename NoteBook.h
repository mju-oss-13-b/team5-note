#pragma once

#include <stdio.h>
#include "NoteFile.h"

class CNoteManager
{
public:
	CNoteManager();
	~CNoteManager();

	static CNoteManager& GetInstance();

	void LoadLog();
	void SaveLog( EStatus eStatus , const char *pNoteFile );
	EStatus GetLastStatus();
	const char *GetLastFile();

	int Create( const char *pFileName );
	int Read( CNoteFile &noteFile, const char *pPwd );	// ��
	int Edit( CNoteFile &noteFile, const char *pPwd );	// �༭
	int Append( CNoteFile &noteFile, const char *pPwd );	// ׷��

	int EndEdit( CNoteFile &noteFile );
	int EndAppend( CNoteFile &noteFile );
	int EndRead( CNoteFile &noteFile );
	
	int Lock( const char *pFileName );
	int Unlock( const char *pFileName );
	int ChangePassword( EPWDType eType, CNoteFile &noteFile );
private:
	
	int _Open( CNoteFile &noteFile, const char *pPwd, bool isRemeberState );
	int _Close( CNoteFile &noteFile, const char *pTextName = NULL );

	struct {
		EStatus	eStatus;
		char	acNoteFile[MAX_FILENAME_LEN];
	} m_tLog;
};

#define	g_NoteMgr	CNoteManager::GetInstance()
/*
cout << "-- ��ʾ�������Ƿ񴴽�һ���ռ��ļ���--" << endl;
if ( ::GetUserChoice() ){
	Create( _pNoteFile );
}else {
	cout << "-- ��Ϣ���˳� --" << endl;
	return 1;
}
err = fopen_s( &fp_note, _pNoteFile, "rb+" );

*/
