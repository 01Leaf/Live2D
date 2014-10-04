#pragma once


//��Ƶ�ļ���ʽ
struct RIFF_HEADER//�ļ�����
{
	char szRiffID[4];
	DWORD dwRiffSize;
	char szRiffFormat[4];
};

struct WAVE_FORMAT
{
	WORD wFormatTag;
	WORD wChannels;
	DWORD dwSamplesPerSec;
	DWORD dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD wBitsPerSample;
};

struct FMT_BLOCK//��ʽ
{
	char szFmtID[4]; // 'f','m','t',' '
	DWORD dwFmtSize;//////////////һ�������Ϊ16�����и�����ϢΪ18
	WAVE_FORMAT wavFormat;
};

struct FACT_BLOCK//������ һ��ɲ���
{
	char szFactID[4]; // 'f','a','c','t'
	DWORD dwFactSize;
};

struct DATA_BLOCK//����ͷ
{
	char szDataID[4]; // 'd','a','t','a'
	DWORD dwDataSize;//���ݳ���
};

bool PlayModelSound(wchar_t path[],char p[],int index);
void  StopPlaySound();