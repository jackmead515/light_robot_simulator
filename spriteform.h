//---------------------------------------------------------------------------
#ifndef spriteformH
#define spriteformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TBoundary
{
public:
  int num;
  int X1, Y1, X2, Y2;
  bool nearBndry, mustClear;

  __fastcall TBoundary(int n, int x1, int y1, int x2, int y2);
  bool __fastcall inShadow(double x, double y, int xl, int yl);
  bool __fastcall hitBoundary(double x, double y, double dy=16.0001);
  bool __fastcall pastBoundary(double x, double y);
  double __fastcall parallelDirection(double u, double v);
  double __fastcall perpendicularDirection(void);
  void __fastcall adjust(double &x, double &y);
  void __fastcall draw(TCanvas *c);
  double __fastcall distance(double x, double y);
  bool __fastcall moveToward(double x, double y, double u, double v);
};
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
  TPaintBox *PB;
  TTimer *Timer1;
  TStatusBar *SB;
  TTimer *Timer2;
  TMainMenu *MainMenu1;
  TMenuItem *Exit1;
  TMenuItem *Restart1;
  TMenuItem *Pause1;
        TMenuItem *Edit1;
        TMenuItem *Intensity1;
        TMenuItem *Speed1;
        TMenuItem *LightSources1;
  void __fastcall Timer1Timer(TObject *Sender);
  void __fastcall FormPaint(TObject *Sender);
  void __fastcall Timer2Timer(TObject *Sender);
  void __fastcall Exit1Click(TObject *Sender);
  void __fastcall Restart1Click(TObject *Sender);
  void __fastcall PBMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Pause1Click(TObject *Sender);

private:	// User declarations
  int RX, RY, X, Y, xx, yy, maxnum, num, LX, LY, L1X, L1Y;
  double x, y, u, v, theta, theta2, pi;
  double xpl, ypl;
  TGraphic *bm;
  TColor sensor;
  TList *FBoundary;
  AnsiString currSound;
  int startSound;

  void __fastcall playSound(AnsiString sound);
  int __fastcall Refract(int ix, int iy, double xl, double yl, double mag);
  void __fastcall PaintBack();
  double __fastcall Intensity2(double xi, double yi, double xl, double yl, double mag);
  double __fastcall Intensity(double xi, double yi);
  double __fastcall Search(bool find = false);
  void __fastcall Rotate(double th);
  void __fastcall Initialize();
  bool __fastcall changeDirection(double u, double v);
  void __fastcall move(void);
  bool __fastcall foundBoundary(double x, double y);
  bool __fastcall stuck(double x, double y);
  int __fastcall get_num_borders() { return FBoundary->Count; };
  TBoundary *getBoundary(int i) { return (TBoundary *)(FBoundary->Items[i-1]); }

public:		// User declarations
  int bnum;

  __fastcall TForm1(TComponent* Owner);
  __property int num_borders = {read=get_num_borders};
  __property TBoundary *boundary[int i] = {read=getBoundary};
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
