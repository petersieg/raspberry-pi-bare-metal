/*
 * Copyright (c) 2014 Marco Maccaferri and Others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// Adapted from: https://www.rosettacode.org/wiki/Number_reversal_game#C
// 03/2019 peter.sieg2@gmx.de

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "kernel.h"
#include "wiring.h"

#define puts addstr
#define putchar addch

#if BYTES_PER_PIXEL == 2

#define BORDER_COLOR        RGB(26, 5, 10)
#define BACKGROUND_COLOR    RGB(12, 0, 4)

#elif BYTES_PER_PIXEL == 4

#define BORDER_COLOR        RGB(213, 41, 82)
#define BACKGROUND_COLOR    RGB(98, 0, 32)

#endif

#if defined(__cplusplus)
extern "C" {
#endif

__attribute__ ((interrupt ("IRQ"))) void interrupt_irq() {
#ifdef HAVE_USPI
    USPiInterruptHandler ();
#endif
}

#if defined(__cplusplus)
}
#endif

int check_array(int *arr, int len)
{
    while(--len)
    {
        if(arr[len]!=(arr[len-1]+1))
            return 0;
    }
    return 1;
}

void shuffle_list(int *list, int len)
{
    //We'll just be swapping 100 times. Could be more/less. Doesn't matter much.
    int n=100;
    int a=0;
    int b=0;
    int buf = 0;
    //Random enough for common day use..
    srand(time(NULL));
    while(n--)
    {
        //Get random locations and swap
        a = rand()%len;
        b = rand()%len;
        buf = list[a];
        list[a] = list[b];
        list[b] = buf;
    }
    // "shuffled to ordered state" fix:
    if (check_array(list, len)) shuffle_list (list, len);
}
 
void do_flip(int *list, int length, int num)
{
    //Flip a part on an array
    int swap;
    int i=0;
    for(i;i<--num;i++)
    {
        swap=list[i];
        list[i]=list[num];
        list[num]=swap;
    }
}
 
#define printf addstrf

void main() 
{
    int c;
    int tries;
    unsigned int i;
    int input;
    int list[9] = {1,2,3,4,5,6,7,8,9};

    // Default screen resolution (set in config.txt or auto-detected)
    //fb_init(0, 0);

    // Sets a specific screen resolution
    fb_init(32 + 320 + 32, 32 + 200 + 32);

    fb_fill_rectangle(0, 0, fb_width - 1, fb_height - 1, BORDER_COLOR);

    initscr(40, 25);
    cur_fore = WHITE;
    cur_back = BACKGROUND_COLOR;
    clear();

    if (mount("sd:") != 0) {
        addstrf("\r\nSD CARD NOT MOUNTED (%s)\r\n", strerror(errno));
    }

    usb_init();
    if (keyboard_init() != 0) {
        addstr("\r\nNO KEYBOARD DETECTED\r\n");
    }
    while(1) 
    { 
    clear();
    printf("Number Reversal Game.\r\n");
    printf("Type 2..9 to flip the first n numbers.\r\n");
    printf("Sort the numbers in ascending order.\r\n");
    printf("Anything besides numbers are ignored.\r\n");
    printf("\t  |1__2__3__4__5__6__7__8__9|\r\n");
    shuffle_list(list,9);
    tries=0;

      while(1) 
      { 
        while(!check_array(list, 9))
        {
          ((tries<10) ? printf("\r\nRound %d :  ", tries) : printf("\r\nRound %d:  ", tries));
          for(i=0;i<9;i++)printf("%d  ",list[i]);
          printf("  Input number: ");
	  while (1) 
          {
            if ((c = getch()) != -1)
            {
            //Just keep asking for proper input
            //scanf("%d", &input);
	    input = (c - 48); // convert int 48.. to int 0..9
            if(input>1&&input<10)
                break;
 
            printf("\r\n%d - Please enter a number between 2 and 9: ", c);
            refresh();
            }
          }        
          tries++;
          do_flip(list, 9, input);
          refresh();
        }
        printf("\r\nHurray! You solved it in %d moves!\r\n", tries);
        shuffle_list(list,9);
        tries=0;
        while(1) {
          if ((c = getch()) != -1) break;
        }
     }
    }
    refresh();
}
