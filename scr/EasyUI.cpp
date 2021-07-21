#include"EasyUI.h"

StuffPoint::StuffPoint(BaseStuff * lpStuffPointG, int iPriorityLevelG)
{
	lpStuffPoint = lpStuffPointG;
	iPriorityLevel = iPriorityLevelG;
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
	if (lpInsertStuffG->iStuffState == STS_OUT)
	{
		lpInsertStuffG->iAbsPostionX = iAbsPostionX + lpInsertStuffG->iRelPostionX;
		lpInsertStuffG->iAbsPostionY = iAbsPostionY + lpInsertStuffG->iRelPostionY;

		lpInsertStuffG->iStuffState = lpInsertStuffG->iDefultState;
		setStuffinPage.insert(StuffPoint(lpInsertStuffG, iPriorityLevelG));

		return true;
	}
	else return false;
}

void Page::GetCommand(MOUSEMSG * m)
{
	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end();)
	{
		if (DeleteCheck(itSetIterator->lpStuffPoint->iStuffState))
		{
			if (itSetIterator->lpStuffPoint->iStuffState == STS_DELETE)itSetIterator->lpStuffPoint->iStuffState = STS_OUT;

			if(itSetIterator != setStuffinPage.end())setStuffinPage.erase(itSetIterator++);//若把erase和++放到一起，则此时的it会预先缓存，可以安全完成++操作
		}
		else
		{
			if (!itSetIterator->lpStuffPoint->blKeepRun && !RectCheck(itSetIterator->lpStuffPoint->iAbsPostionX, itSetIterator->lpStuffPoint->iAbsPostionY, itSetIterator->lpStuffPoint->iStuffW + iAbsPostionX, itSetIterator->lpStuffPoint->iStuffH + iAbsPostionY, 0, 0, getwidth(), getheight()))
			{
				itSetIterator->lpStuffPoint->iStuffState = STS_BEYOND;
			}
			else if (GetcommandCheck(itSetIterator->lpStuffPoint->iStuffState))
			{
				itSetIterator->lpStuffPoint->GetCommand(m);
			}

			itSetIterator++;
		}
	}
}

void Page::Draw()
{
	if (!blIsTransparent)
	{
		setfillcolor(dwBackgroundClr);
		solidrectangle(iAbsPostionX, iAbsPostionY, iAbsPostionX + iStuffW, iAbsPostionY + iStuffH);
	}

	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end(); itSetIterator++)
	{
		if (DrawCheck(itSetIterator->lpStuffPoint->iStuffState))itSetIterator->lpStuffPoint->Draw();
	}
}

Page::Page(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, DWORD dwBackgroundClrG, bool blIsTransparentG, bool blKeepRunG)
{
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;
	iStuffW = iStuffWG;
	iStuffH = iStuffHG;

	dwBackgroundClr = dwBackgroundClrG;
	blIsTransparent = blIsTransparentG;

	iStuffState = STS_OUT;
	iDefultState = STS_NONE;

	iAbsPostionX = iRelPostionX;
	iAbsPostionY = iRelPostionY;

	blKeepRun = blKeepRunG;
}

void Page::ReRelPostion(int iRelPostionXG, int iRelPostionYG)
{
	iAbsPostionX += iRelPostionXG - iRelPostionX;
	iAbsPostionY += iRelPostionYG - iRelPostionY;

	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;

	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end(); itSetIterator++)
	{
		itSetIterator->lpStuffPoint->ReAbsPostion(iAbsPostionX, iAbsPostionY);
	}
}

void Page::ReAbsPostion(int iCorePostionXG, int iCorePostionYG)
{
	iAbsPostionX = iCorePostionXG + iRelPostionX;
	iAbsPostionY = iCorePostionYG + iRelPostionY;

	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end(); itSetIterator++)
	{
		itSetIterator->lpStuffPoint->ReAbsPostion(iAbsPostionX, iAbsPostionY);
	}
}

///////////////////////////////////////

void BaseStuff::ReAbsPostion(int iCorePostionXG, int iCorePostionYG)
{
	iAbsPostionX = iCorePostionXG + iRelPostionX;
	iAbsPostionY = iCorePostionYG + iRelPostionY;
}

