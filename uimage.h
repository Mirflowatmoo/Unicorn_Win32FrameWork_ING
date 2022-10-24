#pragma once

#include <Windows.h>
#include "unicorn.h"

class UImage : public UObserved<NotifyType> {
public:

	UImage(const wchar_t* filename, unsigned int width = 0, unsigned int height = 0);
	UImage(const UImage& image);
	UImage& operator=(const UImage& image);

	UImage(HBITMAP hBitmap, unsigned int width = 0, unsigned int height = 0);

	~UImage();
	
	//关联窗口
	template <typename DERIVED_TYPE>
	bool BindUWindow(UWindow<DERIVED_TYPE>& uwindow, bool isBindObserver = true) {

		try {
			rectNow_ = rectBefore_ = { 0, 0, static_cast<LONG>(widthDst_), static_cast<LONG>(heightDst_) };
			hWnd_ = uwindow.GetHWnd();
			hdc_ = uwindow.GetMemDC();

			ImageScaling();

			if (isBindObserver) {
				uwindow.GetObserver().AddObserved(this);
			}
		}
		catch (std::exception& e) {
			return false;
		}

		return true;
	}

	//通过内置尺寸绘制
	void Draw();
	//通过给定坐标绘制
	void Draw(float posx, float posy);
	//通过给定尺寸绘制
	void Draw(unsigned int width, unsigned int height); 

	//偏移坐标
	void AddPos(float deltaX, float deltaY);
	//偏移坐标
	void AddPos(const UVector2<float>& deltaPos);

	//设置坐标
	void SetPos(UVector2<float> pos);
	//设置坐标
	void SetPos(float posx, float posy);

	//获取待显示的图像尺寸
	UVector2<unsigned int> GetSize() const;
	//获取源图像尺寸
	UVector2<unsigned int> GetSizeSrc() const;

	//获取待显示的图像宽度
	unsigned int GetWidth() const;
	//获取待显示的图像高度
	unsigned int GetHeight() const;
	//获取源图像宽度
	unsigned int GetWidthSrc() const;
	//获取源图像宽度
	unsigned int GetHeightSrc() const;

	//设置待显示的图像尺寸
	void SetSize(UVector2<int> size);
	//设置待显示的图像尺寸
	void SetSize(unsigned int width, unsigned int height);
	//设置待显示的图像宽度
	void SetWidth(unsigned int width);
	//设置待显示的图像高度
	void SetHeight(unsigned int height);

	//获取当前透明度(Get current transparency)
	int GetTransparency() const;
	//设置透明度
	void SetTransparency(int transparency);

	//开启背景透明
	void OpenBkTransparent();
	//关闭背景透明
	void CloseBkTransparent();

	//设置绘制锁状态
	void SetPaintLock(bool paintLock);
	//立即更新裁剪区
	void InvalidateNow();

	//图像翻转
	void Flip(FlipType flipType);
	//图像旋转
	void Rotate(float rotationAngle);

	//获取旋转角度
	float GetRotationAngle() const;

protected:
		
	int transparency_; //透明度
	int alphaFormat_; //alpha通道格式
	bool paintLock_; //绘制锁
	float posx_, posy_; //位置
	float rotationAngle_; //当前旋转角
	
	unsigned int widthSrc_, heightSrc_; //源图尺寸
	unsigned int widthDst_, heightDst_; //目标图尺寸

	HWND hWnd_; //所绑定窗口句柄
	HDC hdc_; //所绑定窗口内存DC
	HDC hMemDC_; //内存DC
	HDC hTmpMemDC_; //临时内存DC，保存未经过旋转的图像
	HBITMAP hBitmap_; //位图句柄
	RECT rectNow_; //新位置裁剪矩形
	RECT rectBefore_; //旧位置裁剪矩形

protected:

	void ImageScaling();
	void InvalidateTool();

	virtual void ObservedFunc(NotifyType notify) override {}
};