#include "LAppFontMananger.h"


extern HINSTANCE				g_ThreadID;
extern LPDIRECT3DDEVICE9  g_pD3DDevice;

//�ı���Ϣ
ID3DXFont* Font   = 0;
ID3DXSprite* Sprite = 0;
D3DXFONT_DESC lf; // Initialize a LOGFONT structure that describes the font

// we want to create.
LOGFONTW lf1;
RECT tt={0,0,40,20};
D3DCOLOR textcolor;
wchar_t message[500];

LAppFontMananger::LAppFontMananger()
{

}

LAppFontMananger::~LAppFontMananger()
{

}

HRESULT LAppFontMananger::InitFont()
{
	return D3DXCreateFontIndirect(g_pD3DDevice, &lf, &Font);// ��2�������ǽṹ��D3DXFONT_DESCA����
}

HRESULT LAppFontMananger::DrawMessageText()
{
	return Font->DrawText(
		Sprite,//����ʱ����ͨ���������ĸú�����6�����������˵�һ������ͷ���ϣ�����ΪID3DXSprite* Sprite = 0;
		(LPCWSTR)message, 
		-1, // size of string or -1 indicates null terminating string
		&tt,            // rectangle text is to be formatted to in windows coords
		DT_TOP | DT_LEFT, // draw in the top left corner of the viewport
		textcolor);      // black text
}


//��ʾ�ı�
bool LAppFontMananger::ShowMessage(HINSTANCE hinst, int x,int y,int width,int height,wchar_t * msg,
								   int fontHeight,int fontWidth,int fontWeight,bool italic,wchar_t * family,D3DCOLOR color)
{
	if(g_ThreadID==hinst)//��⵱ǰ���ý����Ƿ�Ϸ�
	{
		try{
			DeleteObject(Font);
			D3DXCreateFont(g_pD3DDevice,fontHeight,fontWidth,
				fontWeight,0,italic,DEFAULT_CHARSET,0,0,0,(LPCWSTR)family, &Font);// �����޷�ͨ�������ֵ�2�������ǽṹ��D3DXFONT_DESCA���ͣ����¶��岢��ֵ;
			tt.left=x;tt.top=y;tt.right=width;tt.bottom=height;
			textcolor=color;
			wcscpy(message,msg);
		}
		catch(...)
		{
			return false;
		}
		return true;
	}
	else
		return false;
}


