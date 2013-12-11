#include "NoteBook.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "Interface.h"
#include "Encrypt.h"
#include "NoteFile.h"

using namespace std;

const char *_sNoteConfigFile = "notebook.log";

////////////////////////////////////////////////////////////////////////////

CNoteManager::CNoteManager()
{
	srand(static_cast <unsigned   int> (time(NULL)));
	LoadLog();
}

CNoteManager::~CNoteManager()
{
}

CNoteManager& CNoteManager::GetInstance()
{
	static CNoteManager instance;
	return instance;
}

void CNoteManager::LoadLog()
{
	FILE *fp = NULL;
	fp = fopen( _sNoteConfigFile, "rb" );
	if ( fp ){
		fread( &m_tLog, sizeof(m_tLog), 1, fp );
		fclose( fp );
	}else {
		m_tLog.eStatus = STATUS_CLOSE;
		m_tLog.acNoteFile[0] = '\0';
	}
}

void CNoteManager::SaveLog( EStatus eStatus , const char *pNoteFile )
{
	m_tLog.eStatus = eStatus;
	if ( pNoteFile ){
		strncpy( m_tLog.acNoteFile, pNoteFile, MAX_FILENAME_LEN );
	}

	FILE *fp = NULL;
	fp = fopen( _sNoteConfigFile, "wb" );
	if ( fp ){
		fwrite( &m_tLog, sizeof(m_tLog), 1, fp );
		fclose( fp );
	}else {
		cout << "-- INFO: Create notefile fail! --" << endl;
	}
}

EStatus CNoteManager::GetLastStatus()
{
	return m_tLog.eStatus;
}

const char *CNoteManager::GetLastFile()
{
	return m_tLog.acNoteFile;
}
int CNoteManager::Create( const char *pFileName )
{
	char	_acPassword[PWD_LEN+1];
	int		retVal = 0;

	cout << "-- 정보: 새 노트를 생성합니다... --" << endl;
	
	if ( pFileName == NULL || *pFileName == 0 ){
		cout << "-- 에러: 파일이 유효하지 않습니다. --" << endl;
		return ERRNO_PARAM_INVAILD;
	}

	CNoteFile	_noteFile( pFileName );

	if ( _noteFile.Create() ){
		if ( GetNewPassword( _acPassword ) ){

			_noteFile.SetDefault();

			char _acTextBuff[50] = "";
			time_t _currTime = time(NULL);
			//ctime_s(_acTextBuff, 50, &_currTime);
		    strncpy( _acTextBuff, ctime(&_currTime), 50 );	
            _acTextBuff[ strlen(_acTextBuff)-1 ] = '\0'; // ȥ��ctimes()�������ɵ� '\n'
			strncat( _acTextBuff, "\xd\xa�����ռǱ�", 50 );
			int _nLen = (int)strlen(_acTextBuff);

			_noteFile.SaveBody( _acTextBuff, _nLen, _acPassword );
			_noteFile.SetPasswordVaild( PWD_NOTE, _acPassword );
			_noteFile.SetPasswordVaild( PWD_PRIV, _acPassword );
			_noteFile.SetPasswordVaild( PWD_UNLOCK, _acPassword );
			_noteFile.SetDataLen( _nLen );
			_noteFile.SetStatus( STATUS_CLOSE );
			_noteFile.SaveHead();
			
		}
		_noteFile.Close();
	} else {
		cout << "-- 정보: 새 노트를 생성하는데 실패하였습니다! --" << endl;
		retVal = ERRNO_FILE_CREATE_FAIL;
	}

	return retVal;
}


int CNoteManager::Read( CNoteFile &noteFile, const char *pPwd )
{
	cout << "-- 정보: 파일 읽기를 시작합니다 --" << endl;

	if ( !noteFile.GetReadEnable() ){
		cout << "-- 정보: 이 기능은 사용이 불가능합니다! --" << endl;
		return ERRNO_FUNC_DISABLE;
	}

	return	_Open( noteFile, pPwd, false );
}

int CNoteManager::Edit( CNoteFile &noteFile, const char *pPwd )
{
	cout << "-- 정보: 파일 편집을 시작합니다 --" << endl;

	if ( !noteFile.GetEditEnable() ){
		cout << "-- 정보: 이 기능은 사용이 불가능합니다! --" << endl;
		return ERRNO_FUNC_DISABLE;
	}

	return	_Open( noteFile, pPwd, true );
}

int CNoteManager::EndEdit( CNoteFile &noteFile )
{
	cout << "-- 정보: 파일 편집을 종료합니다 --" << endl;
	return _Close( noteFile );
}

