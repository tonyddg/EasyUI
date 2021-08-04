#pragma once
#include"EasyGraphic.h"
#include<set>

#define STS_DISABLE			-1//未启用(不更新消息，但仍然绘图)
#define STS_NOCOMMAND		-2//无需接受命令
#define STS_NONE			-3//无命令
#define STS_ERROR			-4//错误
#define STS_OUT				-5//未加入Page中
#define STS_HIDE			-6//隐藏
#define STS_DELETE			-7//等待删除
#define STS_BEYOND			-8//于边界外（整个绘图窗口）
#define STS_UNPREPARED		-9//未准备好的

#define STS_SET_DISABLE	0
#define STS_SET_HIDE	1
#define STS_SET_DELETE	2
#define STS_SET_REABLE	3

bool DrawCheck(int iState);
bool GetcommandCheck(int iState);
bool DeleteCheck(int iState);
void GetCurrentOrigin(std::pair<int, int> &piReceiveOrigin);
void SetCurrentOrigin(std::pair<int, int> piSetOrigin);

#define SIC_BASE	0
#define SIC_PAGE	1
#define SIC_BOTTOM	2
#define SIC_STR		3
#define SIC_SLIDE	4

class BaseStuff
{
	friend class Page;
	friend class ChooseBlock;

public:

	int GetRelPostionX();
	int GetRelPostionY();

	int GetState();

	virtual void ReRelPostion(int iRelPostionXG, int iRelPostionYG);

	//仅限STS_SET_ 使用时确保目标已在Page中
	bool SetState(int iSetStateG);

	//设置为强制运行状态，不会因在屏幕外而停止
	void SetKeepRun(bool blKeepRunG);

	virtual ~BaseStuff();

protected:

	int iRelPostionX;
	int iRelPostionY;
	int iStuffW;
	int iStuffH;

	int iStuffState;

	int iDefultState;

	bool blKeepRun;

	int iIdentityCode;

	virtual void GetCommand(ExMessage* m, bool blValidChoosed);
	virtual void Draw();

};

struct StuffPoint
{
	BaseStuff* lpStuffPoint;
	int iPriorityLevel;

	StuffPoint(BaseStuff * lpStuffPointG, int iPriorityLevelG);
};

bool operator> (StuffPoint a, StuffPoint b);
bool operator< (StuffPoint a, StuffPoint b);

class Page: public BaseStuff
{
public:

	Page(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, DWORD dwBackgroundClrG, bool blIsTransparentG, bool blKeepRunG, bool blAbsDealG, bool blAutoFlushG);

	bool Insert(BaseStuff * lpInsertStuffG, int iPriorityLevelG);
	void GetCommand(ExMessage* m, bool blValidChoosed);
	void Draw();

	virtual void Flush();
	virtual void StateFlush();//更新Page中单层Stuff（！）的状态，包含在GetCommand中，如出现删除失败与物件缺失（BEYOND与DELETE无法实时执行，如移动Page时建议使用）时尝试使用

protected:

	std::set<StuffPoint> setStuffinPage;

	bool blAutoFlush;//是否自动刷新 仅绝对模式下有效
	bool blAbsDeal;//绝对模式，开启后仅iStuffW与iStuffH内的物件可以被显示，若开启blKeepRun则仍然运行，但与边界外的部分依然无法显示
	bool blIsTransparent;
	DWORD dwBackgroundClr;

	IMAGE imgCacheDisplay;
};

#define STS_BS_UNCHOOSED	1
#define STS_BS_CHOOSED		2
#define STS_BS_CLICKED		3
#define STS_BS_RELEASE		4

class BottomStuff: public BaseStuff
{
public:

	BottomStuff(int iRelPostionXG, int iRelPostionYG, IMAGE * lpBottomIconG, DWORD dwChoosedClrG, DWORD dwChoosedAlpG, DWORD dwClickClrG, DWORD dwClickAlpG, bool blKeepRunG);

	void FlushWH();

private:

	IMAGE* lpBottomIcon;

	DWORD dwChoosedClr;
	DWORD dwChoosedAlp;

	DWORD dwClickClr;
	DWORD dwClickAlp;

	void GetCommand(ExMessage* m, bool blValidChoosed);
	void Draw();
};

class StrStuff : public BaseStuff
{
public:

	StrStuff(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, TCHAR * lpDisplayStrG, DWORD dwBackgroundClrG, DWORD dwTextClrG, int iWordSizeG, UINT uFormatG, bool blIsTransparentG, bool blAutoFlushG, bool blKeepRunG);

	void Flush();

private:

	TCHAR* lpDisplayStr;
	bool blIsTransparent;
	DWORD dwBackgroundClr;
	DWORD dwTextClr;
	int iWordSize;
	UINT uFormat;

	bool blAutoFlush;

	IMAGE imgCacheDisplay;

	//void GetCommand(ExMessage* m);
	void Draw();
};

#define STS_SL_UNCHOOSED	1
#define STS_SL_CHOOSED		2
#define STS_SL_HOLD			3

class SlideStuff : public BaseStuff
{
	friend class ChooseBlock;

public:

	SlideStuff(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, int iSlideLengthG, int iSlideWidthG, int iMaxValueG, DWORD dwBackgroundClrG, DWORD dwSlideClrG, DWORD dwPassedClrG, int iHeadRadius, bool blIsTransparentG, bool blIsReverseG, bool blProcessModeG, bool blKeepRunG);

	int GetValue();
	bool SetValue(int iPreValueG);

	bool SetMaxValue(int iMaxValueG);

private:

	int iSlideLength;// 必须要求 iStuffH >= iSlideLength 与 iStuffWidth >= iSlideLength >= iHeadRadius 否则 状态为 ERROE
	int iSlideWidth;
	int iMaxValue;
	int iPreValue;

	int iMidPositionX;
	int iMidPositionY;
	double dLengthDivMaxvalue;
	int iHeadDivLDM;

	DWORD dwBackgroundClr;
	DWORD dwSlideClr;
	DWORD dwPassedClr;

	int iHeadRadius;

	bool blIsTransparent;
	bool blIsReverse;//true:竖直模式 false:水平模式
	bool blProcessMode;//true:进度条模式 false:侧边条模式 此模式下 iHeadRadius 为移动条的长度 此长度包含于iSlideLength dwPassedClr 为移动条的颜色 

	int iLastMouse;

	int iPassedLength;

	void GetCommand(ExMessage* m, bool blValidChoosed);
	void Draw();
};