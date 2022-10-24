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
	bool isEnd; //帧播放结束信号
	float timerAcc_; //每帧时间累加器
	float frameIntervalTime_; //帧间隔时间
	unsigned int frameIndex_; //当前帧索引
	unsigned int frameCounts_; //帧总数
	unsigned int widthSrc_, heightSrc_; //源图尺寸
	unsigned int widthDst_, heightDst_; //目标图尺寸

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
			//生成的索引值为空闲邻接矩阵项索引
			index = idleIndexQueue_.pop();
		}
		else {
			//生成的索引值为状态机状态数
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

			//解除待移除状态的所有关联
			for (int _index = 0; _index < capacity_; ++_index) {
				adjacencyMatrix_[_index][index] = { false, nullptr };
			}
			std::fill(adjacencyMatrix_[index].begin(), adjacencyMatrix_[index].end(), { false, nullptr });

			//将待移除状态分配到的邻接矩阵索引添加进空闲索引队列
			idleIndexQueue_.push(index);

			indexMap_.erase(); //移除状态名与索引的映射
			statesMap_.erase(); //移除索引与状态的映射
		}
		catch (std::out_of_range e) {
			return false;
		}
		
		return true;
	}

	//状态关联
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

	//解除状态关联
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
	int curStateIndex_; //当前状态索引
	std::string curStateName_; //当前状态名
	unsigned int capacity_; //状态机容量
	std::queue<int> idleIndexQueue_; //移除状态产生的空闲邻接矩阵项索引队列
	std::map<std::string, int> indexMap_; //状态名——索引 映射表
	std::vector<std::vector<Association> > adjacencyMatrix_; //邻接矩阵
	std::map<int, std::shared_ptr<DERIVED_TYPE> > statesMap_; //索引——状态 映射表

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
				//如果正在播放的当前帧是最后一帧且有播放结束信号，则切换状态
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