// NoteBook.cpp : ��������̨Ӧ�ó��������ڵ㡣
//
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "Encrypt.h"
#include "NoteBook.h"
#include "VirtualArray.h"
#include "Interface.h"

using namespace std;

// �����ļ�
int DestoryFile( const char * pFileName )
{
	FILE *fp = NULL;
	fp = fopen( pFileName, "rb+" );
	if ( fp ){

		fseek( fp, 0, 2 );
		int len = ftell( fp );	// ��ȡ�ļ���С
		rewind( fp );

		// ����0
		while ( len-- ){
			fputc( 0, fp );
		}

		fclose( fp );
		fp = NULL;

		// ɾ���ļ�
		unlink( pFileName );
		cout << "-- 정보: 파일이 파괴되었습니다! --" << endl;

	} else {
		cout << "-- 에러: 파일이 존재하지 않습니다! --" << endl;
	}
	
	return 0;
}

// �����ϴ�δ���ɵĹ���
int CloseTextFile( void )
{
	g_NoteMgr.LoadLog();

	if ( g_NoteMgr.GetLastStatus() == STATUS_CLOSE )
		return 0;

	CNoteFile _noteFile( g_NoteMgr.GetLastFile() );
	if ( _noteFile.Open() ){
		switch ( g_NoteMgr.GetLastStatus() ){
			case STATUS_APPEND:
				g_NoteMgr.EndAppend( _noteFile );
				break;
			case STATUS_EDIT:
				g_NoteMgr.EndEdit( _noteFile );
				break;
			case STATUS_READ:
				break;
		}
		g_NoteMgr.SaveLog( STATUS_CLOSE, NULL );
		char acTextFile[MAX_FILENAME_LEN];
		::ChangeFileSuffix( acTextFile, g_NoteMgr.GetLastFile(), "txt" );
		::DestoryFile( acTextFile );
	}else {
		cout << "-- 에러: 파일을 여는데 실패하였습니다! --" << endl;
	}
	return 0;
}

bool OpenNoteFile( CNoteFile &noteFile, char *pPwd )
{
	bool retVal = false;

	while ( !noteFile.Open() ){
		cout << "-- 에러: 파일이 존재하지 않거나 올바른 형식이 아닙니다! --" << endl;
		cout << "-- 정보: 새 파일 생성을 원하십니까? --" << endl;
		if ( !::GetUserChoice() ){
			return false;
		}
		g_NoteMgr.Create( noteFile.GetFileName() );
	}

	// NoteFile�ļ�һ���Ǵ򿪳ɹ��˵�
	if ( noteFile.IsLock() ){
		
		cout << "-- 정보: 파일이 잠겨 있습니다, 우선 잠금을 풀어주세요! --" << endl;
		while (true){
			cout << "Input Password: ";
			Sleep(1000);
			if ( ::GetPassword( pPwd ) == 0 ){
				cout << "-- 정보: 유저가 유효한 비밀번호를 취소하였습니다 --" << endl;
				return false;
			}
			if ( noteFile.GetPasswordVaild( PWD_UNLOCK, pPwd ) ){
				noteFile.Unlock();
				noteFile.SaveHead();
				cout << "-- 정보: 파일 잠금이 성공하였습니다 --" << endl;
				break;
			}
			usleep(2000);
		}
	}

	int	tries = 0;
	int _nLen = 0;
	
	cout << "-- 정보: 비밀번호를 확인하시고, 다시 입력해주세요 -" << endl;
	// ������֤
	while(true){

		if ( tries >= noteFile.GetTryTimes() ){
			break;
		}
		cout << "다시 입력해주세요[" << noteFile.GetTryTimes() - tries <<"] : ";
		tries ++ ;

		_nLen = ::GetPassword( pPwd );
		if ( _nLen == 0 ){
			break;
		}
		if ( noteFile.GetPasswordVaild( PWD_NOTE, pPwd ) ){
			break;	
		}
	}
	if ( tries < noteFile.GetTryTimes() ){	// ���Դ���δ����

		if ( _nLen != 0 ){	// �û���������ȷ������
			
			cout << "-- 정보: 축하합니다! 진입하셨습니다! --" << endl;
			retVal = true;

		}else {
			cout << "-- 정보: 유저가 유효한 비밀번호를 취소하였습니다 --" << endl;
		}
	} else {
		// ���ļ�
		noteFile.Lock();
		noteFile.SaveHead();
		cout << "-- 정보: 제한횟수를 초과하셨습니다, 파일이 잠깁니다! --" << endl;
	}

	return retVal;
}

