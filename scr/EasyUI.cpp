#include"EasyUI.h"

std::pair<int, int> piCurrentOrigin = { 0, 0 };//当前原点

StuffPoint::StuffPoint(BaseStuff * lpStuffPointG, int iPriorityLevelG):
	lpStuffPoint(lpStuffPointG),
	iPriorityLevel(iPriorityLevelG)
{
}

bool operator>(StuffPoint a, StuffPoint b)
{
	return a.iPriorityLevel > b.iPriorityLevel;
}

bool operator<(StuffPoint a, StuffPoint b)
{
	return a.iPriorityLevel < b.iPriorityLevel;
}

///////////////////////////////////////

bool DrawCheck(int iState)
{
	return (iState == STS_NONE || iState == STS_NOCOMMAND || iState == STS_DISABLE || iState >= 0);
}

bool GetcommandCheck(int iState)
{
	return (STS_BEYOND || iState == STS_NONE || iState >= 0);
}

bool DeleteCheck(int iState)
{
	return (iState == STS_OUT || iState == STS_ERROR || iState == STS_DELETE);
}

bool Page::Insert(BaseStuff * lpInsertStuffG, int iPriorityLevelG)
{
	if (lpInsertStuffG == this)return false;

	if (lpInsertStuffG->iStuffState == STS_OUT)
	{
		lpInsertStuffG->iStuffState = lpInsertStuffG->iDefultState;
		setStuffinPage.insert(StuffPoint(lpInsertStuffG, iPriorityLevelG));

		return true;
	}
	else return false;
}

void Page::GetCommand(MOUSEMSG * m)
{
	setorigin(piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY);
	piCurrentOrigin.first += iRelPostionX;
	piCurrentOrigin.second += iRelPostionY;

	m->x -= iRelPostionX;
	m->y -= iRelPostionY;

	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	RECT rtPageRect = { iRelPostionX , iRelPostionY , iRelPostionX + iStuffW , iRelPostionY + iStuffH };
	RECT rtScreenRect = { 0 , 0 , getwidth() , getheight() };

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end();)
	{
		if (DeleteCheck(itSetIterator->lpStuffPoint->iStuffState))
		{
			if (itSetIterator->lpStuffPoint->iStuffState == STS_DELETE)itSetIterator->lpStuffPoint->iStuffState = STS_OUT;

			if(itSetIterator != setStuffinPage.end())setStuffinPage.erase(itSetIterator++);//若把erase和++放到一起，则此时的it会预先缓存，可以安全完成++操作
		}
		else
		{
			if (itSetIterator->lpStuffPoint->blKeepRun)
			{
				if (GetcommandCheck(itSetIterator->lpStuffPoint->iStuffState))
				{
					itSetIterator->lpStuffPoint->GetCommand(m);
				}
			}
			else
			{
				RECT rtStuffRect = { piCurrentOrigin.first + itSetIterator->lpStuffPoint->iRelPostionX ,  piCurrentOrigin.second + itSetIterator->lpStuffPoint->iRelPostionY , piCurrentOrigin.first + itSetIterator->lpStuffPoint->iStuffW + itSetIterator->lpStuffPoint->iRelPostionX ,  piCurrentOrigin.second + itSetIterator->lpStuffPoint->iStuffH + itSetIterator->lpStuffPoint->iRelPostionY };

				if (RectCheck(rtStuffRect, rtScreenRect))
				{
					itSetIterator->lpStuffPoint->iStuffState = STS_BEYOND;
				}
				else if (RectCheck(rtStuffRect, rtPageRect) && blAbsDeal)
				{
					itSetIterator->lpStuffPoint->iStuffState = STS_BEYOND;
				}
				else if (GetcommandCheck(itSetIterator->lpStuffPoint->iStuffState))
				{
					itSetIterator->lpStuffPoint->GetCommand(m);
				}
			}

			itSetIterator++;
		}
	}

	setorigin(piCurrentOrigin.first - iRelPostionX, piCurrentOrigin.second - iRelPostionY);

	piCurrentOrigin.first -= iRelPostionX;
	piCurrentOrigin.second -= iRelPostionY;

	m->x += iRelPostionX;
	m->y += iRelPostionY;
}

