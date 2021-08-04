#include"EasyUI.h"

std::pair<int, int> piCurrentOrigin = { 0, 0 };//当前原点
TCHAR lpDefultFormat[32] = _T("宋体");

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

void GetCurrentOrigin(std::pair<int, int> &piReceiveOrigin)
{
	piReceiveOrigin.first = piCurrentOrigin.first;
	piReceiveOrigin.second = piCurrentOrigin.second;
}

void SetCurrentOrigin(std::pair<int, int> piSetOrigin)
{
	setorigin(piSetOrigin.first, piSetOrigin.second);
	piCurrentOrigin = piSetOrigin;
}

bool DrawCheck(int iState)
{
	return (iState == STS_NONE || iState == STS_NOCOMMAND || iState == STS_DISABLE || iState >= 0);
}

bool GetcommandCheck(int iState)
{
	return (iState == STS_BEYOND || iState == STS_NONE || iState >= 0);
}

bool DeleteCheck(int iState)
{
	return (iState == STS_OUT || iState == STS_ERROR || iState == STS_DELETE || iState == STS_UNPREPARED);
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

void Page::GetCommand(ExMessage * m, bool blValidChoosed)
{
	m->x -= iRelPostionX;
	m->y -= iRelPostionY;

	setorigin(piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY);
	piCurrentOrigin.first += iRelPostionX;
	piCurrentOrigin.second += iRelPostionY;

	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	RECT rtPageRect = { iRelPostionX , iRelPostionY , iRelPostionX + iStuffW , iRelPostionY + iStuffH };

	for (itSetIterator = --setStuffinPage.end(); itSetIterator != --setStuffinPage.begin();)
	{
		if (DeleteCheck(itSetIterator->lpStuffPoint->iStuffState))
		{
			if (itSetIterator->lpStuffPoint->iStuffState == STS_DELETE)itSetIterator->lpStuffPoint->iStuffState = STS_OUT;

			if (itSetIterator != setStuffinPage.end())setStuffinPage.erase(itSetIterator--);//若把erase和++放到一起，则此时的it会预先缓存，可以安全完成++操作
		}
		else
		{
			RECT rtStuffRect = { piCurrentOrigin.first + itSetIterator->lpStuffPoint->iRelPostionX ,  piCurrentOrigin.second + itSetIterator->lpStuffPoint->iRelPostionY , piCurrentOrigin.first + itSetIterator->lpStuffPoint->iStuffW + itSetIterator->lpStuffPoint->iRelPostionX ,  piCurrentOrigin.second + itSetIterator->lpStuffPoint->iStuffH + itSetIterator->lpStuffPoint->iRelPostionY };

			if (RectCheck(rtStuffRect, rtPageRect) && blAbsDeal && !itSetIterator->lpStuffPoint->blKeepRun)
			{
				itSetIterator->lpStuffPoint->iStuffState = STS_BEYOND;
			}
			else if (GetcommandCheck(itSetIterator->lpStuffPoint->iStuffState))
			{
				if (blValidChoosed)
				{
					if (NumCheck(m->x + piCurrentOrigin.first, rtStuffRect.left, rtStuffRect.right) && NumCheck(m->y + piCurrentOrigin.second, rtStuffRect.top, rtStuffRect.bottom))
					{
						itSetIterator->lpStuffPoint->GetCommand(m, true);
						blValidChoosed = false;
					}
					else
					{
						itSetIterator->lpStuffPoint->GetCommand(m, false);
					}
				}
				else
				{
					itSetIterator->lpStuffPoint->GetCommand(m, false);
				}
			}
			itSetIterator--;
		}
	}

	piCurrentOrigin.first -= iRelPostionX;
	piCurrentOrigin.second -= iRelPostionY;
	SetCurrentOrigin(piCurrentOrigin);

	m->x += iRelPostionX;
	m->y += iRelPostionY;
}

void Page::StateFlush()
{
	setorigin(piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY);
	piCurrentOrigin.first += iRelPostionX;
	piCurrentOrigin.second += iRelPostionY;

	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	RECT rtPageRect = { iRelPostionX , iRelPostionY , iRelPostionX + iStuffW , iRelPostionY + iStuffH };
	////////////////////////////////////////
	
	for (itSetIterator = --setStuffinPage.end(); itSetIterator != --setStuffinPage.begin();)
	{
		if (DeleteCheck(itSetIterator->lpStuffPoint->iStuffState))
		{
			if (itSetIterator->lpStuffPoint->iStuffState == STS_DELETE)itSetIterator->lpStuffPoint->iStuffState = STS_OUT;

			if (itSetIterator != setStuffinPage.end())setStuffinPage.erase(itSetIterator--);//若把erase和++放到一起，则此时的it会预先缓存，可以安全完成++操作
		}
		else
		{
			RECT rtStuffRect = { piCurrentOrigin.first + itSetIterator->lpStuffPoint->iRelPostionX ,  piCurrentOrigin.second + itSetIterator->lpStuffPoint->iRelPostionY , piCurrentOrigin.first + itSetIterator->lpStuffPoint->iStuffW + itSetIterator->lpStuffPoint->iRelPostionX ,  piCurrentOrigin.second + itSetIterator->lpStuffPoint->iStuffH + itSetIterator->lpStuffPoint->iRelPostionY };

			if (RectCheck(rtStuffRect, rtPageRect) && blAbsDeal)
			{
				itSetIterator->lpStuffPoint->iStuffState = STS_BEYOND;
			}
			else itSetIterator->lpStuffPoint->iStuffState = itSetIterator->lpStuffPoint->iDefultState;

			itSetIterator--;
		}
	}

	////////////////////////////////////////
	piCurrentOrigin.first -= iRelPostionX;
	piCurrentOrigin.second -= iRelPostionY;
	SetCurrentOrigin(piCurrentOrigin);
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
			PutImgWithout(&imgCacheDisplay, piCurrentOrigin.first, piCurrentOrigin.second, dwBackgroundClr);
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

	piCurrentOrigin.first -= iRelPostionX;
	piCurrentOrigin.second -= iRelPostionY;
	SetCurrentOrigin(piCurrentOrigin);
}

Page::Page(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, DWORD dwBackgroundClrG, bool blIsTransparentG, bool blKeepRunG, bool blAbsDealG, bool blAutoFlushG) :
	dwBackgroundClr(dwBackgroundClrG),
	blAbsDeal(blAbsDealG),
	blAutoFlush(blAutoFlushG),
	blIsTransparent(blIsTransparentG)
{
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;
	iStuffW = iStuffWG;
	iStuffH = iStuffHG;

	imgCacheDisplay.Resize(iStuffW, iStuffH);

	iStuffState = STS_OUT;
	iDefultState = STS_NONE;
	iIdentityCode = SIC_PAGE;

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
	SetCurrentOrigin(piTmpOrigin);
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

void BaseStuff::GetCommand(ExMessage* m, bool blValidChoosed)
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

BaseStuff::~BaseStuff()
{

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
	iIdentityCode = SIC_BOTTOM;

	blKeepRun = blKeepRunG;
}

void BottomStuff::GetCommand(ExMessage* m, bool blValidChoosed)
{
	if (blValidChoosed)
	{
		if (m->lbutton)
		{
			iStuffState = STS_BS_CLICKED;
		}
		else
		{
			if(iStuffState == STS_BS_CLICKED)iStuffState = STS_BS_RELEASE;
			else iStuffState = STS_BS_CHOOSED;
		}
	}
	else 
	{
		iStuffState = STS_BS_UNCHOOSED;//未被选中
	}
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

void BottomStuff::FlushWH()
{
	iStuffH = lpBottomIcon->getheight();
	iStuffW = lpBottomIcon->getwidth();
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
	iIdentityCode = SIC_STR;

	blKeepRun = blKeepRunG;

	imgCacheDisplay.Resize(iStuffW, iStuffH);
}

void StrStuff::Flush()
{
	IMAGE* lpOriginalImg = GetWorkingImage();
	std::pair<int, int> piTmpOrigin = { 0 , 0 };
	std::swap(piTmpOrigin, piCurrentOrigin);

	SetWorkingImage(&imgCacheDisplay);
	setorigin(0, 0);

	RECT r = { 0, 0, iStuffW, iStuffH };

	settextcolor(dwTextClr);
	setbkcolor(dwBackgroundClr);
	settextstyle(iWordSize, 0, lpDefultFormat);
	cleardevice();
	drawtext(lpDisplayStr, &r, uFormat);

	SetWorkingImage(lpOriginalImg);
	std::swap(piTmpOrigin, piCurrentOrigin);
	setorigin(piCurrentOrigin.first, piCurrentOrigin.second);
}

void StrStuff::Draw()
{
	if (blAutoFlush)
	{
		int iOriginalMode = getbkmode();
		setbkmode(TRANSPARENT);

		RECT r = { iRelPostionX, iRelPostionY, iRelPostionX + iStuffW, iRelPostionY + iStuffH };

		settextcolor(dwTextClr);
		settextstyle(iWordSize, 0, lpDefultFormat);

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
			PutImgWithout(&imgCacheDisplay, piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY, dwBackgroundClr);
		}
		else putimage(iRelPostionX, iRelPostionY, &imgCacheDisplay);
	}
}

////////////////////////////////////////

SlideStuff::SlideStuff(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, int iSlideLengthG, int iSlideWidthG, int iMaxValueG, DWORD dwBackgroundClrG, DWORD dwSlideClrG, DWORD dwPassedClrG, int iHeadRadiusG, bool blIsTransparentG, bool blIsReverseG, bool blProcessModeG, bool blKeepRunG) :
	iSlideLength(iSlideLengthG),
	iSlideWidth(iSlideWidthG),
	iMaxValue(iMaxValueG),
	dwBackgroundClr(dwBackgroundClrG),
	dwSlideClr(dwSlideClrG),
	dwPassedClr(dwPassedClrG),
	iHeadRadius(iHeadRadiusG),
	blIsTransparent(blIsTransparentG),
	blIsReverse(blIsReverseG),
	blProcessMode(blProcessModeG)
{
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;

	iStuffW = iStuffWG;
	iStuffH = iStuffHG;

	if (iSlideLength < iHeadRadius)
	{
		iStuffState = STS_ERROR;
		return;
	}

	if (blIsReverse)
	{
		if ((iStuffW < iSlideWidth) || (iStuffH < iSlideLength))
		{
			iStuffState = STS_ERROR;
			return;
		}

		iMidPositionX = (iStuffW - iSlideWidth) / 2;
		iMidPositionY = (iStuffH - iSlideLength) / 2;
	}
	else
	{
		if ((iStuffH < iSlideWidth) || (iStuffW < iSlideLength))
		{
			iStuffState = STS_ERROR;
			return;
		}

		iMidPositionY = (iStuffH - iSlideWidth) / 2;
		iMidPositionX = (iStuffW - iSlideLength) / 2;
	}

	if(blProcessMode)dLengthDivMaxvalue = (double)iSlideLength / (double)iMaxValue;
	else
	{
		dLengthDivMaxvalue = (double)(iSlideLength - iHeadRadius) / (double)iMaxValue;

		if (iSlideLength > iHeadRadius)iHeadDivLDM = (iHeadRadius * iMaxValue) / (iSlideLength - iHeadRadius);
		else iHeadDivLDM = 0;
	}

	blKeepRun = blKeepRunG;

	iPreValue = 0;

	iStuffState = STS_OUT;
	iDefultState = STS_NONE;
	iIdentityCode = SIC_SLIDE;
}

bool SlideStuff::SetValue(int iPreValueG)
{
	if (NumCheck(iPreValue, 0, iMaxValue + 1))
	{
		iPreValue = iPreValueG;
		return true;
	}
	else
	{
		iPreValue = inRange(iPreValue, 0, iMaxValue + 1);
		return false;
	}
}

int SlideStuff::GetValue()
{
	return iPreValue;
}

bool SlideStuff::SetMaxValue(int iMaxValueG)
{
	iMaxValue = iMaxValueG;

	dLengthDivMaxvalue = (double)iSlideLength / (double)iMaxValue;

	if (NumCheck(iPreValue, 0, iMaxValue + 1))
	{
		return true;
	}
	else
	{
		iPreValue = inRange(iPreValue, 0, iMaxValue + 1);
		return false;
	}
}

void SlideStuff::Draw()
{
	iPassedLength = (int)(dLengthDivMaxvalue * iPreValue);

	if (blProcessMode)
	{
		if (!blIsTransparent)
		{
			setfillcolor(dwBackgroundClr);
			solidrectangle(iRelPostionX, iRelPostionY, iRelPostionX + iStuffW, iRelPostionY + iStuffH);
		}

		if (blIsReverse)
		{
			setfillcolor(dwSlideClr);
			solidrectangle(iRelPostionX + iMidPositionX, iRelPostionY + iMidPositionY + iPassedLength, iRelPostionX + iMidPositionX + iSlideWidth, iRelPostionY + iMidPositionY + iSlideLength);

			setfillcolor(dwPassedClr);
			solidrectangle(iRelPostionX + iMidPositionX, iRelPostionY + iMidPositionY, iRelPostionX + iMidPositionX + iSlideWidth, iRelPostionY + iMidPositionY + iPassedLength);

			if (iStuffState != STS_SL_UNCHOOSED && iHeadRadius != 0)
			{
				solidcircle(iRelPostionX + iStuffW / 2, iRelPostionY + iMidPositionY + iPassedLength, iHeadRadius);
			}

		}
		else
		{
			setfillcolor(dwSlideClr);
			solidrectangle(iRelPostionX + iMidPositionX + iPassedLength, iRelPostionY + iMidPositionY, iRelPostionX + iMidPositionX + iSlideLength, iRelPostionY + iMidPositionY + iSlideWidth);

			setfillcolor(dwPassedClr);
			solidrectangle(iRelPostionX + iMidPositionX, iRelPostionY + iMidPositionY, iRelPostionX + iMidPositionX + iPassedLength, iRelPostionY + iMidPositionY + iSlideWidth);

			if (iStuffState != STS_SL_UNCHOOSED && iHeadRadius != 0)
			{
				solidcircle(iRelPostionX + iMidPositionX + iPassedLength, iRelPostionY + iStuffH / 2, iHeadRadius);
			}
		}
	}
	else
	{
		if (!blIsTransparent)
		{
			setfillcolor(dwBackgroundClr);
			solidrectangle(iRelPostionX, iRelPostionY, iRelPostionX + iStuffW, iRelPostionY + iStuffH);
		}

		if (blIsReverse)
		{
			setfillcolor(dwSlideClr);
			solidrectangle(iRelPostionX + iMidPositionX, iRelPostionY + iMidPositionY, iRelPostionX + iMidPositionX + iSlideWidth, iRelPostionY + iMidPositionY + iSlideLength);

			setfillcolor(dwPassedClr);
			solidrectangle(iRelPostionX + iMidPositionX, iRelPostionY + iMidPositionY + iPassedLength, iRelPostionX + iMidPositionX + iSlideWidth, iRelPostionY + iMidPositionY + iPassedLength + iHeadRadius);
		}
		else
		{
			setfillcolor(dwSlideClr);
			solidrectangle(iRelPostionX + iMidPositionX, iRelPostionY + iMidPositionY, iRelPostionX + iMidPositionX + iSlideLength, iRelPostionY + iMidPositionY + iSlideWidth);

			setfillcolor(dwPassedClr);
			solidrectangle(iRelPostionX + iMidPositionX + iPassedLength, iRelPostionY + iMidPositionY, iRelPostionX + iMidPositionX + iHeadRadius + iPassedLength, iRelPostionY + iMidPositionY + iSlideWidth);
		}
	}
}

void SlideStuff::GetCommand(ExMessage* m, bool blValidChoosed)
{
	if (blProcessMode)
	{
		if (blIsReverse)
		{
			if (m->lbutton && iStuffState == STS_SL_HOLD)
			{
				if (m->y <= iRelPostionY + iMidPositionY)iPreValue = 0;
				else if (m->y >= iRelPostionY + iMidPositionY + iSlideLength)iPreValue = iMaxValue;
				else
				{
					iPreValue = (int)((m->y - iMidPositionY - iRelPostionY) / dLengthDivMaxvalue);
				}

				return;
			}

			if (blValidChoosed)
			{
				if (m->lbutton)
				{
					if (iStuffState == STS_SL_CHOOSED)
					{
						iStuffState = STS_SL_HOLD;

						if (m->y <= iRelPostionY + iMidPositionY)iPreValue = 0;
						else if (m->y >= iRelPostionY + iMidPositionY + iSlideLength)iPreValue = iMaxValue;
						else
						{
							iPreValue = (int)((m->y - iMidPositionY - iRelPostionY) / dLengthDivMaxvalue);
						}
					}
					else iStuffState = STS_SL_UNCHOOSED;
				}
				else
				{
					iStuffState = STS_SL_CHOOSED;
				}
			}
			else iStuffState = STS_SL_UNCHOOSED;//未被选中
		}
		else
		{
			if (m->lbutton && iStuffState == STS_SL_HOLD)
			{
				if (m->x <= iRelPostionX + iMidPositionX)iPreValue = 0;
				else if (m->x >= iRelPostionX + iMidPositionX + iSlideLength)iPreValue = iMaxValue;
				else
				{
					iPreValue = (int)((m->x - iMidPositionX - iRelPostionX) / dLengthDivMaxvalue);
				}

				return;
			}

			if (blValidChoosed)
			{
				if (m->lbutton)
				{
					if (iStuffState == STS_SL_CHOOSED)
					{
						iStuffState = STS_SL_HOLD;

						if (m->x <= iRelPostionX + iMidPositionX)iPreValue = 0;
						else if (m->x >= iRelPostionX + iMidPositionX + iSlideLength)iPreValue = iMaxValue;
						else
						{
							iPreValue = (int)((m->x - iMidPositionX - iRelPostionX) / dLengthDivMaxvalue);
						}
					}
					else iStuffState = STS_SL_UNCHOOSED;
				}
				else
				{
					iStuffState = STS_SL_CHOOSED;
				}
			}
			else iStuffState = STS_SL_UNCHOOSED;//未被选中
		}
	}
	else
	{
		if (blIsReverse)
		{
			if (m->lbutton && iStuffState == STS_SL_HOLD)
			{
				iPreValue += (int)((m->y - iLastMouse - iRelPostionY - iMidPositionY) / dLengthDivMaxvalue);
				iPreValue = inRange(iPreValue, 0, iMaxValue);

				iLastMouse = m->y - iRelPostionY - iMidPositionY;

				return;
			}

			if (blValidChoosed)
			{
				if (m->lbutton)
				{
					if (iStuffState == STS_SL_CHOOSED)
					{
						if (NumCheck(m->y - iRelPostionY - iMidPositionY, iPassedLength, iPassedLength + iHeadRadius))
						{
							iStuffState = STS_SL_HOLD;

							iLastMouse = m->y - iRelPostionY - iMidPositionY;
						}
						else
						{
							if (m->y - iRelPostionY - iMidPositionY < iPassedLength)
							{
								iPreValue -= iHeadDivLDM;
							}
							else
							{
								iPreValue += iHeadDivLDM;
							}

							iPreValue = inRange(iPreValue, 0, iMaxValue);
						}
					}
					else iStuffState = STS_SL_UNCHOOSED;
				}
				else
				{
					iStuffState = STS_SL_CHOOSED;
				}
			}
			else iStuffState = STS_SL_UNCHOOSED;//未被选中
		}
		else
		{
			if (m->lbutton && iStuffState == STS_SL_HOLD)
			{
				iPreValue += (int)((m->x - iLastMouse - iRelPostionX - iMidPositionX) / dLengthDivMaxvalue);
				iPreValue = inRange(iPreValue, 0, iMaxValue);

				iLastMouse = m->x - iRelPostionX - iMidPositionX;

				return;
			}

			if (blValidChoosed)
			{
				if (m->lbutton)
				{
					if (iStuffState == STS_SL_CHOOSED)
					{
						if (NumCheck(m->x - iRelPostionX - iMidPositionX, iPassedLength, iPassedLength + iHeadRadius))
						{
							iStuffState = STS_SL_HOLD;

							iLastMouse = m->x - iRelPostionX - iMidPositionX;
						}
						else
						{
							if (m->x - iRelPostionX - iMidPositionX < iPassedLength)
							{
								iPreValue -= iHeadDivLDM;
							}
							else
							{
								iPreValue += iHeadDivLDM;
							}

							iPreValue = inRange(iPreValue, 0, iMaxValue);
						}
					}
					else iStuffState = STS_SL_UNCHOOSED;
				}
				else
				{
					iStuffState = STS_SL_CHOOSED;
				}
			}
			else iStuffState = STS_SL_UNCHOOSED;//未被选中
		}
	}
}