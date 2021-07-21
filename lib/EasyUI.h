#pragma once
#include"EasyGraphy.h"
#include<set>
#include<string>

#define STS_DISABLE		-1//未启用
#define STS_NOCOMMAND	-2//无需接受命令
#define STS_NONE		-3//无命令
#define STS_ERROR		-4//错误
#define STS_OUT			-5//未加入Page中
#define STS_HIDE		-6//隐藏
#define STS_DELETE		-7//等待删除
#define STS_BEYOND		-8//于边界外（整个绘图窗口）

#define STS_SET_DISABLE	0
#define STS_SET_HIDE	1
#define STS_SET_DELETE	2
#define STS_SET_REABLE	3

bool DrawCheck(int iState);
bool GetcommandCheck(int iState);
bool DeleteCheck(int iState);

class BaseStuff
{
	friend class Page;

public:

	int GetRelPostionX();
	int GetRelPostionY();

	int GetAbsPostionX();
	int GetAbsPostionY();

	int GetState();

	virtual void ReRelPostion(int iRelPostionXG, int iRelPostionYG);

	//仅限STS_SET_
	bool SetState(int iSetStateG);

	//设置为强制运行状态，不会因在屏幕外而停止
	void SetKeepRun(bool blKeepRunG);

protected:

	int iRelPostionX;
	int iRelPostionY;
	int iStuffW;
	int iStuffH;

	int iAbsPostionX;
	int iAbsPostionY;

	int iStuffState;

	int iDefultState;

	bool blKeepRun;

private:

	virtual void ReAbsPostion(int iCorePostionXG, int iCorePostionYG);

	virtual void GetCommand(MOUSEMSG* m);
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

class Page:public BaseStuff
{
public:

	Page(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, DWORD dwBackgroundClrG, bool blIsTransparentG, bool blKeepRunG);

	bool Insert(BaseStuff * lpInsertStuffG, int iPriorityLevelG);
	void GetCommand(MOUSEMSG* m);
	void Draw();

	void ReRelPostion(int iRelPostionXG, int iRelPostionYG);

private:

	void ReAbsPostion(int iCorePostionXG, int iCorePostionYG);

	std::set<StuffPoint> setStuffinPage;

	bool blIsTransparent;
	DWORD dwBackgroundClr;

};

#define STS_BS_UNCHOOSED 1
#define STS_BS_CHOOSED 2
#define STS_BS_CLICKED 3

class BottomStuff: public BaseStuff
{
public:

	BottomStuff(int iRelPostionXG, int iRelPostionYG, IMAGE * lpBottomIconG, DWORD dwChoosedClrG, DWORD dwChoosedAlpG, DWORD dwClickClrG, DWORD dwClickAlpG, bool blKeepRunG);

private:

	IMAGE* lpBottomIcon;

	DWORD dwChoosedClr;
	DWORD dwChoosedAlp;

	DWORD dwClickClr;
	DWORD dwClickAlp;

	void GetCommand(MOUSEMSG* m);
	void Draw();
};

class StrStuff : public BaseStuff
{
public:

	StrStuff(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, std::string * lpDisplayStrG, DWORD dwBackgroundClrG, DWORD dwTextClrG, int iWordSizeG, UINT uFormatG, bool blIsTransparentG, bool blAutoFlushG, bool blKeepRunG);

	void FlushStr();

private:

	std::string* lpDisplayStr;
	bool blIsTransparent;
	DWORD dwBackgroundClr;
	DWORD dwTextClr;
	int iWordSize;
	UINT uFormat;

	bool blAutoFlush;

	IMAGE imgCacheImg;

	//void GetCommand(MOUSEMSG* m);
	void Draw();
};