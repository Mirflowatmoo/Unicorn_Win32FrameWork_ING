#pragma once
#include <Windows.h>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <numeric>
#include "unicorn.h"
#include "uimage.h"

class UAnimation : public UObserved<NotifyType> {
public:
	UAnimation(const wchar_t* filename, unsigned int width = 0, unsigned int height = 0);
	UAnimation(const UAnimation& animation);
	UAnimation& operator=(const UAnimation& animation);
	~UAnimation() { }

	//friend class UAnimationMachine;

	template <typename DERIVED_TYPE>
	bool BindUWindow(UWindow<DERIVED_TYPE>& uwindow) {

		uwindow.GetObserver().AddObserved(this);

		for (auto& item : animationFrames_) {

			item->SetSize(widthDst_, heightDst_);
			item->OpenBkTransparent();

			if (!item->BindUWindow(uwindow, false)) {
				return false;
			}
		}

		return true;
	}

	void Play();
	void Reset();
	void Flip(FlipType flipType);
	void Rotate(float rotationAngle);
	void AddPos(float deltaX, float deltaY);
	void AddPos(const UVector2<float>& deltaPos);
	void SetPos(float deltaX, float deltaY);
	void SetPos(const UVector2<float>& deltaPos);
	void SetFrameIntervalTime(float time);
	unsigned int GetCurrentFrameIndex() const;
	unsigned int GetFrameCounts() const;
	float GetCurrentFrameRunTime() const;
	bool GetCurrentFramePlayState() const;

protected:
	bool isEnd; //֡���Ž����ź�
	float timerAcc_; //ÿ֡ʱ���ۼ���
	float frameIntervalTime_; //֡���ʱ��
	unsigned int frameIndex_; //��ǰ֡����
	unsigned int frameCounts_; //֡����
	unsigned int widthSrc_, heightSrc_; //Դͼ�ߴ�
	unsigned int widthDst_, heightDst_; //Ŀ��ͼ�ߴ�

	UTimer timer_;
	std::vector<std::unique_ptr<UImage> > animationFrames_;

	virtual void ObservedFunc(NotifyType notify) override;
};

template <typename DERIVED_TYPE>
class UStateMachine {
public:
	UStateMachine(unsigned int capacity = 50U) {
		capacity_ = capacity;
		curStateIndex_ = 0;
		std::vector<Association> row(capacity, { false, nullptr });

		for (unsigned int index = 0U; index < capacity; ++index) {
			adjacencyMatrix_.push_back(row);
		}

	}

	UStateMachine(const UStateMachine<DERIVED_TYPE>&) = delete;
	UStateMachine<DERIVED_TYPE>& operator=(const UStateMachine<DERIVED_TYPE>&) = delete;
	virtual ~UStateMachine() = default;

	bool AddState(const DERIVED_TYPE& state, const char* stateName) {
		if (statesMap_.size() >= capacity_) {
			return false;
		}

		int index = -1;
		if (idleIndexQueue_.size() > 0) {
			//���ɵ�����ֵΪ�����ڽӾ���������
			index = idleIndexQueue_.pop();
		}
		else {
			//���ɵ�����ֵΪ״̬��״̬��
			index = statesMap_.size();
		}

		indexMap_.insert(std::pair<int, std::string>(stateName, index));
		statesMap_.insert(std::pair<int, std::string>(index, std::make_shared<DERIVED_TYPE>(state)));

		return true;
	}

	bool AddState(std::shared_ptr<DERIVED_TYPE> pState, const char* stateName) {
		if (statesMap_.size() >= capacity_) {
			return false;
		}

		int index = -1;
		if (idleIndexQueue_.size() > 0) {
			index = idleIndexQueue_.front();
			idleIndexQueue_.pop();
		}
		else {
			index = statesMap_.size();
		}

		indexMap_.insert(std::pair<std::string, int>(stateName, index));
		statesMap_.insert(std::pair<int, std::shared_ptr<DERIVED_TYPE> >(index, pState));
		
		return true;
	}

	bool RemoveState(const char* stateName) {
		try {
			int index = indexMap_.at((std::string)stateName);

			//������Ƴ�״̬�����й���
			for (int _index = 0; _index < capacity_; ++_index) {
				adjacencyMatrix_[_index][index] = { false, nullptr };
			}
			std::fill(adjacencyMatrix_[index].begin(), adjacencyMatrix_[index].end(), { false, nullptr });

			//�����Ƴ�״̬���䵽���ڽӾ���������ӽ�������������
			idleIndexQueue_.push(index);

			indexMap_.erase(); //�Ƴ�״̬����������ӳ��
			statesMap_.erase(); //�Ƴ�������״̬��ӳ��
		}
		catch (std::out_of_range e) {
			return false;
		}
		
		return true;
	}

