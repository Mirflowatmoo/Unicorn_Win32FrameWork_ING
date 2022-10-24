#pragma once
#include <Windows.h>
#include "udeclare.h"

class Unicorn final {

public:

	Unicorn(const Unicorn&) = delete;
	Unicorn& operator=(const Unicorn&) = delete;
	~Unicorn();

	static Unicorn* GetInstance();
	static bool Initialize();
	static void MessageProc();
	static UWINDOW_MSG msg_;

private:

	Unicorn() = default;

	class DeUnicorn {
	public:
		~DeUnicorn() {
			if (Unicorn::pInstance_ != nullptr) {
				delete Unicorn::pInstance_;
				Unicorn::pInstance_ = nullptr;
			}
		}
	};

	static ULONG_PTR pGdiplusToken_;
	static Unicorn* pInstance_;
	static DeUnicorn deUnicorn_;
	static bool InitStatus;
};