void Page::Draw()
{
	setorigin(piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY);
	piCurrentOrigin.first += iRelPostionX;
	piCurrentOrigin.second += iRelPostionY;

	if (blAbsDeal)
	{
		if (blAutoFlush)
		{
			Flush();
		}

		if (blIsTransparent)
		{
			PutImgWithout(&imgCacheDisplay, piCurrentOrigin.first, piCurrentOrigin.first, dwBackgroundClr);
		}
		else putimage(0, 0, &imgCacheDisplay);
	}
	else
	{
		if (!blIsTransparent)
		{
			setfillcolor(dwBackgroundClr);
			solidrectangle(0, 0, iStuffW, iStuffH);
		}

		std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

		for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end(); itSetIterator++)
		{
			if (DrawCheck(itSetIterator->lpStuffPoint->iStuffState))itSetIterator->lpStuffPoint->Draw();
		}
	}

	setorigin(piCurrentOrigin.first - iRelPostionX, piCurrentOrigin.second - iRelPostionY);
	piCurrentOrigin.first -= iRelPostionX;
	piCurrentOrigin.second -= iRelPostionY;
}

Page::Page(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, DWORD dwBackgroundClrG, bool blIsTransparentG, bool blKeepRunG, bool blAbsDealG, bool blAutoFlushG) :
	dwBackgroundClr(dwBackgroundClrG),
	blAbsDeal(blAbsDealG),
	blAutoFlush(blAutoFlushG)
{
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;
	iStuffW = iStuffWG;
	iStuffH = iStuffHG;

	imgCacheDisplay.Resize(iStuffW, iStuffH);

	iStuffState = STS_OUT;
	iDefultState = STS_NONE;

	blKeepRun = blKeepRunG;
}

void Page::Flush()
{
	IMAGE* lpOriginalImg = GetWorkingImage();
	std::pair<int,int> piTmpOrigin = { 0 , 0 };
	std::swap(piTmpOrigin, piCurrentOrigin);

	SetWorkingImage(&imgCacheDisplay);
	setorigin(0, 0);

	setbkcolor(dwBackgroundClr);
	cleardevice();

	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end(); itSetIterator++)
	{
		if (DrawCheck(itSetIterator->lpStuffPoint->iStuffState))itSetIterator->lpStuffPoint->Draw();
	}

	SetWorkingImage(lpOriginalImg);
	std::swap(piTmpOrigin, piCurrentOrigin);
	setorigin(piCurrentOrigin.first, piCurrentOrigin.second);
}

///////////////////////////////////////

void BaseStuff::ReRelPostion(int iRelPostionXG, int iRelPostionYG)
{
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;
}

int BaseStuff::GetState()
{
	return iStuffState;
}

void BaseStuff::GetCommand(MOUSEMSG* m)
{
	iStuffState = iDefultState;
}

void BaseStuff::Draw()
{
}

bool BaseStuff::SetState(int iSetStateG)
{
	if (iStuffState == STS_NONE || iStuffState == STS_NOCOMMAND || iStuffState == STS_DISABLE || iStuffState >= 0 || iStuffState == STS_HIDE)
	{
		if (iSetStateG == STS_SET_DELETE)
		{
			iStuffState = STS_DELETE;
		}
		else if (iSetStateG == STS_SET_DISABLE)
		{
			iStuffState = STS_DISABLE;
		}
		else if (iSetStateG == STS_SET_HIDE)
		{
			iStuffState = STS_HIDE;
		}
		else if (iSetStateG == STS_SET_REABLE)
		{
			iStuffState = iDefultState;
		}

		return true;
	}
	else return false;
}

int BaseStuff::GetRelPostionX()
{
	return iRelPostionX;
}

int BaseStuff::GetRelPostionY()
{
	return iRelPostionY;
}

void BaseStuff::SetKeepRun(bool blKeepRunG)
{
	blKeepRun = blKeepRunG;
}

/////////////////////////////////////////

