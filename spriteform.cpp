//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <math.h>
#include <stdlib.h>
#include <mmsystem.hpp>
#include "spriteform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TBoundary::TBoundary(int n, int x1, int y1, int x2, int y2)
{
  nearBndry = mustClear = false;
  num = n;
  X1 = x1; Y1 = y1; X2 = x2; Y2 = y2;
}
//---------------------------------------------------------------------------
double __fastcall TBoundary::distance(double x, double y)
{
  double t, d1, d2;
  t = ((X2-X1)*(x-X1) + (Y2-Y1)*(y-Y1)) / ((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1));
  if (0.0 <= t && t <= 1.0) return hypot(t * (X2 - X1) + X1 - x, t * (Y2 - Y1) + Y1 - y);
  d1 = hypot(X1-x, Y1-y);
  d2 = hypot(X2-x, Y2-y);
  return (d1 < d2) ? d1 : d2;
}
//---------------------------------------------------------------------------
bool __fastcall TBoundary::inShadow(double x, double y, int xl, int yl)
{
  float d, t, s;
  x -= xl;
  y -= yl;
  xl -= X1;
  yl -= Y1;
  d = (Y2 - Y1) * x - (X2 - X1) * y;
  t = (x * yl - y * xl) * d;
  s = (yl * (X2 - X1) - xl * (Y2 - Y1)) * d;
  d *= d;
  return (t > 0) && (t < d) && (s > 0) && (s < d);
}
//---------------------------------------------------------------------------
void __fastcall TBoundary::adjust(double &x, double &y)
{
  double xp, xx, dx, dy;
  xx = hypot(X2-X1, Y2-Y1);
  dx = (X2 - X1) / xx;
  dy = (Y2 - Y1) / xx;
  xp = (x - X1) * dy - (y - Y1) * dx;
  if (xp > 0) xp = 16 - xp;
  else xp = -16 - xp;
  x += xp * dy;
  y -= xp * dx;
}
//---------------------------------------------------------------------------
bool __fastcall TBoundary::moveToward(double x, double y, double u, double v)
{
  double xp, vp, xx;

  x -= X1;
  y -= Y1;
  xx = hypot(X2-X1, Y2-Y1);
  xp = (x * (Y2 - Y1) - y * (X2 - X1)) / xx;
  vp = (u * (Y2 - Y1) - v * (X2 - X1)) / xx;
  return xp*vp < 0;
}
//---------------------------------------------------------------------------
bool __fastcall TBoundary::hitBoundary(double x, double y, double dy)
{
  double xp, yp, xx;

  x -= X1;
  y -= Y1;
  xx = hypot(X2-X1, Y2-Y1);
  yp = fabs((x * (X2 - X1) + y * (Y2 - Y1)) / xx - 0.5 * xx) - 0.5 * xx;
  xp = fabs((x * (Y2 - Y1) - y * (X2 - X1)) / xx);
  return (yp <= dy) && (xp <= 16.0001);
}
//---------------------------------------------------------------------------
bool __fastcall TBoundary::pastBoundary(double x, double y)
{
  double xp, yp, xx;

  x -= X1;
  y -= Y1;
  xx = hypot(X2-X1, Y2-Y1);
  xp = fabs((x * (Y2 - Y1) - y * (X2 - X1)) / xx);
  yp = fabs((x * (X2 - X1) + y * (Y2 - Y1)) / xx - 0.5 * xx) - 0.5 * xx;
  return (xp > 18) || (yp > 32);
}
//---------------------------------------------------------------------------
double __fastcall TBoundary::parallelDirection(double u, double v)
{
  double vp, xx;
  xx = hypot(X2-X1, Y2-Y1);
  vp = (u * (X2 - X1) + v * (Y2 - Y1)) / xx;
  if (vp > 0) return atan2(Y2-Y1,X2-X1);
  return atan2(Y1-Y2,X1-X2);
}
//---------------------------------------------------------------------------
double __fastcall TBoundary::perpendicularDirection()
{
  return atan2(X2-X1,Y1-Y2);
}
//---------------------------------------------------------------------------
void __fastcall TBoundary::draw(TCanvas *c)
{
  c->Pen->Color = clSilver;
  c->MoveTo(X2, Y2);
  c->LineTo(X1, Y1);
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
  TBoundary *b;
  int i;
  int bndry[13][4] = {
    {400, 500, 300, 500},
    {300, 500, 300, 200},
    {300, 200, 600, 200},
    {600, 200, 600, 500},
    {600, 500, 440, 500},
    {440, 500, 440, 492},
    {440, 492, 592, 492},
    {592, 492, 592, 208},
    {592, 208, 308, 208},
    {308, 208, 308, 492},
    {308, 492, 400, 492},
    {400, 492, 400, 500},
    {200, 50, 100, 200}
  };

  currSound = "";
  randomize();
  bnum = 0;
  sensor = (TColor)0xFFFF00;
  pi = atan2(0.0, -1.0);
  PB->Canvas->CopyMode = cmSrcInvert;
  bm = new Graphics::TBitmap();
  bm->LoadFromFile("robot.bmp");

  FBoundary = new TList();
  for (i=0; i < 13; i++) {
    b = new TBoundary(i+1, bndry[i][0], bndry[i][1], bndry[i][2], bndry[i][3]);
    FBoundary->Add(b);
    SB->Panels->Add();
    SB->Panels->Items[SB->Panels->Count-1]->Width = 100;
  }
  L1X = random(PB->ClientWidth-40) + 20;
  L1Y = random(PB->ClientHeight-40) + 20;
  LX = random(PB->ClientWidth-40) + 20;
  LY = random(PB->ClientHeight-40) + 20;
  do {
    RX = random(PB->ClientWidth-32);
    RY = random(PB->ClientHeight-32);
  } while (Intensity(RX, RY) != 0);
  Initialize();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Initialize()
{
  int i;

  for (i=0; i < SB->Panels->Count; i++) SB->Panels->Items[i]->Text = "";
  for (i=1; i <= num_borders; i++) {
    boundary[i]->nearBndry = false;
    boundary[i]->mustClear = false;
  }
  startSound = 0;
  currSound = "";
  xpl = ypl = 0.0;
  x = X = RX;
  y = Y = RY;
  xx = X+28; yy = Y+16;
  u = 12.0;
  v = 0.0;
  theta = theta2 = Search(true);
  Timer1->Enabled = true;
  Timer2->Enabled = false;
  playSound("emotor.wav");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::playSound(AnsiString sound)
{
  if (sound != currSound) {
    sndPlaySound(sound.c_str(), (sound == "emotor.wav") ? (SND_ASYNC|SND_LOOP) : SND_ASYNC);
    startSound = 0;
  }
  currSound = sound;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Rotate(double th)
{
  num = 0;
  theta2 = th;
  if (theta2-theta < -pi) theta2 += 2*pi;
  if (theta2-theta > pi) theta2 -= 2*pi;
  maxnum = floor(36 * fabs(theta2 - theta) / pi + 1);
  if (maxnum > 12) maxnum = 12;
  if (! Timer2->Enabled) {
    playSound("shortturn.wav");
  }
  Timer1->Enabled = false;
  Timer2->Enabled = true;
}
//---------------------------------------------------------------------------
double __fastcall TForm1::Search(bool find)
{
  double i1, i2, th, save, dth = pi / 36;
  double c, s, r = 12;
  int i, ix, iy;
  save = theta;
  do {
    i1 = Intensity(xx, yy);
    for (i=1; i < 72; i++) {
      th = theta + i * dth;
      ix = X + 16 + floor(r * cos(th) + .5);
      iy = Y + 16 + floor(r * sin(th) + .5);
      i2 = Intensity(ix, iy);
      if (i2 > i1) {
        i1 = i2;
        save = th;
      }
    }
    r += 12;
  } while (find && i1 == 0);
  s = (sin(save) + 5000.0) - 5000.0;
  c = (cos(save) + 5000.0) - 5000.0;
  if ((y <= 0 && s < 0) || (y >= PB->ClientHeight-32 && s > 0))
    save = atan2(0.0, c?c:(PB->ClientWidth-2*x));
  if ((x >= PB->ClientRect.Right-32 && c > 0) || (x <= 0 && c < 0))
    save = atan2(s?s:(PB->ClientHeight-2*y), 0.0);
//  if (theta != save) Rotate(save);
  return save;
}
//---------------------------------------------------------------------------
bool __fastcall TForm1::foundBoundary(double x, double y)
{
  int i;
  for (i=1; i <= num_borders; i++) {
    if (i != bnum && boundary[i]->hitBoundary(x, y, 12.0) && boundary[i]->moveToward(x,y,u,v)) {
      boundary[i]->nearBndry = true;
      bnum = i;
//      sndPlaySound("click.wav", SND_SYNC);
//      sndPlaySound("emotor.wav", SND_ASYNC|SND_LOOP);
      return true;
    }
  }
  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TForm1::stuck(double x, double y)
{
  return (fabs(x - X) <= 1 && fabs(y - Y) <= 1);
}
//---------------------------------------------------------------------------
bool __fastcall TForm1::changeDirection(double u, double v)
{
  int k, save = bnum;
  double c, s, u1, v1, thp, th = theta;

  for (k=1; k <= num_borders; k++)
    SB->Panels->Items[k]->Text = IntToStr(k) + ": " + IntToStr(boundary[k]->nearBndry);
  if (bnum) {
    if (foundBoundary(x+16, y+16)) {
      th = boundary[bnum]->parallelDirection(u, v);
      if (boundary[save]->moveToward(x, y, u, v)) th += pi;
      c = x + 16;
      s = y + 16;
      boundary[bnum]->adjust(c, s);
      x = c - 16;
      y = s - 16;
    } else if (boundary[bnum]->hitBoundary(x+16, y+16)) {
      th = theta;
    } else if (boundary[bnum]->mustClear) {
      boundary[bnum]->mustClear = ! boundary[bnum]->pastBoundary(x+16, y+16);
    } else if (boundary[bnum]->nearBndry) {
      boundary[bnum]->mustClear = true;
      boundary[bnum]->nearBndry = false;
      thp = boundary[bnum]->parallelDirection(u, v);
      th = Search(true);
      if (x == xpl && y == ypl) th += pi;
      xpl = x;
      ypl = y;
      if (th-thp > pi)  th -= 2*pi;
      if (th-thp < -pi) th += 2*pi;
      if (fabs(th-thp) > 0.5 * pi) {
        c = cos(th);
        s = sin(th);
        th = boundary[bnum]->perpendicularDirection();
        u1 = cos(th);
        v1 = sin(th);
        if (u1 * c + v1 * s < 0) th += pi;
      }
    } else {
      bnum = 0;
      th = Search();
    }
  } else if (foundBoundary(x+16, y+16)) {
    th = boundary[bnum]->parallelDirection(u, v);
    c = 12*cos(th);
    s = 12*sin(th);
    if (Intensity(x+16-c,y+16-s) > Intensity(x+16+c,y+16+s)) {
      th += pi;
    }
    c = x + 16;
    s = y + 16;
    boundary[bnum]->adjust(c, s);
    x = c - 16;
    y = s - 16;
  } else {
    th = Search();
  }
  if (th - theta >= 2*pi) th -= 2*pi;
  if (th - theta <= -2*pi) th += 2*pi;
  if (th != theta) Rotate(th);
  return Timer2->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::move(void)
{
  int t;
  double dx, dy;

  u = cos(theta);
  v = sin(theta);
  dx = .01 * u * Timer1->Interval;
  dy = .01 * v * Timer1->Interval;

  for (t=0; t < 12; t++) {
    x += dx;
    y += dy;
    if (Intensity(xx,yy) == 1) {
      sndPlaySound(NULL, SND_SYNC);
      Timer1->Enabled = false;
      Timer2->Enabled = false;
      return;
    }
    if (changeDirection(u, v)) break;
  }

  X = x;
  Y = y;
  xx = x + 16 + floor(12*u + .5);
  yy = y + 16 + floor(12*v + .5);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  if (++startSound > 20 && currSound == "shortturn.wav")
    playSound("emotor.wav");
  PB->Canvas->Brush->Color = sensor;
  PB->Canvas->Pen->Color = sensor;
  PB->Canvas->Pen->Mode = pmXor;

  // erase
  PB->Canvas->Ellipse(xx-2,yy-2,xx+2,yy+2);
  PB->Canvas->Draw(X,Y,bm);

  move();

  // draw
  PB->Canvas->Draw(X,Y,bm);
  PB->Canvas->Ellipse(xx-2,yy-2,xx+2,yy+2);
}
//---------------------------------------------------------------------------
double __fastcall TForm1::Intensity2(double xi, double yi, double xl, double yl, double mag)
{
  double r, I;
  for (int i=1; i <= num_borders; i++) if (boundary[i]->inShadow(xi, yi, xl, yl)) return 0.0;
  xi -= xl;
  yi -= yl;

  I = mag;
  r = sqrt(xi*xi + yi*yi);
  if (r != 0) I = mag / r;
  return I;
}
//---------------------------------------------------------------------------
double __fastcall TForm1::Intensity(double xi, double yi)
{
  double I1, I;

  I = Intensity2(xi, yi, LX, LY, 1.0);

  for (int i=1; i <= num_borders; i++) {
    I1 = Intensity2(boundary[i]->X1, boundary[i]->Y1, LX, LY, 1.0);  // last parameter is magnitude of intensity
    if (I1) I += Intensity2(xi, yi, boundary[i]->X1, boundary[i]->Y1, .1*I1);

    I1 = Intensity2(boundary[i]->X2, boundary[i]->Y2, LX, LY, 1.0);
    if (I1) I += Intensity2(xi, yi, boundary[i]->X2, boundary[i]->Y2, .1*I1);
  }

  return I;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintBack()
{
  int ix, iy, col, rcol;

  sndPlaySound(NULL, SND_SYNC);
  currSound = "";
  PB->Canvas->Brush->Color = clBlack;
  PB->Canvas->FillRect(PB->ClientRect);
  for (ix=0; ix < ClientWidth; ix++) {
    PB->Canvas->MoveTo(ix, 0);
    for (iy=0; iy < ClientHeight; iy++) {
      rcol = 0;
/*
      for (int i=1; i < num_borders; i++) {
        if (Intensity2(boundary[i]->X1, boundary[i]->Y1, LX, LY, 10*Intensity2(ix,iy,LX,LY,1.0)))
          rcol |= Refract(ix, iy, boundary[i]->X1, boundary[i]->Y1, 1.0);
        if (Intensity2(boundary[i]->X2, boundary[i]->Y2, LX, LY, 10*Intensity2(ix,iy,LX,LY,1.0)))
          rcol |= Refract(ix, iy, boundary[i]->X2, boundary[i]->Y2, 1.0);
      }
*/
      col = floor(2550 * Intensity(ix, iy) + 0.5);
      if (col > 255) col = 255;                                 //intensity of light from 0 to 256
      PB->Canvas->Pen->Color = (TColor)(col * 0x010101 | rcol); //hex color for light
      PB->Canvas->LineTo(ix, iy);
    }
  }
  playSound("emotor.wav");
}
//---------------------------------------------------------------------------
int __fastcall TForm1::Refract(int ix, int iy, double xl, double yl, double mag)
{
  int col;

  col = floor(2550 * Intensity2(ix, iy, xl, yl, mag) + 0.5);
  if (col > 255) col = 255;
  return col;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormPaint(TObject *Sender)
{
  PaintBack();
  for (int i=1; i <= num_borders; i++) boundary[i]->draw(PB->Canvas);
  PB->Canvas->Draw(X,Y,bm);
  PB->Canvas->Brush->Color = sensor;
  PB->Canvas->Pen->Color = sensor;
  PB->Canvas->Pen->Mode = pmXor;
  PB->Canvas->Ellipse(xx-2,yy-2,xx+2,yy+2);
  Search();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
  double th = (num*theta2 + (maxnum-num)*theta) / maxnum;
  ++startSound;
  Timer1->Enabled = (num++ == maxnum);
  PB->Canvas->Brush->Color = sensor;
  PB->Canvas->Pen->Color = sensor;
  PB->Canvas->Ellipse(xx-2,yy-2,xx+2,yy+2);
  xx = X + 16 + floor(12 * cos(th) + .5);
  yy = Y + 16 + floor(12 * sin(th) + .5);
  PB->Canvas->Brush->Color = sensor;
  PB->Canvas->Pen->Color = sensor;
  PB->Canvas->Ellipse(xx-2,yy-2,xx+2,yy+2);
  Timer2->Enabled = ! Timer1->Enabled;
  if (Timer1->Enabled) {
    theta = atan2(sin(theta2),cos(theta2));
//    if (maxnum > 3) playSound("emotor.wav");
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Exit1Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Restart1Click(TObject *Sender)
{
  Initialize();
  Form1->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PBMouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
  if (Button == mbRight) {
    LX = X;                     //light starting position
    LY = Y;                     //
  } else if (Button == mbLeft) {
    RX = X;                     //Rover starting position
    RY = Y;                     //
    SB->Panels->Items[0]->Text = Intensity(X, Y);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Pause1Click(TObject *Sender)
{
  Timer1->Enabled = !Timer1->Enabled;
  currSound = "";
  if (Timer1->Enabled)
    playSound("emotor.wav");
  else
    sndPlaySound(NULL, SND_SYNC);
}
//---------------------------------------------------------------------------

