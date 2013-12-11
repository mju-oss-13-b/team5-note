#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "Interface.h"

using namespace std;

#define ASCII_ENTER      13
#define ASCII_TAB        '\t' 
#define ASCII_BACKSPACE  0x08
#define ASCII_SPACE      ' '
#define ASCII_ESC        27 

// ��������ʵ�ʳ���
int	GetPassword( char *password , int nMaxLen )
{
    strncpy( password, getpass(""), nMaxLen );
    return strlen( password );
}

char* ChangeFileSuffix( char *pOutFile, const char *pSrcFile, const char *pSuffix )
{
	if ( pOutFile && pSrcFile && pSuffix ){
		
		strncpy( pOutFile, pSrcFile, MAX_FILENAME_LEN );
		size_t len = strlen( pOutFile );
		
		char ch;
		do{
			ch = pOutFile[--len];
		}while ( ch != '.' && ch != '\\' && ch != '/' && len );
		
		if ( ch == '.' ){
			pOutFile[len+1] = '\0';
			strncat( pOutFile, pSuffix, MAX_FILENAME_LEN );
			pOutFile[len+4] = '\0';

			return pOutFile;
		}
	}

	return NULL;
}
 
bool	GetUserChoice( void )
{
	while (1){
		cout << "예 혹은 아니오(Y/N) : " ;
		char ch = getchar();
		switch ( ch ){
		case 'Y':
		case 'y':
			cout << "예" << endl;
			return true;
		case 'N':
		case 'n':
			cout << "아니오" << endl;
			return false;
		default:
			cout << "다시 입력해주세요." << endl;
		}
	}
}

bool	GetNewPassword( char *password )
{
	int _nLen1, _nLen2;
	char _acComfirm[PWD_LEN];

	while(true){
		cout << "새 비밀번호를 입력해주세요: ";
		_nLen1 = ::GetPassword( password );
		if (_nLen1==0){
			break;
		}
		cout << "다시 입력해주세요: ";
		_nLen2 = ::GetPassword( _acComfirm );
		if (_nLen2==0){
			break;
		}
		if ( (_nLen1==_nLen2) && (strcmp( password , _acComfirm) == 0) ){
			break;
		}
		cout << "-- 안내: 비밀번호가 유효하지 않습니다, 다시 입력해주세요 --" << endl;
	}

	if ( _nLen1 == 0 || _nLen2 == 0 ){
		cout << "-- 안내: 유저가 취소하였습니다. --" << endl;
		return false;
	}else{
		return true;
	}
}