BottomStuff::BottomStuff(int iRelPostionXG, int iRelPostionYG, IMAGE * lpBottomIconG, DWORD dwChoosedClrG, DWORD dwChoosedAlpG, DWORD dwClickClrG, DWORD dwClickAlpG, bool blKeepRunG):
	lpBottomIcon(lpBottomIconG),
	dwChoosedClr(dwChoosedClrG),
	dwChoosedAlp(dwChoosedAlpG),
	dwClickAlp(dwClickAlpG),
	dwClickClr(dwClickClrG)
{
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;
	iStuffH = lpBottomIconG->getheight();
	iStuffW = lpBottomIconG->getwidth();

	iStuffState = STS_OUT;
	iDefultState = STS_NONE;

	blKeepRun = blKeepRunG;
}

void BottomStuff::GetCommand(MOUSEMSG* m)
{
	if (NumCheck(m->x, iRelPostionX, iRelPostionX + iStuffW) && NumCheck(m->y, iRelPostionY, iRelPostionY + iStuffH))
	{
		if (m->mkLButton)
		{
			iStuffState = STS_BS_CLICKED;
		}
		else
		{
			iStuffState = STS_BS_CHOOSED;
		}
	}
	else iStuffState = STS_BS_UNCHOOSED;//未被选中
}

void BottomStuff::Draw()
{
	putimage(iRelPostionX, iRelPostionY, lpBottomIcon);
	if (iStuffState == STS_BS_UNCHOOSED)
	{

	}
	else if (iStuffState == STS_BS_CHOOSED)
	{
		AlphaRectangle(piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY, piCurrentOrigin.first + iStuffW + iRelPostionX, piCurrentOrigin.second + iStuffH + iRelPostionY, dwChoosedClr, dwChoosedAlp);
	}
	else if (iStuffState == STS_BS_CLICKED)
	{
		AlphaRectangle(piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY, piCurrentOrigin.first + iStuffW + iRelPostionX, piCurrentOrigin.second + iStuffH + iRelPostionY, dwClickClr, dwClickAlp);
	}

	return;
}

/////////////////////////////////////////////

StrStuff::StrStuff(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, TCHAR * lpDisplayStrG, DWORD dwBackgroundClrG, DWORD dwTextClrG, int iWordSizeG, UINT uFormatG, bool blIsTransparentG, bool blAutoFlushG, bool blKeepRunG):
	lpDisplayStr(lpDisplayStrG),
	dwBackgroundClr(dwBackgroundClrG),
	dwTextClr(dwTextClrG),
	iWordSize(iWordSizeG),
	uFormat(uFormatG),
	blIsTransparent(blIsTransparentG),
	blAutoFlush(blAutoFlushG)
{ 
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;
	iStuffW = iStuffWG;
	iStuffH = iStuffHG;

	iStuffState = STS_OUT;
	iDefultState = STS_NOCOMMAND;

	blKeepRun = blKeepRunG;
}

void StrStuff::Flush()
{
	IMAGE* lpOriginalImg = GetWorkingImage();
	RECT r = { 0, 0, iStuffW, iStuffH };

	imgCacheDisplay.Resize(iStuffW, iStuffH);
	SetWorkingImage(&imgCacheDisplay);

	settextcolor(dwTextClr);
	setbkcolor(dwBackgroundClr);
	settextstyle(iWordSize, 0, _T("微软黑雅"));
	cleardevice();
	drawtext(lpDisplayStr, &r, uFormat);

	SetWorkingImage(lpOriginalImg);
}

void StrStuff::Draw()
{
	if (blAutoFlush)
	{
		int iOriginalMode = getbkmode();
		setbkmode(TRANSPARENT);

		RECT r = { iRelPostionX, iRelPostionY, iRelPostionX + iStuffW, iRelPostionY + iStuffH };

		settextcolor(dwTextClr);
		settextstyle(iWordSize, 0, _T("微软黑雅"));

		if (!blIsTransparent)
		{
			setfillcolor(dwBackgroundClr);
			solidrectangle(iRelPostionX, iRelPostionY, iRelPostionX + iStuffW, iRelPostionY + iStuffH);
		}

		drawtext(lpDisplayStr, &r, uFormat);

		setbkmode(iOriginalMode);
	}
	else
	{
		if (blIsTransparent)
		{
			PutImgWithout(&imgCacheDisplay, piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.first + iRelPostionY, dwBackgroundClr);
		}
		else putimage(iRelPostionX, iRelPostionY, &imgCacheDisplay);
	}
}