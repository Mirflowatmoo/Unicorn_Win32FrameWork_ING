#pragma once
#include <tchar.h>

constexpr float PI = 3.1415926f; //Բ����

//���;�����
struct URect {
	int left;
	int top;
	int right;
	int bottom;
};

//˫���ȸ�����������
struct URect_D {
	double left;
	double top;
	double right;
	double bottom;
};

//�����ȸ�����������
struct URect_F {
	float left;
	float top;
	float right;
	float bottom;
};

//��ά������ģ��
template <class Type>
class UVector2 {
public:
	Type x;
	Type y;

public:
	UVector2(Type _x, Type _y) : x(_x), y(_y) { }

};

//��ά������ģ��
template <class Type>
class UVector3 {
public:
	Type x;
	Type y;
	Type z;

public:
	UVector3(Type _x, Type _y, Type _z) : x(_x), y(_y), z(_z) { }

};

//Button�����ṹ��
struct BUTTON_DESC {
	LPCTSTR pImgFile; //ͼƬ·��
	int posx; //ͼƬ��ʼx����
	int posy; //ͼƬ��ʼy����
	unsigned int width; //ͼƬ���
	unsigned int height; //ͼƬ�߶�
	unsigned int transparency; //ͼƬ͸����
};

//������ö����
enum class Trigger : int {
	NOTRIGGER = 0, //������
	END, //״̬����ʱ����
	NOW //���̴���
};

//�����������ṹ
struct Association {
	bool associated;
	Trigger* trigger;
};

//�ؼ�IDö����
enum class IDC : int {
	IDC_BUTTON = 1,
	IDC_EDIT
};

//��ť��Ϣö����
enum class UBUTTON_MSG : int {
	NOMSG = 0, //����밴ť�޹���
	MOUSE_IN, //���λ�ڰ�ť��
	MOUSE_LDOWN, //����������
	MOUSE_LUP, //������̧��
	MOUSE_RDOWN, //����Ҽ�����
	MOUSE_RUP //����Ҽ�̧��
};

//������Ϣö����
enum class UWINDOW_MSG : int {
	NOMSG = 0, //��ʼֵ
	QUIT, //�˳�����
	MOUSE_LDOWN, //����������
	MOUSE_LUP, //������̧��
	MOUSE_RDOWN, //����Ҽ�����
	MOUSE_RUP, //����Ҽ�̧��
	MOUSE_MDOWN, //��껬�ּ�������
	MOUSE_MUP, //��껬�ּ�̧��
	MOUSE_LDCLICK, //������˫��
	MOUSE_RDCLICK, //����Ҽ�˫��
	MOUSE_MDCLICK, //��껬�ּ�˫��
	MOUSEWHEEL_RUSH, //����ǰ��
	MOUSEWHEEL_BACK //���ֻ���
};

//��ת����ö����
enum class FlipType : int {
	HORIZONTAL = 0,
	VERTICAL
};

//֪ͨ����ö����
enum class NotifyType : int {
	INVISIBLE = 0,
	VISIBLE,
	REFRESH_ONLY
};