int CNoteManager::_Open( CNoteFile &noteFile, const char *pPwd, bool isRemeberState )
{
	int		retVal = 0;

	if ( noteFile.GetStatus() == STATUS_CLOSE ){

		char *_pTextBuff = new char [noteFile.GetDataLen()];

		if ( _pTextBuff ){

			noteFile.LoadBody( _pTextBuff, noteFile.GetDataLen(), pPwd );
			
			char _acTextFileName[MAX_FILENAME_LEN] = "";

			ChangeFileSuffix( _acTextFileName, noteFile.GetFileName(), "txt" );
			
			// _pTextBuff �������ģ���ֱ����ʾ
			FILE *fp_text = NULL;
			fp_text = fopen( _acTextFileName, "wb" );
			if ( fp_text ){
				fwrite( _pTextBuff, noteFile.GetDataLen(), 1, fp_text );	// ������д�� _pTextFileName �ļ�ȥ

				if ( isRemeberState ){
					noteFile.SetStatus( STATUS_READ );	// ������־״̬
					noteFile.StorePassword( pPwd );
					noteFile.SaveHead();
				}

				fclose( fp_text );
				fp_text = NULL;
			}else {
				cout << "--에러: 파일 [" << _acTextFileName << "] 의 생성을 실패하였습니다! --" << endl; 
				retVal = ERRNO_FILE_CREATE_FAIL;
			}
			memset( _pTextBuff, 0, noteFile.GetDataLen() );
			delete [] _pTextBuff;
			_pTextBuff = NULL;
		}else{
			cout << "-- 에러: 메모리 Alloc을 실패하였습니다! --" << endl;
			retVal = ERRNO_ALLOC_MEMORY_FAIL;
		}
	}else {
		cout << "-- 에러: 파일이 열려있습니다! --" << endl;
		retVal = ERRNO_FILE_STATUS_ERR;
	}
	
	return retVal;
}

int CNoteManager::_Close(  CNoteFile &noteFile, const char *pTextFile )
{

	FILE *fp_text = NULL;
	int	retVal = 0;

	if ( noteFile.GetStatus() == STATUS_READ ){

		char _acTextFileName[MAX_FILENAME_LEN];
		const char *_pTextFile = NULL;

		if ( pTextFile ){
			_pTextFile = pTextFile;
		}else {
			::ChangeFileSuffix( _acTextFileName, noteFile.GetFileName(), "txt" );
			_pTextFile = _acTextFileName;
		}

		FILE *fp_text ;
		fp_text = fopen( _pTextFile, "rb" );	// ����pTextName�ļ�����������ȡ������

		if ( fp_text ){	// �����ļ��ɹ�
			
			fseek( fp_text, 0, 2 );
			int text_len = ftell( fp_text );

			char *pBuff = new char [text_len];
			if ( pBuff ){
				
				rewind( fp_text );
				fread ( pBuff, text_len, 1, fp_text );
				
				char _acPassword[PWD_LEN];
				noteFile.LoadPassword( _acPassword );	// �����ݴ�������
				
				noteFile.SaveBody( pBuff, text_len, _acPassword );	// ������������

				noteFile.SetStatus( STATUS_CLOSE );	// ������־�ļ�ͷ
				noteFile.SetDataLen( text_len );
				noteFile.SaveHead();

				delete [] pBuff;
				pBuff = NULL;
			}else {
				cout << "-- 에러: 메모리 Alloc을 실패하였습니다! --" << endl;
				retVal = ERRNO_ALLOC_MEMORY_FAIL;
			}
			fclose( fp_text );
		}else {
			cout << "-- 에러: [" << "] 여는 것을 실패하였습니다! --" << endl; 
			retVal = ERRNO_FILE_OPEN_FAIL;
		}
	}else {
		cout << "-- 에러: 파일이 닫혔습니다! --" << endl;
		retVal = ERRNO_FILE_STATUS_ERR;
	}

	return retVal;
}

int CNoteManager::Append( CNoteFile &noteFile, const char *pPwd )
{
	FILE	*_pFile = NULL;
	int		retVal = 0;

	cout << "-- 에러: 파일 수정을 시작합니다! --" << endl;

	if ( !noteFile.GetAppendEnable() ){
		cout << "-- 정보: 이 기능은 사용이 불가능합니다! --" << endl;
		return ERRNO_FUNC_DISABLE;
	}

	if ( noteFile.GetStatus() == STATUS_CLOSE ){
		
		char _acTextFileName[MAX_FILENAME_LEN] = "";
		ChangeFileSuffix( _acTextFileName, noteFile.GetFileName(), "txt" );
		
		FILE *fp_text = NULL;
		fp_text = fopen( _acTextFileName, "wb" );
		if ( fp_text ){

			char _acTextBuff[50] = "";
			time_t _currTime = time(NULL);
			//ctime_s(_acTextBuff, 50, &_currTime);
		    strncpy( _acTextBuff, ctime(&_currTime), 50 );
            _acTextBuff[ strlen(_acTextBuff)-1 ] = '\0'; // ȥ��ctimes()�������ɵ� '\n'
			strncat( _acTextBuff, "\xd\xa", 50 );
			int _nLen = (int)strlen(_acTextBuff);

			fwrite( _acTextBuff, _nLen, 1, fp_text );	// ��д����

			noteFile.SetStatus( STATUS_APPEND );	// ������־״̬
			noteFile.StorePassword( pPwd );
			noteFile.SaveHead();

			fclose( fp_text );
			fp_text = NULL;
		}else {
			cout << "-- 에러: 파일 [" << _acTextFileName << "] 의 생성을 실패하였습니다! --" << endl; 
			retVal = ERRNO_FILE_CREATE_FAIL;
		}
	}else {
		cout << "-- 에러: 파일이 열려있습니다! --" << endl;
		retVal = ERRNO_FILE_STATUS_ERR;
	}

	return retVal;
}

