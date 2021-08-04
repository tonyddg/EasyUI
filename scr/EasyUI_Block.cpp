#include"EasyUI_Block.h"
#include<iostream>

TCHAR lpDefultStr[] = _T("无选项");

strList::strList():
	top(0)
{
}

imgList::imgList():
	top(0)
{
}

ChooseBlock::ChooseBlock(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, DWORD dwBackgroundClrG, bool blIsTransparentG, bool blKeepRunG):
	Page(iRelPostionXG, iRelPostionYG, iStuffWG, iStuffHG, dwBackgroundClrG, blIsTransparentG, blKeepRunG, true, true),
	iBottomQuantity(0)
{
	StrStuff *lpTmpText = new StrStuff(0, 0, iStuffW, iStuffH, lpDefultStr, dwBackgroundClr, BLACK, min(15, iStuffW), DT_CENTER, true, true, false);
	Insert(lpTmpText, 0);

	iStuffState = STS_OUT;
	iDefultState = STS_NONE;
	iIdentityCode = SIC_PAGE;

	blIsEmpty = true;
}

//iSlideWidth 包含于 iStuffW ， iStuffWidth 必须小于 iStuffW 
ChooseBlock::ChooseBlock(int iRelPostionXG, int iRelPostionYG, int iStuffWG, int iStuffHG, int iBottomWG, int iBottomHG, int iSlideWidthG, int iBottomGapG, DWORD dwSlideClrG, DWORD dwPassedClrG, imgList &ilImgListG, DWORD dwBackgroundClrG, DWORD dwChoosedClrG, DWORD dwChoosedAlpG, DWORD dwClickClrG, DWORD dwClickAlpG, bool blIsTransparentG, bool blKeepRunG) :
	Page(iRelPostionXG, iRelPostionYG, iStuffWG, iStuffHG, dwBackgroundClrG, blIsTransparentG, blKeepRunG, true, true),
	iBottomGap(iBottomGapG),
	iBottomQuantity(ilImgListG.top)
{
	if (iBottomQuantity == 0)
	{
		StrStuff *lpTmpText = new StrStuff(0, 0, iStuffW, iStuffH, lpDefultStr, dwBackgroundClr, BLACK, min(15, iStuffW), DT_CENTER, true, true, false);
		Insert(lpTmpText, 0);

		iStuffState = STS_OUT;
		iDefultState = STS_NONE;
		iIdentityCode = SIC_PAGE;

		blIsEmpty = true;

		return;
	}

	if (iSlideWidthG >= iStuffW)
	{
		iStuffState = STS_ERROR;
		return;
	}

	iLastOffset = 0;

	int iMidPos = (iStuffWG - iSlideWidthG - iBottomWG) / 2;
	iBottomDis = iBottomGap + iBottomHG;
	iListLength = 0;

	lpSideSlide = new SlideStuff(iStuffW - iSlideWidthG, 0, iSlideWidthG, iStuffH, iStuffH, iSlideWidthG, max(0, iBottomDis * iBottomQuantity - iBottomGap - iStuffH), 1, dwSlideClrG, dwPassedClrG, min(iStuffH, iStuffH * iStuffH / (iBottomQuantity * iBottomDis - iBottomGap)), true, true, false, false);

	if (lpSideSlide->iStuffState == STS_ERROR)
	{
		delete lpSideSlide;
		iStuffState = STS_ERROR;
		return;
	}
	Insert(lpSideSlide, 0);

	for (int i = 0; i < iBottomQuantity; i++)
	{
		if ((ilImgListG.list + i)->getwidth() != iBottomWG || (ilImgListG.list + i)->getheight() != iBottomHG)
		{
			(ilImgListG.list + i)->Resize(iBottomWG, iBottomHG);
		}

		BottomStuff* lpTmpBottom = new BottomStuff(iMidPos, iListLength, ilImgListG.list + i, dwChoosedClrG, dwChoosedAlpG, dwClickClrG, dwClickAlpG, false);

		Insert(lpTmpBottom, i + 1);

		iListLength += iBottomDis;
	}

	iListLength += iBottomHG;

	iStuffState = STS_OUT;
	iDefultState = STS_NONE;
	iIdentityCode = SIC_PAGE;

	blIsEmpty = false;
}

bool ChooseBlock::Rebase(int iBottomWG, int iBottomHG, int iSlideWidthG, int iBottomGapG, DWORD dwSlideClrG, DWORD dwPassedClrG, imgList *ilImgListG, DWORD dwChoosedClrG, DWORD dwChoosedAlpG, DWORD dwClickClrG, DWORD dwClickAlpG)
{
	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end();)
	{
		delete itSetIterator->lpStuffPoint;

		itSetIterator++;
	}
	setStuffinPage.clear();

	iBottomQuantity = ilImgListG->top;

	if (iBottomQuantity == 0)
	{
		StrStuff *lpTmpText = new StrStuff(0, 0, iStuffW, iStuffH, lpDefultStr, dwBackgroundClr, WHITE, min(30, iStuffW), DT_CENTER | DT_VCENTER | DT_SINGLELINE, true, true, false);
		Insert(lpTmpText, 0);

		blIsEmpty = true;

		return false;
	}

	iBottomGap = iBottomGapG;

	if (iSlideWidthG >= iStuffW)
	{
		iStuffState = STS_ERROR;
		return false;
	}

	iLastOffset = 0;

	int iMidPos = (iStuffW - iSlideWidthG - iBottomWG) / 2;
	iBottomDis = iBottomGap + iBottomHG;
	iListLength = 0;

	lpSideSlide = new SlideStuff(iStuffW - iSlideWidthG, 0, iSlideWidthG, iStuffH, iStuffH, iSlideWidthG, max(0, iBottomDis * iBottomQuantity - iBottomGap - iStuffH), 1, dwSlideClrG, dwPassedClrG, min(iStuffH, iStuffH * iStuffH / (iBottomQuantity * iBottomDis - iBottomGap)), true, true, false, false);

	if (lpSideSlide->iStuffState == STS_ERROR)
	{
		delete lpSideSlide;
		iStuffState = STS_ERROR;
		return false;
	}
	Insert(lpSideSlide, 0);

	for (int i = 0; i < iBottomQuantity; i++)
	{
		if ((ilImgListG->list + i)->getwidth() != iBottomWG || (ilImgListG->list + i)->getheight() != iBottomHG)
		{
			(ilImgListG->list + i)->Resize(iBottomWG, iBottomHG);
		}

		BottomStuff* lpTmpBottom = new BottomStuff(iMidPos, iListLength, ilImgListG->list + i, dwChoosedClrG, dwChoosedAlpG, dwClickClrG, dwClickAlpG, false);

		Insert(lpTmpBottom, i + 1);

		iListLength += iBottomDis;
	}

	iListLength += iBottomHG;

	blIsEmpty = false;

	return true;
}

