/*
 ******************************************************************
 * HISTORY
 * 15-Oct-94  Jeff Shufelt (js), Carnegie Mellon University
 *      Prepared for 15-681, Fall 1994.
 *
 ******************************************************************
 */

#include "stdafx.h"

#include <stdio.h>
#include "pgmimage.h"
#include "backprop.h"
#include <string>
using namespace std;
extern void exit();

#define TARGET_HIGH 0.9
#define TARGET_LOW 0.1


/*** This is the target output encoding for a network with one output unit.
     It scans the image name, and if it's an image of me (js) then
     it sets the target unit to HIGH; otherwise it sets it to LOW.
     Remember, units are indexed starting at 1, so target unit 1
     is the one to change....  ***/

void load_target(IMAGE *img,BPNN *net)
{
  int scale;
  char userid[40], head[40], expression[40], eyes[40], photo[40];

  userid[0] = head[0] = expression[0] = eyes[0] = photo[0] = '\0';

  /*** scan in the image features ***/
  sscanf(NAME(img), "%[^_]_%[^_]_%[^_]_%[^_]_%d.%[^_]",
    userid, head, expression, eyes, &scale, photo);

  char *p=NULL;
  p = strrchr(eyes,'.');
  if(p!=NULL)
  {
	    userid[0] = head[0] = expression[0] = eyes[0] = photo[0] = '\0';

	    sscanf(NAME(img), "%[^_]_%[^_]_%[^_]_%[^.].%[^_]",
    userid, head, expression, eyes, photo);
		scale = 1;
  }

  p = strrchr(userid,'\\');
  if(p!=NULL)
	  p++;

#if TARGET == TARGET_glasses
  if (!strcmp(eyes, "sunglasses")) {
    net->target[1] = TARGET_HIGH;  /* it's me, set target to HIGH */
  } else {
    net->target[1] = TARGET_LOW;   /* not me, set it to LOW */
  }
  
#elif TARGET == TARGET_emotion
  for (int i = 1; i <= 4; i++) {
      net->target[i] = TARGET_LOW;   /* not me, set it to LOW */
  }
  if (!strcmp(expression, "angry")) {
      net->target[1] = TARGET_HIGH;  /* it's me, set target to HIGH */
  }
  if (!strcmp(expression, "happy")) {
      net->target[2] = TARGET_HIGH;  /* it's me, set target to HIGH */
  }
  if (!strcmp(expression, "neutral")) {
      net->target[3] = TARGET_HIGH;  /* it's me, set target to HIGH */
  }
  if (!strcmp(expression, "sad")) {
      net->target[4] = TARGET_HIGH;  /* it's me, set target to HIGH */
  }
  
#elif TARGET == TARGET_head
  for (int i = 1; i <= 4; i++) {
      net->target[i] = TARGET_LOW;   /* not me, set it to LOW */
  }
  if (!strcmp(head, "left")) {
      net->target[1] = TARGET_HIGH;  /* it's me, set target to HIGH */
  }
  if (!strcmp(head, "right")) {
      net->target[2] = TARGET_HIGH;  /* it's me, set target to HIGH */
  }
  if (!strcmp(head, "straight")) {
      net->target[3] = TARGET_HIGH;  /* it's me, set target to HIGH */
  }
  if (!strcmp(head, "up")) {
      net->target[4] = TARGET_HIGH;  /* it's me, set target to HIGH */
  }
#elif TARGET == TARGET_who
  for (int i = 1; i <= 20; i++) {
      net->target[i] = TARGET_LOW;   /* not me, set it to LOW */
  }
  string strs[] = { "", "an2i", "at33", "boland", "bpm", "ch4f", "cheyer", "choon", "danieln", "glickman", "karyadi", "kawamura", "kk49", "megak", "mitchell", "night", "phoebe", "saavik", "steffi", "sz24", "tammo" };
  for (int i = 1; i <= 20; i++) {
      if (!strcmp(head, strs[i].c_str())) {
        net->target[i] = TARGET_HIGH;  /* it's me, set target to HIGH */
      }
  }
#endif
}


/***********************************************************************/
/********* You shouldn't need to change any of the code below.   *******/
/***********************************************************************/

void load_input_with_image(IMAGE *img,BPNN *net)
{
  double *units;
  int nr, nc, imgsize, i, j, k;

  nr = ROWS(img);
  nc = COLS(img);
  imgsize = nr * nc;;
  
  units = net->input_units;
  
#ifdef NAIVE
  if (imgsize != net->input_n) {
    printf("LOAD_INPUT_WITH_IMAGE: This image has %d pixels,\n", imgsize);
    printf("   but your net has %d input units.  I give up.\n", net->input_n);
    exit (-1);
  }

   //在这里修改输入单元
  
  k = 1;
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++) {
      units[k] = ((double) img_getpixel(img, i, j)) / 255.0;
      k++;
    }
  }
#else
  
  int img_size = 128;
  int size = 32;
  int n = img_size / (size / 2) - 1;
  int count = 1;
  int e[8][2] = { {0, -2}, {1, -1}, {2, 0}, {1, 1}, {0, 2}, {-1, 1}, {-2, 0}, {-1, -1} };
  int x0 = 0, y0 = 0;
  for (int i = 0; i < n; i++) {
      y0 = 0;
      for (int j = 0; j < n; j++) {
          int sum[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
          for (int p = x0 + 2; p < x0 + size - 2; p++) {
              for (int q = y0 + 2; q < y0 + size - 2; q++) {
                  int d = img_getpixel(img, p, q);
                  for (int k = 0; k < 8; k++) {
                      sum[k] += d > img_getpixel(img, p + e[k][0], q + e[k][1]) ? 1 : 0;
                  }
              }
          }
          for (int k = 0; k < 8; k++) {
              units[count] = double(sum[k]) / ((size - 2) * (size - 2));
              count++;
          }
          y0 += size / 2;

      }
      x0 += size / 2;
  }
  
  /**
  int e[8][2] = { { 0, -2 },{ 1, -1 },{ 2, 0 },{ 1, 1 },{ 0, 2 },{ -1, 1 },{ -2, 0 },{ -1, -1 } };
  int x1 = 32;
  int x2 = 96;
  int y1 = 72;
  int y2 = 88;
  int x0 = x1;
  int y0 = y1;
  int count = 1;
  int size = 16;
  int n = (x2 - x1) / (size / 2) - 1;//7
  for (int i = 0; i < n; i++) {
      int sum[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
      for (int p = x0 + 2; p < x0 + size - 2; p++) {
          for (int q = y0 + 2; q < y0 + size - 2; q++) {
              int d = img_getpixel(img, p, q);
              for (int k = 0; k < 8; k++) {
                  sum[k] += d > img_getpixel(img, p + e[k][0], q + e[k][1]) ? 1 : 0;
              }
          }
      }
      for (int k = 0; k < 8; k++) {
          units[count] = double(sum[k]) / ((size - 2) * (size - 2));
          count++;
      }
      x0 += size / 2;
  }*/
  
  /**
  int x1 = 32;
  int x2 = 96;
  int y1 = 72;
  int y2 = 88;
  // 64 * 16
  int count = 1;
  for (int i = x1; i < x2; i++) {
      for (int j = y1; j < y2; j++) {
          int d = img_getpixel(img, i, j);
          units[count] = double(d) / 255;
          count++;
      }
  }
  */
#endif
}
