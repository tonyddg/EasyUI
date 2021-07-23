# EasiUI
 A easy UI lib based on EasyX

## File Introduction
 This lib contains the following file  
 EasyUI.h : The main head file , which declares the class BaseStuff and others.  
 EasyGraphy.h : The attach head file , which provides some functions to assiste the displaying of some Stuffs.  
 EasyUI.cpp and EasyGraphy.cpp : The cpp file to implement the classes and functions in  EasyUI.h and EasyGraphy.h.  
 *.gif in /res : Rescource file for test.  


## How to use
 If you want to use this lib , make sure you have install the C/C++ graphic lib [EasyX](https://easyx.cn/)  
  
 When you use EasyUI , you can initialize some Stuff , than insert them to a Page.  
 Then you can Draw or Get Command by the member function in Page.  
  
Here is a easy code to show you how to use   
ps.You can get some resources draw by me in fold res for test or other use , such as the "quit.gif" in following code  
 ````C++
#include"EasyUI.h"

int main()
{
	initgraph(640, 480);
	BeginBatchDraw();

	MOUSEMSG m;

	IMAGE imgExitBottom;
	loadimage(&imgExitBottom, _T("quit.gif"));
	TCHAR lpText[256] = _T("Hello World");

	StrStuff ssTextBox = StrStuff(170, 210, 300, 20, lpText, YELLOW, BLACK, 20, DT_CENTER, false, false, false);
	BottomStuff bsExitBottom = BottomStuff(10, 10, &imgExitBottom, BLACK, 0xCC, BLACK, 0x66, false);
	
	ssTextBox.Flush();

	Page pMainPage = Page(0, 0, 640, 480, WHITE, false, false, false, false);

	pMainPage.Insert(&ssTextBox, 0);
	pMainPage.Insert(&bsExitBottom, 1);

	while (true)
	{
		if (PeekMouseMsg(&m))pMainPage.GetCommand(&m);

		if (bsExitBottom.GetState() == STS_BS_CLICKED)
		{
			break;
		}

		pMainPage.Draw();
		FlushBatchDraw();
	}

	EndBatchDraw();
	closegraph();

	return 0;
}
 ````

## Attention

### 1.This Library is still work in place , and it may remind bugs . Please post than at issue , thanks a lot.

### 2.Apart from WorkingImage , origin point and bkmode , the function in this lib will change the drawing setting after being called.