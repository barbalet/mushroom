/****************************************************************

 shared.c

 =============================================================

 Copyright 1996-2018 Tom Barbalet. All rights reserved.

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
#include "city.h"

#include <stdio.h>

static n_byte  key_identification = 0;
static n_byte2 key_value = 0;
static n_byte  key_down = 0;

extern n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number);

n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number)
{
    printf("ERROR: %s, %s line: %ld\n", error_text, location, line_number);
	return -1;
}

shared_cycle_state shared_cycle(n_uint ticks, n_byte fIdentification, n_int dim_x, n_int dim_y)
{
    return SHARED_CYCLE_OK;
}

n_int shared_init(n_byte view, n_uint random)
{
    n_byte2   seed[4];
    
    printf("random %lu\n", random);
    
    seed[3] = (random >>  0) & 0xffff;
    seed[2] = (random >> 16) & 0xffff;
    seed[1] = (random >> 32) & 0xffff;
    seed[0] = (random >> 48) & 0xffff;
    
    seed[0] ^= seed[2];
    seed[1] ^= seed[3];

    math_random(seed);
    math_random(seed);
    math_random(seed);
    math_random(seed);
    math_random(seed);
    
    game_init(seed);
    city_init(seed);
    /*ecomony_init(seed);*/
    agent_init();
    
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
    n_int integer_rotation_256 = (n_int)((num * 256) / 360);
    agent_turn(integer_rotation_256);
}


void shared_delta(n_double delta_x, n_double delta_y, n_byte wwind)
{
    if (delta_y > 0)
    {
        agent_move(-1);
    }
    if (delta_y < 0)
    {
        agent_move(1);
    }
}

void shared_zoom(n_double num, n_byte wwind)
{
    n_int integer_zoom = (n_int)(num * 100);
    agent_zoom(integer_zoom);
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
    printf("shared_mouseReceived %ld , %ld\n", valX, valY);
}

void shared_mouseUp(void)
{
    printf("shared_mouseUp\n");
}

void shared_about(n_constant_string value)
{
    
}



void shared_draw(n_byte * outputBuffer, n_byte fIdentification, n_int dim_x, n_int dim_y, n_byte size_changed)
{
    n_int turn_delta = 0;
    n_int move_delta = 0;
    n_int zoomed_delta = 0;
    
    if((key_down == 1) && (key_identification == fIdentification))
    {
        n_int mod_key = key_value & 2047;
        n_int shift_key = key_value >> 11;
        
        if ((mod_key > 27) && (mod_key < 32))
        {
            switch (mod_key)
            {
                case 28:
                    turn_delta --;
                    break;
                case 29:
                    turn_delta ++;
                    break;
                case 30:
                    if (shift_key)
                    {
                        zoomed_delta++;
                    }
                    else
                    {
                        move_delta ++;
                    }
                    break;
                default:
                    if (shift_key)
                    {
                        zoomed_delta--;
                    }
                    else
                    {
                        move_delta --;
                    }
                    break;
            }
        }
    }

    agent_turn(turn_delta);
    agent_zoom(zoomed_delta);
    agent_move(move_delta);
    agent_cycle();
    city_cycle();
    game_draw_scene(dim_x, dim_y);
}

n_int shared_new(n_uint seed)
{    
    return 0;
}

n_int shared_new_agents(n_uint seed)
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

n_uint shared_max_fps(void)
{
    return 60;
}

#ifndef	_WIN32

n_int sim_thread_console_quit(void)
{
    return 0;
}

#endif

