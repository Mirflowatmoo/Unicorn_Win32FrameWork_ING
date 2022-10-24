#include "uanimation.h"

UAnimation::UAnimation(const wchar_t* filename, unsigned int width, unsigned int height) {

	isEnd = false;
	timerAcc_ = 0.0f;
	frameIntervalTime_ = 0.05f;
	frameIndex_ = 0;
	frameCounts_ = 0;
	widthDst_ = width;
	heightDst_ = height;
	widthSrc_ = heightSrc_ = 0;

	IWICImagingFactory* pFactory = NULL;
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory, //标识
		NULL, //?
		CLSCTX_INPROC_SERVER, //？
		IID_PPV_ARGS(&pFactory) //返回实例指针
	);

	IWICBitmapDecoder* pDecoder = NULL;

	if (SUCCEEDED(hr)) {
		//通过文件名创建解码器
		hr = pFactory->CreateDecoderFromFilename(
			filename,                      // Image to be decoded
			NULL,                            // Do not prefer a particular vendor
			GENERIC_READ,                    // Desired read access to the file
			WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
			&pDecoder                        // Pointer to the decoder
		);
	}

	if (SUCCEEDED(hr)) {
		hr = pDecoder->GetFrameCount(&frameCounts_);
	}

	if (SUCCEEDED(hr)) {

		IWICBitmapFrameDecode* pTempFDecoder = NULL; //WIC接口: 解码位图帧
		IWICBitmapFrameDecode* pFDecoder = NULL;

		for (unsigned int index = 0U; index < frameCounts_; ++index) {
			if (SUCCEEDED(hr)) {
				hr = pDecoder->GetFrame(index, &pTempFDecoder);
			}

			if (SUCCEEDED(hr)) {
				hr = WICConvertBitmapSource(GUID_WICPixelFormat32bppBGRA, (IWICBitmapSource*)pTempFDecoder, (IWICBitmapSource**)&pFDecoder);
			}

			if (SUCCEEDED(hr)) {
				hr = pFDecoder->GetSize(&widthSrc_, &heightSrc_);
			}

			std::vector<BYTE> buffer(widthSrc_ * heightSrc_ * 4); //加载为32位位图

			if (SUCCEEDED(hr)) {
				hr = pFDecoder->CopyPixels(0, widthSrc_ * 4, buffer.size(), buffer.data());
			}

			if (SUCCEEDED(hr)) {

				HBITMAP hBitmap = CreateBitmap(widthSrc_, heightSrc_, 1, 32, buffer.data());

				animationFrames_.push_back(std::make_unique<UImage>(hBitmap, widthSrc_, heightSrc_));

				//如果用户传入的图像尺寸参数存在0，则目标尺寸就是源图尺寸
				if (0 == widthDst_ || 0 == heightDst_) {
					widthDst_ = widthSrc_;
					heightDst_ = heightSrc_;
				}
			}

			pTempFDecoder->Release();
			pFDecoder->Release();
			pTempFDecoder = NULL;
			pFDecoder = NULL;
		}
	}

	pFactory->Release();
	pDecoder->Release();
}

UAnimation::UAnimation(const UAnimation& animation) {
	isEnd = false;
	timerAcc_ = 0.0f;
	frameIntervalTime_ = animation.frameIntervalTime_;
	frameIndex_ = 0;
	frameCounts_ = animation.frameCounts_;
	widthDst_ = animation.widthDst_;
	heightDst_ = animation.heightDst_;
	widthSrc_ = animation.widthSrc_;
	heightSrc_ = animation.heightSrc_;

	int size = animation.animationFrames_.size();

	for (int index = 0; index < size; ++index) {
		animationFrames_.push_back(std::make_unique<UImage>(*(animation.animationFrames_[index])));
	}

}

UAnimation& UAnimation::operator=(const UAnimation& animation) {
	timerAcc_ = 0.0f;
	frameIntervalTime_ = animation.frameIntervalTime_;
	frameIndex_ = 0;
	frameCounts_ = animation.frameCounts_;
	widthDst_ = animation.widthDst_;
	heightDst_ = animation.heightDst_;
	widthSrc_ = animation.widthSrc_;
	heightSrc_ = animation.heightSrc_;

	int size = animation.animationFrames_.size();

	animationFrames_.clear();

	for (int index = 0; index < size; ++index) {
		animationFrames_.push_back(std::make_unique<UImage>(*(animation.animationFrames_[index])));
	}

	return *this;
}

void UAnimation::ObservedFunc(NotifyType notify) {
	timer_.Tick();
	timerAcc_ += timer_.DeltaTime();
	isEnd = false;

	if (timerAcc_ > frameIntervalTime_) {
		isEnd = true;
		timerAcc_ = 0.0f;
		frameIndex_ = (frameIndex_ + 1) % frameCounts_;
		animationFrames_[frameIndex_]->SetPaintLock(true);
		animationFrames_[frameIndex_]->InvalidateNow();
	}
}

void UAnimation::Play() {
	animationFrames_[frameIndex_]->Draw();
}

void UAnimation::Reset() {
	frameIndex_ = 0;
	timer_.Reset();
	timerAcc_ = 0.0f;

	/*for (auto& item : animationFrames_) {
		item->Rotate(flipType);
	}*/
}

void UAnimation::Flip(FlipType flipType) {
	for (auto& item : animationFrames_) {
		item->Flip(flipType);
	}
}

void UAnimation::Rotate(float rotationAngle) {
	//animationFrames_[frameIndex_]->Rotate(rotationAngle);
	for (auto& item : animationFrames_) {
		item->Rotate(rotationAngle);
	}
}

void UAnimation::AddPos(float deltaX, float deltaY) {
	for (auto& item : animationFrames_) {
		item->AddPos(deltaX, deltaY);
	}
}

void UAnimation::AddPos(const UVector2<float>& deltaPos) {
	for (auto& item : animationFrames_) {
		item->AddPos(deltaPos);
	}
}

void UAnimation::SetPos(float deltaX, float deltaY) {
	for (auto& item : animationFrames_) {
		item->SetPos(deltaX, deltaY);
	}
}

void UAnimation::SetPos(const UVector2<float>& deltaPos) {
	for (auto& item : animationFrames_) {
		item->SetPos(deltaPos);
	}
}

void UAnimation::SetFrameIntervalTime(float time) {
	frameIntervalTime_ = time;
}

unsigned int UAnimation::GetCurrentFrameIndex() const {
	return frameIndex_;
}

unsigned int UAnimation::GetFrameCounts() const {
	return frameCounts_;
}

float UAnimation::GetCurrentFrameRunTime() const {
	return timerAcc_;
}

bool UAnimation::GetCurrentFramePlayState() const {
	return isEnd;
}