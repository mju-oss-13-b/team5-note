#pragma once
#include "typedef.h"

// nMaxLen Ϊ��ȡ�������󳤶ȣ��������ִ���������
// ʵ�ʴ��� nMaxLen ��ֵ����Ϊpasswordʵ�ֿռ��1
int		GetPassword( char *password , int nMaxLen=PWD_LEN );
char*	ChangeFileSuffix( char *pOutFile, const char *pSrcFile, const char *pSuffix );
bool	GetUserChoice( void );
bool	GetNewPassword( char *password );	// ��ȡ������
