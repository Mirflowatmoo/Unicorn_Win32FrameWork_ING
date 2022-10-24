#include "uimage.h"
#include <vector>
#include <cmath>
#pragma comment(lib, "MSIMG32.lib")

UImage::UImage(const wchar_t* filename, unsigned int width, unsigned int height) : hWnd_(NULL), hdc_(NULL), hMemDC_(NULL), hTmpMemDC_(NULL), hBitmap_(NULL) {

	rectNow_ = rectBefore_ = { 0L };
	posx_ = posy_ = 0.0f;
	rotationAngle_ = 0.0f;
	widthDst_ = width;
	heightDst_ = height;
	widthSrc_ = heightSrc_ = 0;
	transparency_ = 255;
	alphaFormat_ = 0;

	paintLock_ = true;

    // The factory pointer
    IWICImagingFactory* pFactory = NULL;
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory, //��ʶ
        NULL, //?
        CLSCTX_INPROC_SERVER, //��
        IID_PPV_ARGS(&pFactory) //����ʵ��ָ��
    );

    IWICBitmapDecoder* pDecoder = NULL;


	if (SUCCEEDED(hr)) {
		//ͨ���ļ�������������
		hr = pFactory->CreateDecoderFromFilename(
			filename,                      // Image to be decoded
			NULL,                            // Do not prefer a particular vendor
			GENERIC_READ,                    // Desired read access to the file
			WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
			&pDecoder                        // Pointer to the decoder
		);
	}
	
	IWICBitmapFrameDecode* pTempFDecoder = NULL; //WIC�ӿ�: ����λͼ֡
	IWICBitmapFrameDecode* pFDecoder = NULL;
	//IWICBitmapFlipRotator* pIFlipRotator = NULL;

    if (SUCCEEDED(hr)) {
        hr = pDecoder->GetFrame(0, &pTempFDecoder);
    }

	if (SUCCEEDED(hr)) {
		hr = WICConvertBitmapSource(GUID_WICPixelFormat32bppBGRA, (IWICBitmapSource*)pTempFDecoder, (IWICBitmapSource**)&pFDecoder);
	}
	
	/*if (SUCCEEDED(hr)) {
		hr = pFactory->CreateBitmapFlipRotator(&pIFlipRotator);
	}

	if (SUCCEEDED(hr)) {
		hr = pIFlipRotator->Initialize(pFDecoder, WICBitmapTransformRotate90);
	}*/

	if (SUCCEEDED(hr)) {
		hr = pFDecoder->GetSize(&widthSrc_, &heightSrc_);
	}
    
    std::vector<BYTE> buffer(widthSrc_ * heightSrc_ * 4); //����Ϊ32λλͼ

	if (SUCCEEDED(hr)) {
		hr = pFDecoder->CopyPixels(0, widthSrc_ * 4, buffer.size(), buffer.data());
	}

	if (SUCCEEDED(hr)) {
		hBitmap_ = CreateBitmap(widthSrc_, heightSrc_, 1, 32, buffer.data());
		//����û������ͼ��ߴ��������0����Ŀ��ߴ����Դͼ�ߴ�
		if (0 == widthDst_ || 0 == heightDst_) {
			widthDst_ = widthSrc_;
			heightDst_ = heightSrc_;
		}
	}

	pFactory->Release();
	pDecoder->Release();
	pFDecoder->Release();
	pTempFDecoder->Release();
	//pIFlipRotator->Release();
};

UImage::UImage(HBITMAP hBitmap, unsigned int width, unsigned int height) : hWnd_(NULL), hdc_(NULL), hMemDC_(NULL), hTmpMemDC_(NULL), hBitmap_(NULL) {

	rectNow_ = rectBefore_ = { 0L };
	posx_ = posy_ = 0.0f;
	rotationAngle_ = 0.0f;
	widthDst_ = width;
	heightDst_ = height;
	widthSrc_ = width;
	heightSrc_ = height;
	transparency_ = 255;
	alphaFormat_ = 0;
	paintLock_ = true;
	hBitmap_ = hBitmap;
	widthDst_ = width;
	heightDst_ = height;
}

UImage::UImage(const UImage& image) : 
	rectNow_(image.rectNow_), rectBefore_(image.rectBefore_)
{
	posx_ = image.posx_;
	posy_ = image.posy_;
	rotationAngle_ = image.rotationAngle_;
	widthDst_ = image.widthDst_;
	heightDst_ = image.heightDst_;
	widthSrc_ = image.widthSrc_;
	heightSrc_ = image.heightSrc_;
	transparency_ = image.transparency_;
	alphaFormat_ = image.alphaFormat_;
	paintLock_ = image.paintLock_;
	hWnd_ = image.hWnd_;
	hdc_ = image.hdc_; 

	BITMAP bmp = { 0 };
	HBITMAP hBmp1, hBmp2;

	hTmpMemDC_ = CreateCompatibleDC(image.hTmpMemDC_);
	hMemDC_ = CreateCompatibleDC(image.hMemDC_);

	GetObject(image.hBitmap_, sizeof(bmp), &bmp);
	SelectObject(hTmpMemDC_, image.hBitmap_);
	hBitmap_ = CreateBitmap(bmp.bmWidth, bmp.bmHeight, bmp.bmPlanes, bmp.bmBitsPixel, NULL);
	SelectObject(hMemDC_, hBitmap_);
	BitBlt(hMemDC_, image.widthDst_, image.heightDst_, 0, 0, hTmpMemDC_, 0, 0, SRCCOPY);

	hBmp1 = CreateCompatibleBitmap(image.hMemDC_, image.widthDst_, image.heightDst_);
	hBmp2 = CreateCompatibleBitmap(image.hTmpMemDC_, image.widthDst_, image.heightDst_);

	SelectObject(hTmpMemDC_, image.hBitmap_);
	SelectObject(hMemDC_, hBitmap_);
	
	BitBlt(hMemDC_, image.widthDst_, image.heightDst_, 0, 0, image.hMemDC_, 0, 0, SRCCOPY);
	BitBlt(hTmpMemDC_, image.widthDst_, image.heightDst_, 0, 0, image.hTmpMemDC_, 0, 0, SRCCOPY);

}

UImage& UImage::operator=(const UImage& image) {
	posx_ = image.posx_;
	posy_ = image.posy_;
	rotationAngle_ = image.rotationAngle_;
	widthDst_ = image.widthDst_;
	heightDst_ = image.heightDst_;
	widthSrc_ = image.widthSrc_;
	heightSrc_ = image.heightSrc_;
	transparency_ = image.transparency_;
	alphaFormat_ = image.alphaFormat_;
	paintLock_ = image.paintLock_;
	hWnd_ = image.hWnd_;
	hdc_ = image.hdc_;
	rectNow_ = image.rectNow_;
	rectBefore_ = image.rectBefore_;

	BITMAP bmp = { 0 };
	HBITMAP hBmp1, hBmp2;

	hTmpMemDC_ = CreateCompatibleDC(image.hTmpMemDC_);
	hMemDC_ = CreateCompatibleDC(image.hMemDC_);

	GetObject(image.hBitmap_, sizeof(bmp), &bmp);
	HBITMAP hBmpOld1 = (HBITMAP)SelectObject(hTmpMemDC_, image.hBitmap_);

	DeleteObject(hBitmap_);
	hBitmap_ = CreateBitmap(bmp.bmWidth, bmp.bmHeight, bmp.bmPlanes, bmp.bmBitsPixel, NULL);

	HBITMAP hBmpOld2 = (HBITMAP)SelectObject(hMemDC_, hBitmap_);

	BitBlt(hMemDC_, image.widthDst_, image.heightDst_, 0, 0, hTmpMemDC_, 0, 0, SRCCOPY);

	hBmp1 = CreateCompatibleBitmap(image.hMemDC_, image.widthDst_, image.heightDst_);
	hBmp2 = CreateCompatibleBitmap(image.hTmpMemDC_, image.widthDst_, image.heightDst_);

	SelectObject(hTmpMemDC_, image.hBitmap_);
	SelectObject(hMemDC_, hBitmap_);

	BitBlt(hMemDC_, image.widthDst_, image.heightDst_, 0, 0, image.hMemDC_, 0, 0, SRCCOPY);
	BitBlt(hTmpMemDC_, image.widthDst_, image.heightDst_, 0, 0, image.hTmpMemDC_, 0, 0, SRCCOPY);

	DeleteObject(hBmpOld1);
	DeleteObject(hBmpOld2);

	return *this;
}

UImage::~UImage() {
	HBITMAP hBmpOld1 = (HBITMAP)SelectObject(hMemDC_, NULL);
	HBITMAP hBmpOld2 = (HBITMAP)SelectObject(hTmpMemDC_, NULL);

	DeleteDC(hMemDC_);
	DeleteDC(hTmpMemDC_);
	DeleteObject(hBmpOld1);
	DeleteObject(hBmpOld2);
	DeleteObject(hBitmap_);
}

void UImage::ImageScaling() {
	//dcɾ����ʱ�����������bitmap�ǲ��ᱻ�ͷŵģ�
	//����ͨ��SelectObject��ѡ����bitmap���ͷų������ٷֱ����DeleteDC��DeleteObject
	DeleteDC(hMemDC_);
	DeleteDC(hTmpMemDC_);

	HDC hTmpDC = CreateCompatibleDC(hdc_);
	SelectObject(hTmpDC, hBitmap_);

	hMemDC_ = CreateCompatibleDC(hTmpDC);
	hTmpMemDC_ = CreateCompatibleDC(hTmpDC);

	HBITMAP hTmpBmp = CreateCompatibleBitmap(hdc_, widthDst_, heightDst_);
	HBITMAP hTmpBmp2 = CreateCompatibleBitmap(hdc_, widthDst_, heightDst_);
	HBITMAP hBmpOld1 = (HBITMAP)SelectObject(hMemDC_, hTmpBmp);
	HBITMAP hBmpOld2 = (HBITMAP)SelectObject(hTmpMemDC_, hTmpBmp2);

	//HALFTONE�Ǹ�����ģʽ�����ò����ķ�ʽ
	//��ͼƬ�Ŵ�ʱ��ʹ��Ĭ��ģʽ��ʹ��HALFTONEģʽͼ�����������������1��������
	//ͼƬ��Сʱ��Ĭ��ģʽ�����ܲ���ʧ�棬�ʷŴ����Ĭ�ϣ���С����HALFTONE
	//���ﲻ��Ƶ���仯������Ĭ��ȫ�ø�����ģʽ
	int areaDst = widthDst_ * heightDst_;
	int areaSrc = widthSrc_ * heightSrc_;

	if (areaDst < areaSrc) {
		SetStretchBltMode(hMemDC_, COLORONCOLOR);
		SetBrushOrgEx(hMemDC_, 0, 0, NULL);
		SetStretchBltMode(hTmpMemDC_, COLORONCOLOR);
		SetBrushOrgEx(hTmpMemDC_, 0, 0, NULL);
	}

	StretchBlt(hMemDC_, 0, 0, widthDst_, heightDst_, hTmpDC, 0, 0, widthSrc_, heightSrc_, SRCCOPY);
	StretchBlt(hTmpMemDC_, 0, 0, widthDst_, heightDst_, hTmpDC, 0, 0, widthSrc_, heightSrc_, SRCCOPY);

	DeleteDC(hTmpDC);
	DeleteObject(hBmpOld1);
	DeleteObject(hBmpOld2);
}

void UImage::Draw() {

	if (NULL == hdc_ || NULL == hWnd_ || NULL == hMemDC_) {
		return;
	}
	/*_int64 t1, t2, t3;
	QueryPerformanceCounter((LARGE_INTEGER*)&t1);
	QueryPerformanceCounter((LARGE_INTEGER*)&t2);
	std::cout << t2 - t1 << std::endl;*/

	if (255 == transparency_ && 0 == alphaFormat_) {
		BitBlt(hdc_, static_cast<int>(posx_), static_cast<int>(posy_), widthDst_, heightDst_, hMemDC_, 0, 0, SRCCOPY);

		return;
	}

	// �ṹ��ĵ�������Ա��ʾ�����͸���ȣ�0 ��ʾȫ͸����255 ��ʾ��͸����
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, static_cast<BYTE>(transparency_), static_cast<BYTE>(alphaFormat_) };

	AlphaBlend(
		hdc_, //�豸�����ľ��
		static_cast<int>(posx_), //���Ƶ�x����
		static_cast<int>(posy_),  //���Ƶ�y����
		widthDst_, //����ѡ�豸�л���ͼ��Ŀ��
		heightDst_, //����ѡ�豸�л���ͼ��ĸ߶�
		hMemDC_, //ͼ���豸�����ľ��
		0, 0, //����ͼ���������
		widthDst_, //����ͼ��Ŀ��
		heightDst_, //����ͼ��ĸ߶�
		bf
	);

}

void UImage::Draw(float posx, float posy) {

	if (NULL == hdc_ || NULL == hWnd_ || NULL == hMemDC_) {
		return;
	}
	
	posx_ = posx;
	posy_ = posy;

	if (255 == transparency_ && 0 == alphaFormat_) {
		BitBlt(hdc_, static_cast<int>(posx), static_cast<int>(posy), widthDst_, heightDst_, hMemDC_, 0, 0, SRCCOPY);

		return;
	}

	// �ṹ��ĵ�������Ա��ʾ�����͸���ȣ�0 ��ʾȫ͸����255 ��ʾ��͸����
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, static_cast<BYTE>(transparency_), static_cast<BYTE>(alphaFormat_) };

	AlphaBlend(
		hdc_, //�豸�����ľ��
		static_cast<int>(posx), //���Ƶ�x����
		static_cast<int>(posy),  //���Ƶ�y����
		widthDst_, //����ѡ�豸�л���ͼ��Ŀ��
		heightDst_, //����ѡ�豸�л���ͼ��ĸ߶�
		hMemDC_, //ͼ���豸�����ľ��
		0, 0, //����ͼ���������
		widthDst_, //����ͼ��Ŀ��
		heightDst_, //����ͼ��ĸ߶�
		bf
	);

}

void UImage::Draw(unsigned int width, unsigned int height) {
	
	if (NULL == hdc_ || NULL == hWnd_ || NULL == hMemDC_ || (width == widthDst_ && height == heightDst_)) {
		return;
	}

	widthDst_ = width;
	heightDst_ = height;

	ImageScaling();
	BitBlt(hdc_, static_cast<int>(posx_), static_cast<int>(posy_), widthDst_, heightDst_, hMemDC_, 0, 0, SRCCOPY);
}

void UImage::AddPos(float deltaX, float deltaY) {

	RECT tempRect = rectNow_;

	posx_ += deltaX;
	posy_ += deltaY;
	rectNow_.left = (int)posx_;
	rectNow_.right = int(posx_ + static_cast<float>(widthDst_));
	rectNow_.top = (int)posy_;
	rectNow_.bottom = int(posy_ + static_cast<float>(heightDst_));

	//����ȣ�˵����Ҫ�ػ�
	if (memcmp(&tempRect, &rectNow_, sizeof(tempRect)) != 0) {

		paintLock_ = true;
		InvalidateTool();

	}

}

void UImage::AddPos(const UVector2<float>& deltaPos) {
	AddPos(deltaPos.x, deltaPos.y);
}

int UImage::GetTransparency() const {
	return transparency_;
}

void UImage::SetTransparency(int transparency) {
	transparency_ = transparency;
}

void UImage::OpenBkTransparent() {
	alphaFormat_ = 0x01; //AC_SRC_ALPHA
}

void UImage::CloseBkTransparent() {
	alphaFormat_ = 0;
}

void UImage::SetPos(UVector2<float> pos) {
	posx_ = pos.x;
	posy_ = pos.y;
	InvalidateRect(hWnd_, &rectBefore_, TRUE);
	InvalidateRect(hWnd_, &rectNow_, TRUE);
}

void UImage::SetPos(float posx, float posy) {
	posx_ = posx;
	posy_ = posy;
	InvalidateRect(hWnd_, &rectBefore_, TRUE);
	InvalidateRect(hWnd_, &rectNow_, TRUE);
}

UVector2<unsigned int> UImage::GetSize() const {
	return UVector2<unsigned int>(widthDst_, heightDst_);
}

UVector2<unsigned int> UImage::GetSizeSrc() const {
	return UVector2<unsigned int>(widthSrc_, heightSrc_);
}

unsigned int UImage::GetWidth() const {
	return widthDst_;
}

unsigned int UImage::GetHeight() const {
	return heightDst_;
}

unsigned int UImage::GetWidthSrc() const {
	return widthSrc_;
}

unsigned int UImage::GetHeightSrc() const {
	return heightSrc_;
}

void UImage::SetSize(UVector2<int> size) {
	widthDst_ = size.x;
	heightDst_ = size.y;
	ImageScaling();
}

void UImage::SetSize(unsigned int width, unsigned int height) {
	widthDst_ = width;
	heightDst_ = height;
	ImageScaling();
}

void UImage::SetWidth(unsigned int width) {
	widthDst_ = width;
	ImageScaling();
}

void UImage::SetHeight(unsigned int height) {
	heightDst_ = height;
	ImageScaling();
}

void UImage::SetPaintLock(bool paintLock) {
	paintLock_ = paintLock;
}

void UImage::InvalidateNow() {
	paintLock_ = true;
	InvalidateTool();
}

