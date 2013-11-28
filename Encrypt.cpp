#include <string.h>
#include "Encrypt.h"
#include "VirtualArray.h"

/*
*********************************************************************************************
	���������ܺ��������Զ��������ݽ��м��ܡ�
	���룺	
		pData	Ҫ�������ݵ��׵�ַ��
		d_len	���ݵĳ���
		pPwd	����
		p_len	���볤��
	�����
		�ַ�����	���ܺ�������׵�ַ������data
	ע��
*********************************************************************************************
*/
char* CEncrypt::Left ( char *pData , int d_len , const char *pPwd , int p_len )
{
	if ( pData && pPwd && d_len > 0 && p_len > 0 ){

		char *pKey = new char [p_len];

		if (pKey){

			memcpy( pKey , pPwd , p_len );

			CVirtualArray vArray(pData, d_len, pKey, p_len);	// ��pData��pKey��vArray���ӳ�һ����������

			for ( int j = 1 ; j <= p_len ; j ++ ){
				for ( int i = 0 ; ( i + j ) != ( d_len + p_len ) ; i ++ ){
					vArray [i] ^= vArray [i+j] ;
				}
			}

			memset( pKey , 0 , p_len );
			delete [] pKey;
		}
	}
	return 	pData ;
}

/*
*********************************************************************************************
	������������ת����ת����
	���룺pKey		����
		  p_len		���볤��
	�����
		�ַ�����	���ܺ�������׵�ַ������data
*********************************************************************************************
*/
char* CEncrypt::GenKey ( char *pKey , int p_len )
{
	if ( pKey && p_len>0 ){
		for ( int j = 1; j <= p_len; j ++ ){
			for ( int i = 0 ; (i+j) < p_len; i++ ){
				pKey[i] ^= pKey[i+j];
			}
		}
	}
	return pKey;
}

/*
*********************************************************************************************
	���������ܺ��������Զ����Ľ��н��ܡ�
	���룺	
		pData	Ҫ�������ݵ��׵�ַ��
		d_len	���ݵĳ���
		pPwd	����
		p_len	���볤��
	�����
		�ַ�����	���ܺ�������׵�ַ������data
*********************************************************************************************
*/
char* CEncrypt::Right ( char *pData , int d_len , const char *pPwd , int p_len )
{
	if ( pData && pPwd && d_len > 0 && p_len > 0 ){
		
		char *pKey = new char[p_len];

		if ( pKey ){

			memcpy( pKey, pPwd, p_len );	// ��ΪpPwd�ǲ���д�ģ�����Ҫ����һ��pKey����pPwd���м���

			GenKey( pKey, p_len );	// ������ת����Կ��
			
			CVirtualArray vArray( pData, d_len, pKey, p_len );	// ��pData��pKey��vArray���ӳ�һ����������

			for ( int j = p_len ; j > 0 ; j -- ){	// �����㷨...
				for ( int i = d_len + p_len - 1 ; ( i - j ) >= 0 ; i -- ){
					vArray [i-j] ^= vArray [i] ;
				}
			}

			memset( pKey , 0 , p_len );	// ����Կ��
			delete [] pKey;
		}
	}
	return	pData ;
}
