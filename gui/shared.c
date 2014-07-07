/****************************************************************

 shared.h

 =============================================================

 Copyright 1996-2014 Tom Barbalet. All rights reserved.

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

 This software and Noble Ape are a continuing work of Tom Barbalet,
 begun on 13 June 1996. No apes or cats were harmed in the writing
 of this software.

 ****************************************************************/

#include "shared.h"
#include "mushroom.h"
#include <stdio.h>

static n_byte  screen[1024 * 768] = {0};

static n_byte  key_identification = 0;
static n_byte2 key_value = 0;
static n_byte  key_down = 0;

extern n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number);
extern n_byte * draw_screen(void);

static n_byte pixel_black(n_int px, n_int py, n_int dx, n_int dy, void * information)
{
    n_byte	*local_col = information;
    
    if ((px < 0) || (py < 0) || (px > 1023) || (py > 767))
    {
        return 0;
    }
    
    local_col[ px | (py << 10) ] = 255;
    return 0;
}

n_byte * draw_screen(void)
{
    return screen;
}

void draw_line(n_int x1, n_int y1, n_int x2, n_int y2)
{
    n_pixel 	 * local_draw = &pixel_black;
    n_join		   local_kind;
    n_int        dx = x2-x1;
    n_int        dy = y2-y1;
    local_kind.pixel_draw = local_draw;
    local_kind.information = screen;
    
    (void)math_join(x1, y1, dx, dy, &local_kind);
}

n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number)
{
    printf("ERROR: %s, %s line: %ld\n", error_text, location, line_number);
	return -1;
}

shared_cycle_state shared_cycle(n_uint ticks, n_byte fIdentification, n_int dim_x, n_int dim_y)
{
    if((key_down == 1) && (key_identification == fIdentification))
    {
        /* control_key(key_identification, key_value); */
    }
    
    return SHARED_CYCLE_OK;
}

n_int shared_init(n_byte view, n_uint random)
{
    n_byte2   seed[4] = {0x343e, 0xf323, 0xed32, 0x32fa};
    enemy_init();
    ecomony_init(seed);
    return 0;
}

void shared_close(void)
{

}

n_int shared_menu(n_int menuValue)
{
    return 0;
}

void shared_rotate(n_double num, n_byte wwind)
{
    
}

void shared_keyReceived(n_byte2 value, n_byte fIdentification)
{
    key_down = 1;
    key_value = value;
    key_identification = fIdentification;
}

void shared_keyUp(void)
{
    key_down = 0;
}

void shared_mouseOption(n_byte option)
{
    
}

void shared_mouseReceived(n_int valX, n_int valY, n_byte fIdentification)
{

}

void shared_mouseUp(void)
{
    
}

void shared_about(n_constant_string value)
{
    
}

static n_int time_cycle = 0;

n_byte * shared_draw(n_byte fIdentification)
{
    static n_byte2  seed[2] = {0xf728, 0xe231};
    n_vect2         center;
    time_cycle ++;
    if (time_cycle > 59)
    {
        n_int px = 0;
        economy_draw(0, 0);
        while (px < 4)
        {
            n_int py = 0;
            while (py < 4)
            {
                noble_building * building = house_create(seed);
                
                vect2_populate(&center, -100 + (px * 400), -100 + (py * 400));
                time_cycle = 0;
                /*io_erase(screen, (1024*768));*/
                
                if (building)
                {
                    house_transform(building, &center, math_random(seed) & 255);
                    house_draw(building);
                    io_free((void **)&building);
                }
                py++;
            }
            px++;
        }
    }
    /*enemy_move();*/

    return screen;
}

void shared_color(n_byte2 * fit, n_int fIdentification)
{
    n_int loop = 0;
    n_int fitloop = 0;
    while (loop < 256)
    {
        fit[fitloop++] = loop << 8;
        fit[fitloop++] = loop << 8;
        fit[fitloop++] = loop << 8;
        
        loop++;
    }

}

n_int shared_new(n_uint seed)
{    
    return 0;
}

n_byte shared_openFileName(n_string cStringFileName, n_byte isScript)
{
    return 0;
}

void shared_saveFileName(n_string cStringFileName)
{
    
}

void shared_script_debug_handle(n_string cStringFileName)
{
    
}

#ifndef	_WIN32

n_int sim_thread_console_quit(void)
{
    return 0;
}

#endif