void UImage::InvalidateTool() {

	if (paintLock_) {

		RECT rectRight = rectNow_;
		RECT rectLeft = rectBefore_;

		rectBefore_ = rectNow_;
		paintLock_ = false;
		
		if (rectBefore_.right > rectNow_.right) {
			RECT tempRect = rectRight;
			rectRight = rectLeft;
			rectLeft = tempRect;
		}

		InvalidateRect(hWnd_, &rectLeft, TRUE);
		//������ཻ��ֱ����Ϊ��Ч��
		if (rectRight.left >= rectLeft.right || rectRight.bottom <= rectLeft.top || rectRight.top >= rectLeft.bottom) {
			InvalidateRect(hWnd_, &rectBefore_, TRUE);
			InvalidateRect(hWnd_, &rectNow_, TRUE);
			return;
		}

		//���ü����ཻ(���ƽ�У�V�ػ�H����û�������)
		RECT rectV = { rectLeft.right, rectRight.top, rectRight.right, rectRight.bottom };
		RECT rectH = { rectRight.left, rectRight.top, rectLeft.right, rectLeft.bottom };

		if (rectLeft.bottom < rectRight.bottom) {
			rectH.top = rectLeft.bottom;
			rectH.bottom = rectRight.top;
		}

		InvalidateRect(hWnd_, &rectV, TRUE);
		InvalidateRect(hWnd_, &rectH, TRUE);
	
	}
}

void UImage::Flip(FlipType flipType) {
	
	POINT points[3] = {
		{ static_cast<long>(posx_) + static_cast<long>(widthDst_), static_cast<long>(posy_) },
		{ static_cast<long>(posx_), static_cast<long>(posy_) },
		{ static_cast<long>(posx_) + static_cast<long>(widthDst_), static_cast<long>(posy_) + static_cast<long>(heightDst_) }
	};

	if (FlipType::VERTICAL == flipType) {
		points[0] = { static_cast<long>(posx_), static_cast<long>(posy_) + static_cast<long>(heightDst_) };
		points[1] = { static_cast<long>(posx_) + static_cast<long>(widthDst_), static_cast<long>(posy_) + static_cast<long>(heightDst_) };
		points[2] = { static_cast<long>(posx_), static_cast<long>(posy_) };
	}
	
	HDC hTmpDC = CreateCompatibleDC(hMemDC_);
	SelectObject(hTmpDC, hBitmap_);

	RECT rect = { 0L, 0L, static_cast<long>(widthDst_), static_cast<long>(heightSrc_) };
	FillRect(hMemDC_, &rect, (HBRUSH)NULL_BRUSH);

	PlgBlt(hMemDC_, points, hTmpDC, 0, 0, widthDst_, heightDst_, NULL, 0, 0);
	
	DeleteDC(hTmpDC);
}

void UImage::Rotate(float rotationAngle) {
	rotationAngle_ += rotationAngle;
	POINT points[3];
	float factorCos = cos(rotationAngle_ / 180.0f * PI);
	float factorSin = sin(rotationAngle_ / 180.0f * PI);
	int factorHalfWidth = widthDst_ >> 1;
	int factorHalfHeight = heightDst_ >> 1;
	
	points[0].x = static_cast<int>(-factorHalfWidth * factorCos + factorHalfHeight * factorSin + factorHalfWidth);
	points[0].y = static_cast<int>(-factorHalfHeight * factorCos - factorHalfWidth * factorSin + factorHalfHeight);
	points[1].x = static_cast<int>(factorHalfWidth * factorCos + factorHalfHeight * factorSin + factorHalfWidth);
	points[1].y = static_cast<int>(-factorHalfHeight * factorCos + factorHalfWidth * factorSin + factorHalfHeight);
	points[2].x = static_cast<int>(-factorHalfWidth * factorCos - factorHalfHeight * factorSin + factorHalfWidth);
	points[2].y = static_cast<int>(factorHalfHeight * factorCos - factorHalfWidth * factorSin + factorHalfHeight);

	RECT rect = { 0L, 0L, static_cast<long>(widthDst_), static_cast<long>(heightSrc_) };
	FillRect(hMemDC_, &rect, (HBRUSH)NULL_BRUSH);
	//��ԭͼDC����ת
	PlgBlt(hMemDC_, points, hTmpMemDC_, 0, 0, widthDst_, heightDst_, NULL, 0, 0);
}

float UImage::GetRotationAngle() const {
	return rotationAngle_;
}