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
	
	//��������
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

	//ͨ�����óߴ����
	void Draw();
	//ͨ�������������
	void Draw(float posx, float posy);
	//ͨ�������ߴ����
	void Draw(unsigned int width, unsigned int height); 

	//ƫ������
	void AddPos(float deltaX, float deltaY);
	//ƫ������
	void AddPos(const UVector2<float>& deltaPos);

	//��������
	void SetPos(UVector2<float> pos);
	//��������
	void SetPos(float posx, float posy);

	//��ȡ����ʾ��ͼ��ߴ�
	UVector2<unsigned int> GetSize() const;
	//��ȡԴͼ��ߴ�
	UVector2<unsigned int> GetSizeSrc() const;

	//��ȡ����ʾ��ͼ����
	unsigned int GetWidth() const;
	//��ȡ����ʾ��ͼ��߶�
	unsigned int GetHeight() const;
	//��ȡԴͼ����
	unsigned int GetWidthSrc() const;
	//��ȡԴͼ����
	unsigned int GetHeightSrc() const;

	//���ô���ʾ��ͼ��ߴ�
	void SetSize(UVector2<int> size);
	//���ô���ʾ��ͼ��ߴ�
	void SetSize(unsigned int width, unsigned int height);
	//���ô���ʾ��ͼ����
	void SetWidth(unsigned int width);
	//���ô���ʾ��ͼ��߶�
	void SetHeight(unsigned int height);

	//��ȡ��ǰ͸����(Get current transparency)
	int GetTransparency() const;
	//����͸����
	void SetTransparency(int transparency);

	//��������͸��
	void OpenBkTransparent();
	//�رձ���͸��
	void CloseBkTransparent();

	//���û�����״̬
	void SetPaintLock(bool paintLock);
	//�������²ü���
	void InvalidateNow();

	//ͼ��ת
	void Flip(FlipType flipType);
	//ͼ����ת
	void Rotate(float rotationAngle);

	//��ȡ��ת�Ƕ�
	float GetRotationAngle() const;

protected:
		
	int transparency_; //͸����
	int alphaFormat_; //alphaͨ����ʽ
	bool paintLock_; //������
	float posx_, posy_; //λ��
	float rotationAngle_; //��ǰ��ת��
	
	unsigned int widthSrc_, heightSrc_; //Դͼ�ߴ�
	unsigned int widthDst_, heightDst_; //Ŀ��ͼ�ߴ�

	HWND hWnd_; //���󶨴��ھ��
	HDC hdc_; //���󶨴����ڴ�DC
	HDC hMemDC_; //�ڴ�DC
	HDC hTmpMemDC_; //��ʱ�ڴ�DC������δ������ת��ͼ��
	HBITMAP hBitmap_; //λͼ���
	RECT rectNow_; //��λ�òü�����
	RECT rectBefore_; //��λ�òü�����

protected:

	void ImageScaling();
	void InvalidateTool();

	virtual void ObservedFunc(NotifyType notify) override {}
};