void Wait()
{
	cout << "-- 정보: 파일이 열려있습니다, \"close\" 를 입력하시면 파일이 닫힙니다. -- " << endl;
	string str;
	do{
		cout << "Please Input: " ;
		cin >> str;
	}while ( str != "close" );
}

void ChangePassword( CNoteFile &noteFile )
{
	// �������룺�������롢Ȩ���޸����롢��������
	while (true){
		char choice ;
		do{
			cout << "-- 정보: 기능을 선택해주세요 --" << endl;
			cout << "<1> 노트 기본 비밀번호" << endl;
			cout << "<2> 기능 비밀번호" << endl;
			cout << "<3> 잠금해제 비밀번호" << endl;
			cout << "<Q> Return" << endl;
			cout << "선택: ";
			cin >> choice;
		}while ( choice<'1' && choice>'3' && choice!='q' && choice!='Q' );
		cout << endl;

		switch( choice ){
			case '1' :	// ��־��������
				cout << "-- 정보: 노트 기본 비밀번호를 변경합니다 --" << endl;
				if ( g_NoteMgr.ChangePassword( PWD_NOTE, noteFile ) == 0 ){
					cout << "-- 정보: 재실행해주세요 --" << endl;
					cin.get();
					exit(0);
				}
				break;
			case '2' :	// Ȩ����������
				cout << "-- 정보: 기능 비밀번호를 변경합니다 --" << endl;
				if ( g_NoteMgr.ChangePassword( PWD_PRIV, noteFile ) == 0 ){
					cout << "-- 정보: 성공하였습니다! --" << endl;
				}
				break;
			case '3' :	// ��������
				cout << "-- 정보: 잠금해제 비밀번호를 변경합니다--" << endl;
				if ( g_NoteMgr.ChangePassword( PWD_UNLOCK, noteFile ) == 0 ){
					cout << "-- 정보: 성공하였습니다! --" << endl;
				}
				break;
			case 'q' :
			case 'Q' :
				return;
		}
	}
}

void SetPrivilege( CNoteFile &noteFile )
{
	// ������֤
	char acPassword[PWD_LEN];
	cout << "-- 정보: 설정 기능, 기능 비밀번호를 입력해주세요 --" << endl;

	while ( true ){
		cout << "다시 입력해주세요: ";	
		if ( ::GetPassword(acPassword) == 0 ){
			cout << "-- 정보: 유저가 유효한 비밀번호를 취소하였습니다 --" << endl;
			return;
		}
		cout << endl;	
		if ( noteFile.GetPasswordVaild( PWD_PRIV, acPassword ) ){
			cout << "-- 정보: 축하합니다! 진입하였습니다! --" << endl;
			break;
		}
		usleep(2000);
	}

	// ����Ȩ�ޣ�׷�ӡ��Ķ����༭���������ر�
	while (true){
		char choice ;
		do{
			cout << "-- 정보: 기능을 선택해주세요 --" << endl;
			cout << "<1> 쓰기 [" << (noteFile.GetAppendEnable()?"On":"Off") << "]" << endl;
			cout << "<2> 읽기 [" << (noteFile.GetReadEnable()?"On":"Off") << "]" << endl;
			cout << "<3> 수정 [" << (noteFile.GetEditEnable()?"On":"off") << "]" << endl;
			cout << "<Q> 되돌아가기" << endl;
			cout << "선택: ";
			cin >> choice;
		}while ( choice<'1' && choice>'3' && choice!='q' && choice!='Q' );
		cout << endl;

		switch( choice ){
			case '1' :	// ׷��
				noteFile.SetAppendEnable( !noteFile.GetAppendEnable() );
				break;
			case '2' :	// �Ķ�
				noteFile.SetReadEnable( !noteFile.GetReadEnable() );
				break;
			case '3' :	// �༭
				noteFile.SetEditEnable( !noteFile.GetEditEnable() );
				break;
			case 'q' :
			case 'Q' :
				return;
		}
		noteFile.SaveHead();
	}
}

void SetTryTime( CNoteFile &noteFile )
{
	cout << "-- 정보: 제한 횟수를 설정해주세요. --" << endl;
	int times = 0;
	cout << "선택: ";
	cin >> times;
	if ( times > 0 ){
		noteFile.SetTryTimes( times );
		noteFile.SaveHead();
		cout << "-- 정보: 설정에 성공하였습니다! 현재 값은 [" << times << "] 입니다 --" << endl;
	}else{
		cout << "-- 정보: 실패하였습니다 --" << endl;
	}
}

