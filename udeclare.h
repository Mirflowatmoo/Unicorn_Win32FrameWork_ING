#pragma once

class Unicorn;

template <typename ObsrvType>
class UObserver;
template <typename ObsrvType>
class UObserved;

template <class DERIVED_TYPE>
class UWindow;
class UMainWindow;
class UButton;
class UEdit;

class UTimer;

class UImage;
class UAnimation;
class UAudio;

template <class Type>
class UVector2;
template <class Type>
class UVector3;

struct BUTTON_DESC;
struct Association;

enum class FlipType;
enum class NotifyType;
enum class UBUTTON_MSG;
enum class UWINDOW_MSG;
enum class IDC;
enum class Trigger;

#ifdef UNICODE
typedef wchar_t* String;
typedef const wchar_t* CString;
#else
typedef char* String;
typedef const char* CString;
#endif 