int CNoteManager::EndAppend( CNoteFile &noteFile )
{
	int		retVal = 0;

	cout << "-- 정보: 파일 수정을 종료합니다 --" << endl;

	if ( noteFile.GetStatus() == STATUS_APPEND ){
		
		char _acTextFileName[MAX_FILENAME_LEN] = "";
		ChangeFileSuffix( _acTextFileName, noteFile.GetFileName(), "txt" );
		
		FILE *fp_text = NULL;
		fp_text = fopen( _acTextFileName, "rb" );
		if ( fp_text ){

			fseek( fp_text, 0, 2 );
			int nTextLen = ftell( fp_text );	// ��ȡtxt�ļ��ĳ���
			rewind( fp_text );
			int nTotalLen = nTextLen + noteFile.GetDataLen() + 4;	// �ܳ��� = �ı����� + ���ĳ��� + 4B�ġ�\xd\xa\xd\xa��

			char *pBuff = new char [nTotalLen];
			if ( pBuff ){

				fread ( pBuff, nTextLen, 1, fp_text );	// ���ı��ļ������ݶ���pBuff����
				strncpy( pBuff+nTextLen, "\xd\xa\xd\xa", 5 );	// ׷�ӡ�\xd\xa\xd\xa��
				char _acPassword[PWD_LEN];
				noteFile.LoadPassword( _acPassword );	// ��������
				noteFile.LoadBody( pBuff+nTextLen+4, noteFile.GetDataLen(), _acPassword) ;	// �����Ľ��ܵ�����
				noteFile.SaveBody( pBuff, nTotalLen, _acPassword) ;	// ��pBuff�������������м��ܲ�������־�ļ�
				// ����noteFile�����ݳ�����״̬
				noteFile.SetDataLen( nTotalLen );
				noteFile.SetStatus( STATUS_CLOSE );
				noteFile.SaveHead();
				
				delete [] pBuff;
				pBuff = NULL;
			}else {
				cout << "-- 에러: 메모리 Alloc 을 실패하였습니다! --" << endl;
				retVal = ERRNO_ALLOC_MEMORY_FAIL;
			}
			fclose( fp_text );
			fp_text = NULL;
		}else {
			cout << "-- 에러: 파일 [" << _acTextFileName << "] 여는 것을 실패하였습니다! --" << endl; 
			retVal = ERRNO_FILE_CREATE_FAIL;
		}
	}else {
		cout << "-- 에러: 파일이 수정 모드로 열려 있지 않습니다! --" << endl;
		retVal = ERRNO_FILE_STATUS_ERR;
	}

	return retVal;
}

int CNoteManager::EndRead( CNoteFile &noteFile )
{
	noteFile.SetStatus( STATUS_CLOSE );
	return 0;
}

int CNoteManager::ChangePassword( EPWDType eType, CNoteFile &noteFile )
{
	if ( noteFile.GetStatus() == STATUS_CLOSE ){
		cout << "-- 정보: 비밀번호를 입력해주세요 --" << endl;
		
		char acOldPassword[PWD_LEN];
		char acNewPassword[PWD_LEN];
		
		cout << "입력: ";
		::GetPassword( acOldPassword );
		if ( noteFile.GetPasswordVaild( eType, acOldPassword ) ){
			if ( ::GetNewPassword( acNewPassword ) ){
				if ( eType == PWD_NOTE ){
					int len = noteFile.GetDataLen();
					char *pBuff = new char [len];
					if (pBuff){
						noteFile.LoadBody( pBuff, len, acOldPassword );
						noteFile.SaveBody( pBuff, len, acNewPassword );
						noteFile.SetPasswordVaild( PWD_NOTE, acNewPassword );	// ����������֤
						noteFile.SaveHead();
						delete [] pBuff;
						pBuff = NULL;
					}else {
						cout << "-- 에러: 메모리 Alloc 을 실패하였습니다! --" << endl;
						return ERRNO_ALLOC_MEMORY_FAIL;
					}
				}else{
					noteFile.SetPasswordVaild( eType, acNewPassword );
				}
			}else{
				cout << "-- 정보: 유저가 취소하였습니다 --" << endl;  
				return ERRNO_USER_CANCEL;
			}
		}else{
			cout << "-- 정보: 비밀번호가 유효하지 않습니다! --" << endl;
			usleep(2000);
			return ERRNO_PWD_INVAILD;
		}
	}else{
		cout << "-- 에러: 노트가 닫혀 있지 않습니다, 우선 닫아주세요! --" << endl;
		return ERRNO_FILE_STATUS_ERR;
	}
	return 0;
}

int CNoteManager::Lock( const char *pFileName )
{
	return 0;
}

int CNoteManager::Unlock( const char *pFileName )
{
	return 0;
}
