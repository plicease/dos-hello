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
    int draw_pixel(unsigned int, unsigned int, unsigned char);
    int draw_line(unsigned int, unsigned int, unsigned int, unsigned int, unsigned char);
  private:
    unsigned char far *raw;
    inline void draw_pixel_unsafe(unsigned int, unsigned int, unsigned char);
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
c2_video::draw_pixel_unsafe(unsigned int x, unsigned int y, unsigned char color)
{
  raw[(y<<8)+(y<<6)+x] = color;
}

int
c2_video::draw_pixel(unsigned int x, unsigned int y, unsigned char color)
{
  if(x >= 320)
    return 0;
  if(y >= 200)
    return 0;
  draw_pixel_unsafe(x,y,color);
  return 1;
}

inline int int_sign(int x)
{
  return x < 0 ? -1 : x > 0 ? 1 : 0;
}

int
c2_video::draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color)
{
  if(x1 > 320 || x2 > 320 || y1 > 200 || y2 > 200)
    return 0;

  int dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=int_sign(dx);
  sdy=int_sign(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  raw[(py<<8)+(py<<6)+px]=color;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(int i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      draw_pixel_unsafe(px,py,color);
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(int i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      draw_pixel_unsafe(px,py,color);
    }
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
    v.draw_pixel(i,i,i);
  }

  v.draw_line(20,20,30,40,15);
  v.draw_line(40,40,100,35,14);
  getch();

  return 0;
}
