#pragma once
#include <memory>
#include <list>
#include "unicorn.h"

template <typename ObsrvType>
class UObserved {
public:
	//virtual void ObservedHdc(HDC hdc) = 0;
	virtual void ObservedFunc(ObsrvType notify) = 0;
	virtual ~UObserved() = default;
};

template <typename ObsrvType>
class UObserver {
public:

	UObserver() = default;
	virtual ~UObserver() = default;
	
	void Notify(ObsrvType notify) {
		for (auto& item : ObservedPtrsList_) {
			item->ObservedFunc(notify);
		}
	};

	void AddObserved(UObserved<ObsrvType>* pObserved) {
		ObservedPtrsList_.push_back(pObserved);
	}

	void RemoveObeserved(UObserved<ObsrvType>* pObserved) {
		auto iter = std::find(ObservedPtrsList_.begin(), ObservedPtrsList_.end(), pObserved);
		if (iter != ObservedPtrsList_.end()) {
			ObservedPtrsList_.erase(iter);
		}
	}

	void ClearObeserved(UObserved<ObsrvType>* pObserved) {
		ObservedPtrsList_.clear();
	}

private:
	//���Ҫʹ�����Ա������Ӧ����list�����洢����ָ�룬�����Ǻ���ָ��
	std::list<UObserved<ObsrvType>*> ObservedPtrsList_;
	UObserver(const UObserver<ObsrvType>&) = delete;
	UObserver<ObsrvType>& operator=(const UObserver<ObsrvType>&) = delete;
};