ChooseBlock::~ChooseBlock()
{
	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end();)
	{
		delete itSetIterator->lpStuffPoint;

		itSetIterator++;
	}
}

void ChooseBlock::GetCommand(ExMessage * m, bool blValidChoosed)
{
	if (blIsEmpty)return;

	iActiveNum = -1;
	iStuffState = STS_BS_UNCHOOSED;

	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	std::pair<int, int> piCurrentOrigin;
	GetCurrentOrigin(piCurrentOrigin);

	if (m->wheel != 0)
	{
		if (m->wheel > 0)
		{
			lpSideSlide->iPreValue -= lpSideSlide->iHeadDivLDM / 5;
		}
		else
		{
			lpSideSlide->iPreValue += lpSideSlide->iHeadDivLDM / 5;
		}

		lpSideSlide->iPreValue = inRange(lpSideSlide->iPreValue, 0, lpSideSlide->iMaxValue);
	}

	int iOffset = lpSideSlide->iPreValue;
	int iOffsetChange = iOffset - iLastOffset;
	iLastOffset = iOffset;

	for (itSetIterator = ++setStuffinPage.begin(); itSetIterator != setStuffinPage.end();)
	{
		itSetIterator->lpStuffPoint->iRelPostionY -= iOffsetChange;

		itSetIterator++;
	}

	piCurrentOrigin.first += iRelPostionX;
	piCurrentOrigin.second += iRelPostionY;

	SetCurrentOrigin(piCurrentOrigin);

	m->x -= iRelPostionX;
	m->y -= iRelPostionY;

	RECT rtPageRect = { iRelPostionX , iRelPostionY, iRelPostionX + iStuffW, iRelPostionY + iStuffH};

	RECT rtStuffRect;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end();)
	{
		rtStuffRect = { piCurrentOrigin.first + itSetIterator->lpStuffPoint->iRelPostionX ,  piCurrentOrigin.second + itSetIterator->lpStuffPoint->iRelPostionY , piCurrentOrigin.first + itSetIterator->lpStuffPoint->iStuffW + itSetIterator->lpStuffPoint->iRelPostionX ,  piCurrentOrigin.second + itSetIterator->lpStuffPoint->iStuffH + itSetIterator->lpStuffPoint->iRelPostionY };

		if (RectCheck(rtStuffRect, rtPageRect) && blAbsDeal)
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
					if(itSetIterator->lpStuffPoint->iIdentityCode == SIC_BOTTOM)iStuffState = itSetIterator->lpStuffPoint->iStuffState;

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

		if(iStuffState <= STS_BS_UNCHOOSED)iActiveNum++;

		itSetIterator++;
	}

	GetCurrentOrigin(piCurrentOrigin);

	piCurrentOrigin.first -= iRelPostionX;
	piCurrentOrigin.second -= iRelPostionY;

	SetCurrentOrigin(piCurrentOrigin);

	m->x += iRelPostionX;
	m->y += iRelPostionY + iOffset;
}

void ChooseBlock::Flush()
{
	IMAGE* lpOriginalImg = GetWorkingImage();
	std::pair<int, int> piTmpOrigin;
	GetCurrentOrigin(piTmpOrigin);

	SetCurrentOrigin(std::pair<int, int>(0, 0));

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

void ChooseBlock::Draw()
{
	std::pair<int, int> piCurrentOrigin;
	GetCurrentOrigin(piCurrentOrigin);

	if (blAutoFlush)
	{
		Flush();
	}

	if (blIsTransparent)
	{
		PutImgWithout(&imgCacheDisplay, piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY, dwBackgroundClr);
	}
	else putimage(piCurrentOrigin.first + iRelPostionX, piCurrentOrigin.second + iRelPostionY, &imgCacheDisplay);

}

bool ChooseBlock::GetActiveBottom(int &iReceiveNum)
{
	if (iStuffState == STS_BS_UNCHOOSED)return false;
	else
	{
		iReceiveNum = iActiveNum;

		return true;
	}
}

void ChooseBlock::SetState_All(int iSetState)
{
	std::set<StuffPoint, std::greater<StuffPoint>>::iterator itSetIterator;

	for (itSetIterator = setStuffinPage.begin(); itSetIterator != setStuffinPage.end(); itSetIterator++)
	{
		itSetIterator->lpStuffPoint->SetState(iSetState);
	}

	iStuffState = STS_NONE;
}

//////////////////////////////