#include"EasyGraphic.h"

bool RectCheck(int al, int at, int ar, int ab, int bl, int bt, int br, int bb)
{
	return ar < bl || ab < bt || bb < at || br < al;
}

bool RectCheck(RECT &a, RECT &b)
{
	return a.right < b.left || a.bottom < b.top || b.bottom < a.top || b.right < a.left;
}

//clr ÎªBGR 0 < alpha < 255
void AlphaRectangle(int l, int t, int r, int b, DWORD clr, DWORD alpha)
{
	DWORD* pbImg = GetImageBuffer(GetWorkingImage());

	DWORD fclr = BGRtoARGB(clr);

	alpha = inRange(alpha, 0, 255);

	int hw = getwidth();

	int rr = min(r, hw);
	int rb = min(b, getheight());

	for (int i = max(l, 0); i < rr; i++)
	{
		for (int j = max(t, 0); j < rb; j++)
		{
			pbImg[DW(j, i, hw)] = AlphaCocula(pbImg[DW(j, i, hw)], fclr, alpha);
		}
	}
}

void PutImgWithout(IMAGE* obj, int px, int py, COLORREF withouter)
{
	int wX = getwidth();
	int wY = getheight();
	DWORD bitsub = 0x00FFFFFF;
	DWORD* pbWnd = GetImageBuffer(GetWorkingImage());
	DWORD* pbImg = GetImageBuffer(obj);
	int iX = obj->getwidth();
	int iY = obj->getheight();
	int ofy = 0, ofx = 0;

	int sX = min(iX, wX - px);
	int sY = min(iY, wY - py);

	if (px < 0)ofx = -px;
	if (py < 0)ofy = -py;

	for (int i1 = ofx; i1 < sX; i1++)
	{
		for (int i2 = ofy; i2 < sY; i2++)
		{
			if ((pbImg[DW(i2, i1, iX)] & bitsub) == BGR(withouter))continue;	// ¼ì²âÊÇ·ñÒªÅÅ³ý¸ÃÑÕÉ«

			pbWnd[DW(i2 + py, i1 + px, wX)] = pbImg[DW(i2, i1, iX)]; // ²Ù×÷ÏÔ´æ
		}
	}
}

void AlphaChange::Init(DWORD dwStartClrG, bool blGoCoveredG, int iDuringTimeG)
{
	getimage(&imgCacheDisplay, 0, 0, getwidth(), getheight());

	dwStartClr = dwStartClrG;
	blGoCovered = blGoCoveredG;
	iDuringTime = iDuringTimeG;

	fCoverRate = 255.0 / (float)iDuringTime;
	iDrawTime = 0;
}

int AlphaChange::GetDrawTime()
{
	return iDrawTime;
}

void AlphaChange::DrawFrame()
{
	iDrawTime++;
	putimage(0, 0, &imgCacheDisplay);

	if (blGoCovered)
	{
		AlphaRectangle(0, 0, getwidth(), getheight(), dwStartClr, 255 - (int)(fCoverRate * iDrawTime));
	}
	else
	{
		AlphaRectangle(0, 0, getwidth(), getheight(), dwStartClr, (int)(fCoverRate * iDrawTime));
	}
}

bool AlphaChange::DrawOver()
{
	return iDrawTime == iDuringTime;
}

void DrawTextImg(const TCHAR* sl, IMAGE* il, DWORD bkclr, DWORD txclr, int sw, int sh, int word_size, UINT uFormat)
{
	RECT r = { 0, 0, sw, sh };
	IMAGE* gwi = GetWorkingImage();

	il->Resize(sw, sh);
	SetWorkingImage(il);

	settextcolor(txclr);
	setbkcolor(bkclr);
	settextstyle(word_size, 0, _T("Î¢ÈíÑÅºÚ"));
	cleardevice();
	drawtext(sl, &r, uFormat);

	SetWorkingImage(gwi);
}

void DrawTextImg(const TCHAR sl, IMAGE* il, DWORD bkclr, DWORD txclr, int sw, int sh, int word_size, UINT uFormat)
{
	RECT r = { 0, 0, sw, sh };
	IMAGE* gwi = GetWorkingImage();

	il->Resize(sw, sh);
	SetWorkingImage(il);

	settextcolor(txclr);
	setbkcolor(bkclr);
	settextstyle(word_size, 0, _T("Î¢ÈíÑÅºÚ"));
	cleardevice();
	drawtext(sl, &r, uFormat);

	SetWorkingImage(gwi);
}