// ����
void Setting(  CNoteFile &noteFile )
{
	while (true){
		char choice ;
		do{
			cout << "-- 정보: 기능을 선택해주세요 --" << endl;
			cout << "<1> 비밀번호 설정" << endl;
			cout << "<2> 기능 설정" << endl;
			cout << "<3> 노트 잠금" << endl;
			cout << "<4> 제한 횟수 설정" << endl;
			cout << "<Q> 되돌아가기" << endl;
			cout << "선택: ";
			cin >> choice;
		}while ( choice<'1' && choice>'4' && choice!='q' && choice!='Q' );
		cout << endl;

		switch( choice ){
			case '1' :
				ChangePassword( noteFile );
				break;
			case '2' :
				SetPrivilege( noteFile );
				break;
			case '3' :	// ����
				cout << "-- 정보: 정말 노트를 잠그시겠습니까? --" << endl;
				if ( ::GetUserChoice() ){
					noteFile.Lock( true );
					noteFile.SaveHead();
					cout << "-- 정보: 노트가 잠겼습니다! --" << endl;
				}
				break;
			case '4' :
				SetTryTime( noteFile );
				break;
			case 'q' :
			case 'Q' :
				return;
		}
	}
}

void NoteBook( const char *pNoteFile )
{
	char	_acPassword[PWD_LEN+1];
	CNoteFile _noteFile( pNoteFile );

	if ( OpenNoteFile( _noteFile, _acPassword ) ){
		while (true){
			char choice ;
			do{
				cout << "-- 정보: 기능을 선택해주세요 --" << endl;
				cout << "<1> 쓰기" << endl;
				cout << "<2> 읽기" << endl;
				cout << "<3> 수정" << endl;
				cout << "<4> 설정" << endl;
				cout << "<Q> 나가기" << endl;
				cout << "선택: ";
				cin >> choice;
			}while ( choice<'1' && choice>'4' && choice!='q' && choice!='Q' );
			cout << endl;
			
			char acTextFile[MAX_FILENAME_LEN];
			::ChangeFileSuffix( acTextFile, pNoteFile, "txt" );

			switch( choice ){
				case '1' :
                case 'a' :
					if ( g_NoteMgr.Append( _noteFile, _acPassword ) == 0 ){
						g_NoteMgr.SaveLog( STATUS_APPEND, pNoteFile );
						::Wait();
						g_NoteMgr.EndAppend( _noteFile );
						::DestoryFile( acTextFile );
					}
					break;

				case '2' :
                case 'r' :
					if ( g_NoteMgr.Read( _noteFile, _acPassword ) == 0 ){
						g_NoteMgr.SaveLog( STATUS_READ, pNoteFile );
						::Wait();
						g_NoteMgr.EndRead( _noteFile );
						::DestoryFile( acTextFile );
					}
					break;

				case '3' :
                case 'e' :
					if ( g_NoteMgr.Edit( _noteFile, _acPassword ) == 0 ){
						g_NoteMgr.SaveLog( STATUS_EDIT, pNoteFile );
						::Wait();
						g_NoteMgr.EndEdit( _noteFile );
						::DestoryFile( acTextFile );
					}
					break;

				case '4' :
                case 's' :
					Setting( _noteFile );
					break;

				case 'q' :
				case 'Q' :
					return;
			}
			g_NoteMgr.SaveLog( STATUS_CLOSE, NULL );
		}
	}else {
		cout << "-- 정보: 노트를 열 수 없습니다. --" << endl;
		cin.get();
	}
}
void Wellcom()
{
	cout << "+---------------------------------------------------+" << endl;
	cout << "|들어와서 환영합니다! 전 당신의 비밀스런 친구입니다 |" << endl;
	cout << "+---------------------------------------------------+" << endl;
	cout << "아무 키나 누르시면 계속됩니다..." << endl;
	char ch = getchar();
	if ( ch == '?' ){
		cout << "저는 <<NoteBook V1.0>> 입니다, 애용해주세요." << endl;
	}
	cout << endl << endl;
}

int main(int argc, char* argv[])
{
	char acNoteFile[MAX_FILENAME_LEN];
	char *pNoteFile = NULL;

	CloseTextFile();
	
	Wellcom();

	if ( argc < 2 ){
		cout << "-- 정보: 파일이름을 입력해주세요 --" << endl;
		cout << "파일이름 : ";
		cin >> acNoteFile;
		pNoteFile = acNoteFile;
	}else {
		pNoteFile = argv[1];
	}
	NoteBook( pNoteFile );

	return 0;
}
