#pragma once
#include <tchar.h>

constexpr float PI = 3.1415926f; //圆周率

//整型矩形类
struct URect {
	int left;
	int top;
	int right;
	int bottom;
};

//双精度浮点数矩形类
struct URect_D {
	double left;
	double top;
	double right;
	double bottom;
};

//单精度浮点数矩形类
struct URect_F {
	float left;
	float top;
	float right;
	float bottom;
};

//二维向量类模板
template <class Type>
class UVector2 {
public:
	Type x;
	Type y;

public:
	UVector2(Type _x, Type _y) : x(_x), y(_y) { }

};

//三维向量类模板
template <class Type>
class UVector3 {
public:
	Type x;
	Type y;
	Type z;

public:
	UVector3(Type _x, Type _y, Type _z) : x(_x), y(_y), z(_z) { }

};

//Button描述结构体
struct BUTTON_DESC {
	LPCTSTR pImgFile; //图片路径
	int posx; //图片初始x坐标
	int posy; //图片初始y坐标
	unsigned int width; //图片宽度
	unsigned int height; //图片高度
	unsigned int transparency; //图片透明度
};

//触发器枚举类
enum class Trigger : int {
	NOTRIGGER = 0, //不触发
	END, //状态结束时触发
	NOW //立刻触发
};

//触发器关联结构
struct Association {
	bool associated;
	Trigger* trigger;
};

//控件ID枚举类
enum class IDC : int {
	IDC_BUTTON = 1,
	IDC_EDIT
};

//按钮消息枚举类
enum class UBUTTON_MSG : int {
	NOMSG = 0, //鼠标与按钮无关联
	MOUSE_IN, //鼠标位于按钮内
	MOUSE_LDOWN, //鼠标左键按下
	MOUSE_LUP, //鼠标左键抬起
	MOUSE_RDOWN, //鼠标右键按下
	MOUSE_RUP //鼠标右键抬起
};

//窗口消息枚举类
enum class UWINDOW_MSG : int {
	NOMSG = 0, //初始值
	QUIT, //退出进程
	MOUSE_LDOWN, //鼠标左键按下
	MOUSE_LUP, //鼠标左键抬起
	MOUSE_RDOWN, //鼠标右键按下
	MOUSE_RUP, //鼠标右键抬起
	MOUSE_MDOWN, //鼠标滑轮键键按下
	MOUSE_MUP, //鼠标滑轮键抬起
	MOUSE_LDCLICK, //鼠标左键双击
	MOUSE_RDCLICK, //鼠标右键双击
	MOUSE_MDCLICK, //鼠标滑轮键双击
	MOUSEWHEEL_RUSH, //滑轮前进
	MOUSEWHEEL_BACK //滑轮回退
};

//翻转类型枚举类
enum class FlipType : int {
	HORIZONTAL = 0,
	VERTICAL
};

//通知类型枚举类
enum class NotifyType : int {
	INVISIBLE = 0,
	VISIBLE,
	REFRESH_ONLY
};

