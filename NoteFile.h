#pragma once

#include <stdio.h>
#include <time.h>
#include "typedef.h"

#define	TAG				"NoteBook"
#define	MAGIC			0x3fa83ec9
#define	START_POS		200
#define	TRYTIMES		5

typedef enum _Status{
	STATUS_CLOSE,	// �ر�
	STATUS_READ,	// ��ֻ��
	STATUS_EDIT,	// �򿪱༭
	STATUS_APPEND	// ��׷��
}EStatus;

typedef enum _PWDType{
	PWD_NOTE,
	PWD_PRIV,
	PWD_UNLOCK
}EPWDType;

class CNoteFile
{
public:
	CNoteFile( const char *pNoteFile );
	~CNoteFile();

	void	SetFileName( const char *pNoteFile );
	const char * GetFileName(void) { return m_acNoteFile; }
	bool	Create(void);
	bool	Open(void);	// �򿪲������ļ�ͷ���Ը�ʽ������֤
	void	Close(void);	// �����ļ�ͷ�����ر��ļ�

	bool	LoadHead(void);		// ���ļ��ﵼ���ļ�ͷ
	bool	SaveHead(void);		// ��m_head���浽�ļ���
	
	void	SetDefault(void);
	bool	IsVaild(void);
	
	void	SetTryTimes(int times);
	int		GetTryTimes(void){ return m_head.tryTimes; };

	bool	IsLock(void);
	void	Lock( bool isForever = false );
	void	Unlock(void);

	EStatus	GetStatus(void) { return m_head.status; }
	void	SetStatus(EStatus newStatus) { m_head.status = newStatus; }

	int		GetDataLen(void) { return m_head.dataLen; }
	void	SetDataLen( int len ) { m_head.dataLen = len; }

	bool	GetPasswordVaild( EPWDType eType, const char *pwd );	// ��ȡ������֤���
	void	SetPasswordVaild( EPWDType eType, const char *pwd );	// ����������֤����
	
	void	StorePassword( const char * pwd );	// ��������ܺ��ݴ����ļ�ͷ��
	void	LoadPassword( char * pwd );	// ���ļ�ͷ�������벢��ѹ����

	int		LoadBody( char *pBuff , int nLen, const char *pPwd );	// ��ȡ�ļ����ݣ���ʹ��pPwd������н���
	int		SaveBody( char *pBuff , int nLen, const char *pPwd );	// �����ļ��������浽�ļ�

	void	SetAppendEnable( bool newState ) { m_head.append_en = newState; }
	void	SetReadEnable( bool newState ) { m_head.read_en = newState; }
	void	SetEditEnable( bool newState ) { m_head.edit_en = newState; }
	bool	GetAppendEnable(void) { return m_head.append_en; }
	bool	GetReadEnable(void) { return m_head.read_en; }
	bool	GetEditEnable(void) { return m_head.edit_en; }

private:

	void	FillRandCode(void);	// ��һ���������RandCode����
	bool	CheckRandCode( const int * pData );// ���RandCode�����Ƿ���Ϲ���

private:
	
	struct{
		char	tag[10];	// "NoteBook\0"
		int		magic;		// ħ����

		EStatus	status;		// ״̬

		int		tryTimes;	// ���볢��ʧ�ܴ���
		time_t	lastLock;	// �ϴα���ʱ��

		int		startPos;	// ����������ʼ��ַ
		int		dataLen;	// �������ݳ���

		int		m_acNotePwd[3];	// ����У�鴮
		char	key;
		char	pwd[PWD_LEN];	// �����ݴ�

		bool	append_en;	// ׷��ʹ��
		bool	read_en;	// �Ķ�ʹ��
		bool	edit_en;	// �༭ʹ��

		int		m_acPrivPwd[3];
		int 	m_acUnlockPwd[3];

	}			m_head;

	FILE		*m_pFile;
	char		m_acNoteFile[MAX_FILENAME_LEN];	// ��־�ļ���
} ;