	//״̬����
	bool Assocaite(const char* srcStateName, const char* dstStateName, Trigger* trigger) {
		try {
			int indexSrc = indexMap_.at((std::string)srcStateName);
			int indexDst = indexMap_.at((std::string)dstStateName);
			adjacencyMatrix_[indexSrc][indexDst] = { true, trigger };
		}
		catch (std::out_of_range e) {
			return false;
		}

		return true;

	}

	//���״̬����
	bool Disassocaite(const char* srcStateName, const char* dstStateName) {
		try {
			int indexSrc = indexMap_.at((std::string)srcStateName);
			int indexDst = indexMap_.at((std::string)dstStateName);
			adjacencyMatrix_[indexSrc][indexDst] = { false, nullptr };
		}
		catch (std::out_of_range e) {
			return false;
		}

		return true;
	}

	int GetCapacity() const {
		return capacity_;
	}

	size_t GetStatesQuantity() const {
		return statesMap_.size();
	}

	virtual void AddPos(float deltaX, float deltaY) = 0;
	virtual void AddPos(const UVector2<float>& deltaPos) = 0;

	virtual void SetPos(float deltaX, float deltaY) = 0;
	virtual void SetPos(const UVector2<float>& deltaPos) = 0;

	virtual void Run() = 0;

protected:
	int curStateIndex_; //��ǰ״̬����
	std::string curStateName_; //��ǰ״̬��
	unsigned int capacity_; //״̬������
	std::queue<int> idleIndexQueue_; //�Ƴ�״̬�����Ŀ����ڽӾ�������������
	std::map<std::string, int> indexMap_; //״̬���������� ӳ���
	std::vector<std::vector<Association> > adjacencyMatrix_; //�ڽӾ���
	std::map<int, std::shared_ptr<DERIVED_TYPE> > statesMap_; //��������״̬ ӳ���

};

class UAnimationStateMachine : public UStateMachine<UAnimation> {
public:
	UAnimationStateMachine(unsigned int statesQuantity = 50U) : UStateMachine<UAnimation>(statesQuantity) {
		
	}

	UAnimationStateMachine(const UAnimationStateMachine&) = delete;
	UAnimationStateMachine& operator=(const UAnimationStateMachine&) = delete;

	virtual void Run() override {
		
		if (statesMap_.size() <= 0) {
			return;
		}
		for (size_t index = 0; index < capacity_; ++index) {
			if (!adjacencyMatrix_[curStateIndex_][index].associated ||
				Trigger::NOTRIGGER == *adjacencyMatrix_[curStateIndex_][index].trigger)
			{
				continue;
			}
			
			if (Trigger::NOW == *adjacencyMatrix_[curStateIndex_][index].trigger) {
				*adjacencyMatrix_[curStateIndex_][index].trigger = Trigger::NOTRIGGER;
				curStateIndex_ = index;
				statesMap_[curStateIndex_]->Reset();
				
				break;
			} 
			else if (Trigger::END == *adjacencyMatrix_[curStateIndex_][index].trigger) {
				//������ڲ��ŵĵ�ǰ֡�����һ֡���в��Ž����źţ����л�״̬
				if ((statesMap_[curStateIndex_]->GetFrameCounts() - 1) == statesMap_[curStateIndex_]->GetCurrentFrameIndex() && 
					statesMap_[curStateIndex_]->GetCurrentFramePlayState() ) 
				{
					curStateIndex_ = index;
					statesMap_[curStateIndex_]->Reset();
					//*adjacencyMatrix_[curStateIndex_][index].trigger = Trigger::NOTRIGGER;
					break;
				}
			}
		}

		statesMap_[curStateIndex_]->Play();
	}

	virtual void AddPos(float deltaX, float deltaY) override {
		for (auto& item : statesMap_) {
			item.second->AddPos(deltaX, deltaY);
		}
	}

	virtual void AddPos(const UVector2<float>& deltaPos) override {
		for (auto& item : statesMap_) {
			item.second->AddPos(deltaPos);
		}
	}

	virtual void SetPos(float deltaX, float deltaY) override {
		for (auto& item : statesMap_) {
			item.second->SetPos(deltaX, deltaY);
		}
	}

	virtual void SetPos(const UVector2<float>& deltaPos) override {
		for (auto& item : statesMap_) {
			item.second->SetPos(deltaPos);
		}
	}
private:

};