void BaseStuff::ReRelPostion(int iRelPostionXG, int iRelPostionYG)
{
	iAbsPostionX += iRelPostionXG - iRelPostionX;
	iAbsPostionY += iRelPostionYG - iRelPostionY;

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

int BaseStuff::GetAbsPostionX()
{
	return iAbsPostionX;
}

int BaseStuff::GetAbsPostionY()
{
	return iAbsPostionY;
}

void BaseStuff::SetKeepRun(bool blKeepRunG)
{
	blKeepRun = blKeepRunG;
}

/////////////////////////////////////////

BottomStuff::BottomStuff(int iRelPostionXG, int iRelPostionYG, IMAGE * lpBottomIconG, DWORD dwChoosedClrG, DWORD dwChoosedAlpG, DWORD dwClickClrG, DWORD dwClickAlpG, bool blKeepRunG)
{
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;
	iStuffH = lpBottomIconG->getheight();
	iStuffW = lpBottomIconG->getwidth();

	lpBottomIcon = lpBottomIconG;
	dwChoosedClr = dwChoosedClrG;
	dwChoosedAlp = dwChoosedAlpG;
	dwClickAlp = dwClickAlpG;
	dwClickClr = dwClickClrG;

	iStuffState = STS_OUT;
	iDefultState = STS_NONE;

	iAbsPostionX = iRelPostionX;
	iAbsPostionY = iRelPostionY;

	blKeepRun = blKeepRunG;
}

void BottomStuff::GetCommand(MOUSEMSG* m)
{
	if (NumCheck(m->x, iAbsPostionX, iAbsPostionX + iStuffW) && NumCheck(m->y, iAbsPostionY, iAbsPostionY + iStuffH))
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
	putimage(iAbsPostionX, iAbsPostionY, lpBottomIcon);
	if (iStuffState == STS_BS_UNCHOOSED)
	{

	}
	else if (iStuffState == STS_BS_CHOOSED)
	{
		AlphaRectangle(iAbsPostionX, iAbsPostionY, iStuffW + iAbsPostionX, iStuffH + iAbsPostionY, dwChoosedClr, dwChoosedAlp);
	}
	else if (iStuffState == STS_BS_CLICKED)
	{
		AlphaRectangle(iAbsPostionX, iAbsPostionY, iStuffW + iAbsPostionX, iStuffH + iAbsPostionY, dwClickClr, dwClickAlp);
	}

	return;
}

/////////////////////////////////////////////

StrStuff::StrStuff(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, std::string * lpDisplayStrG, DWORD dwBackgroundClrG, DWORD dwTextClrG, int iWordSizeG, UINT uFormatG, bool blIsTransparentG, bool blAutoFlushG, bool blKeepRunG)
{ 
	iRelPostionX = iRelPostionXG;
	iRelPostionY = iRelPostionYG;
	iStuffW = iStuffWG;
	iStuffH = iStuffHG;
	lpDisplayStr = lpDisplayStrG;
	dwBackgroundClr = dwBackgroundClrG;
	dwTextClr = dwTextClrG;
	iWordSize = iWordSizeG;
	uFormat = uFormatG;
	blIsTransparent = blIsTransparentG;
	blAutoFlush = blAutoFlushG;

	iStuffState = STS_OUT;
	iDefultState = STS_NOCOMMAND;

	iAbsPostionX = iRelPostionX;
	iAbsPostionY = iRelPostionY;

	blKeepRun = blKeepRunG;

	if (!blAutoFlush)
	{
		FlushStr();
	}
}

void StrStuff::FlushStr()
{
	IMAGE* lpOriginalImg = GetWorkingImage();
	RECT r = { 0, 0, iStuffW, iStuffH };

	imgCacheImg.Resize(iStuffW, iStuffH);
	SetWorkingImage(&imgCacheImg);

	settextcolor(dwTextClr);
	setbkcolor(dwBackgroundClr);
	settextstyle(iWordSize, 0, "微软黑雅");
	cleardevice();
	drawtext(lpDisplayStr->c_str(), &r, uFormat);

	SetWorkingImage(lpOriginalImg);
}

void StrStuff::Draw()
{
	if (blAutoFlush)
	{
		int iOriginalMode = getbkmode();
		setbkmode(TRANSPARENT);

		RECT r = { iAbsPostionX, iAbsPostionY, iAbsPostionX + iStuffW, iAbsPostionY + iStuffH };

		settextcolor(dwTextClr);
		settextstyle(iWordSize, 0, "微软黑雅");

		if (!blIsTransparent)
		{
			setfillcolor(dwBackgroundClr);
			solidrectangle(iAbsPostionX, iAbsPostionY, iAbsPostionX + iStuffW, iAbsPostionY + iStuffH);
		}

		drawtext(lpDisplayStr->c_str(), &r, uFormat);

		setbkmode(iOriginalMode);
	}
	else
	{
		if (blIsTransparent)
		{
			PutImgWithout(&imgCacheImg, iAbsPostionX, iAbsPostionY, GetImageBuffer(), dwBackgroundClr);
		}
		else putimage(iAbsPostionX, iAbsPostionY, &imgCacheImg);
	}
}