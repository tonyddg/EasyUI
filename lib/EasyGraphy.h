#pragma once
#include<graphics.h>

//范围检查
#define NumCheck(Obj, L, R) ((((Obj) >= (L)) && ((Obj) < (R))))
#define inRange(Obj, L, R) max(min((Obj), (R)), (L))

//当两个矩形不相交时返回真
bool RectCheck(int al, int at, int ar, int ab, int bl, int bt, int br, int bb);
bool RectCheck(RECT &a, RECT &b);

//适用于显存中的颜色 0x	AARRGGBB
#define GA(a) ((((a) & 0xFF000000) >> 24) & 0xff)
#define GR(a) ((((a) & 0x00FF0000) >> 16) & 0xff)
#define GG(a) ((((a) & 0x0000FF00) >> 8) & 0xff)
#define GB(a) ((((a) & 0x000000FF)) & 0xff)
#define ARGB(a,r,g,b) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b)  & 0xffffffff)

#define BGRtoARGB(bgr) ARGB(0, GB(bgr), GG(bgr), GR(bgr))//bgr : 0x__000000
#define ARGBtoBGR(argb) ARGB(GB(argb), GG(argb), GR(argb), 0)

//<<表示除256 b 为覆盖色，a 为底色，alpha 越大越透明
#define AlphaCocula(a,b,alpha) ARGB(0, ((GR((a)) * (alpha)) + (GR((b)) * (256 - (alpha)))) >> 8, ((GG((a)) * (alpha)) +(GG((b)) * (256 - (alpha)))) >> 8, ((GB((a)) * (alpha)) + (GB((b)) * (256 - (alpha)))) >> 8)

//读取指针形式的二维数组 相当于l[a][b] 要加括号!!!
#define DW(a,b,w) (a) * (w) + (b) 

void AlphaRectangle(int l, int t, int r, int b, DWORD clr, DWORD alpha);

void PutImgWithout(IMAGE* obj, int px, int py, COLORREF withouter);