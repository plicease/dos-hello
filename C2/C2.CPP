#include <dos.h>
#include <conio.h>
#include <math.h>

/*
 * much of this is borrowed from
 *http://www.brackeen.com/vga/index.html
 */


class c2_video {
  public:
    c2_video();
    ~c2_video();
    int put_pixel(unsigned int, unsigned int, unsigned char);
    int draw_line(unsigned int, unsigned int, unsigned int, unsigned int, unsigned char);
  private:
    unsigned char far *raw;
    inline void put_pixel_unsafe(unsigned int, unsigned int, unsigned char);
};


c2_video::c2_video()
{
  union REGS regs;

  regs.h.ah = 0x00;
  regs.h.al = 0x13;
  int86(0x10, &regs, &regs);

  raw = (unsigned char far *)0xa0000000;
}

inline void
c2_video::put_pixel_unsafe(unsigned int x, unsigned int y, unsigned char color)
{
  raw[(y<<8)+(y<<6)+x] = color;
}

int
c2_video::put_pixel(unsigned int x, unsigned int y, unsigned char color)
{
  if(x >= 320)
    return 0;
  if(y >= 200)
    return 0;
  put_pixel_unsafe(x,y,color);
  return 1;
}

inline int int_sign(int x)
{
  return x < 0 ? -1 : x > 0 ? 1 : 0;
}

int
c2_video::draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color)
{
  int dx,dy,sdx,sdy;

  if(x1 > 320 || x2 > 320 || y1 > 200 || y2 > 200)
    return 0;

  dx=x2-x1;
  dy=y2-y1;
  sdx=int_sign(dx);
  sdy=int_sign(dy);

  if(abs(dx) >= abs(dy))
  {
    float slope = (float)dy / (float)dx;
    for(int i=0; i != dx; i += sdx)
      put_pixel_unsafe(i+x1, slope*i + y1, color);
  }
  else
  {
    float slope = (float)dx / (float)dy;
    for(int i=0; i != dy; i += sdy)
      put_pixel_unsafe(slope*i + x1, i + y1, color);
  }

  return 1;
}

c2_video::~c2_video()
{
  union REGS regs;

  regs.h.ah = 0x00;
  regs.h.al = 0x03;
  int86(0x10, &regs, &regs);
}

int
main(int argc, char *argv[])
{
  int i;

  c2_video v;
  for(i=0; i < 10; i++)
  {
    v.put_pixel(i,i,i);
  }

  v.draw_line(20,20,30,40,15);
  v.draw_line(40,40,100,35,14);
  getch();

  return 0;
}
