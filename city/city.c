/****************************************************************
 
 city.c
 
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

#include "mushroom.h"
#include "city.h"
#include "entity.h"

#define MAX_NUMBER_APES (256)

static noble_being beings[MAX_NUMBER_APES];
static n_uint beings_number;

#define DIMENSION_X (CITY_TOP_RIGHT_X - CITY_BOTTOM_LEFT_X)
#define DIMENSION_Y (CITY_TOP_RIGHT_Y - CITY_BOTTOM_LEFT_Y)

n_byte city_being_can_move(n_vect2 * location, n_vect2 * delta)
{
    return 1;
}

void city_being_wrap(n_vect2 * location)
{
    n_int px = location->x;
    n_int py = location->y;
    
    px = (px + DIMENSION_X) % DIMENSION_X;
    py = (py + DIMENSION_Y) % DIMENSION_Y;
    
    location->x = px;
    location->y = py;
}

void city_being_initial_location(n_vect2 * location, n_byte2 * seed)
{
    location->x = math_random(seed) % DIMENSION_X;
    location->y = math_random(seed) % DIMENSION_Y;
}

void city_translate(n_vect2 * pnt)
{
    pnt->x = pnt->x + CITY_BOTTOM_LEFT_X;
    pnt->y = pnt->y + CITY_BOTTOM_LEFT_Y;
}

n_byte city_line_of_sight(noble_being * being, n_vect2 * location)
{
    n_vect2 start, delta, end;
    n_byte  return_value;
    return_value = being_basic_line_of_sight(being, location, &start, &delta, &end);
    if (return_value != 2)
    {
        return return_value;
    }
    return matrix_visually_open(&start, &end);
}

void city_init(n_byte2 * seed)
{
    beings_number = being_init_group(beings, seed, (MAX_NUMBER_APES * 5)/7, MAX_NUMBER_APES);
    being_wrap_override(&city_being_wrap);
    being_can_move_override(&city_being_can_move);
    being_initial_location_override(&city_being_initial_location);
    being_line_of_sight_override(&city_line_of_sight);
}

void city_listen(noble_being * beings, n_uint beings_number, noble_being * local_being)
{
    being_listen_struct bls;
    n_uint loop = 0;

    if (local_being->delta.awake == 0) return;
    
    bls.max_shout_volume = 127;
    bls.local = local_being;
    /** clear shout values */
    if (local_being->changes.shout[SHOUT_CTR] > 0)
    {
        local_being->changes.shout[SHOUT_CTR]--;
    }
    
    while (loop < beings_number)
    {
        noble_being * other = &beings[loop];
        if (other!= local_being)
        {
            being_listen_loop_no_sim(other, (void *) &bls);
        }
        loop++;
    }
}

void city_sociability(noble_being * beings, n_uint beings_number, noble_being * local_being)
{
    drives_sociability_data dsd;
    n_uint loop = 0;

    dsd.beings_in_vacinity = 0;
    dsd.being = local_being;
    
    while (loop < beings_number)
    {
        noble_being * other = &beings[loop];
        if (other!= local_being)
        {
            drives_sociability_loop_no_sim(other, (void *) &dsd);
        }
        loop++;
    }
    being_crowding_cycle(local_being, dsd.beings_in_vacinity);
}

static void city_calculate_speed(noble_being * local, n_int tmp_speed, n_byte loc_state)
{
    n_int          loc_s      = being_speed(local);
    
    if (tmp_speed > 10) tmp_speed = 10;
    if (tmp_speed < 0) tmp_speed = 0;
    
    if ((local->delta.awake != FULLY_AWAKE) || (loc_state & BEING_STATE_HUNGRY))
    {
        if ((loc_state & BEING_STATE_SWIMMING) != 0)
        {
            tmp_speed = (being_energy(local) >> 7);
        }
        else
        {
            tmp_speed = 0;
        }
    }
    
    if (tmp_speed > loc_s) loc_s++;
    if (tmp_speed < loc_s) loc_s--;
    if (tmp_speed < loc_s) loc_s--;
    
    being_set_speed(local, (n_byte)loc_s);
}

void city_cycle_awake(noble_being * local)
{
    n_int          loc_s      = being_speed(local);
    n_int          loc_h      = being_height(local);
    /** tmp_speed is the optimum speed based on the gradient */
    /** delta_energy is the energy required for movement */
    being_nearest nearest;
    n_int   tmp_speed = being_random(local) & 3;/*being_temporary_speed(local, &test_land, &az);*/
    n_byte  loc_state = BEING_STATE_ASLEEP;/*being_state_find(local, az, loc_s);*/
    
    if (local->delta.awake != FULLY_ASLEEP)
    {
        loc_state |= BEING_STATE_AWAKE;
    }
    
    if (loc_s != 0)
    {
        loc_state |= BEING_STATE_MOVING;
    }
    nearest.opposite_sex = 0L;
    nearest.same_sex = 0L;

    {
        n_vect2 right_close, right_medium, right_far;
        n_vect2 straight_close, straight_medium, straight_far;
        n_vect2 left_close, left_medium, left_far;
        
        being_facing_vector(local, &straight_close, 16);
        
        if (city_line_of_sight(local, &straight_close))
        {
            being_facing_vector(local, &straight_medium, 4);
            if (city_line_of_sight(local, &straight_close))
            {
                
            }
        }
        else
        {
            
        }
        
        being_facing_vector(local, &straight_far, 1);

        /*if (being_line_of_sight(local,))*/
        

        
        /*vect2_direction(vect, value->delta.direction_facing, divisor * 32);*/

    }
    
    being_set_height(local, loc_h);
    being_set_state(local, loc_state);
    city_calculate_speed(local, tmp_speed, loc_state);
    being_genetic_wandering(local, &nearest);
#ifdef TERRITORY_ON
    being_territory_index(local);
#endif
    /*being_mass_calculation(sim, local, loc_state);*/
}

void city_cycle(void)
{
    n_uint loop = 0;
    n_int  max_honor = 0;

    while (loop < beings_number)
    {
        beings[loop].delta.awake = FULLY_AWAKE;
        loop++;
    }

    loop = 0;
    while (loop < beings_number)
    {
        noble_being * local = &beings[loop];
        being_immune_response(local);
        
#ifdef BRAINCODE_ON
#ifdef BRAIN_ON
        /** may need to add external probe linking too */
        /*being_brain_probe(local);*/
#endif
#endif
        
        if ((local->delta.awake == 0) && local)
        {
            being_set_state(local, BEING_STATE_ASLEEP);
            being_reset_drive(local, DRIVE_FATIGUE);
        }
        
        loop++;
    }
    
    loop = 0;
    while (loop < beings_number)
    {
        city_listen(beings, beings_number, &beings[loop]);
        loop++;
    }

    loop = 0;
    while (loop < beings_number)
    {
        episodic_cycle_no_sim(&beings[loop]);
        loop++;
    }
    /*
     if (local_being->delta.awake == 0) return;
     
     being_cycle_awake(local_sim, local_being);
     */
    
    loop = 0;
    
    while (loop < beings_number)
    {
        city_cycle_awake(&beings[loop]);
        loop++;
    }
    
    loop = 0;
    while (loop < beings_number)
    {
        drives_hunger(&beings[loop]);
        loop++;
    }
    
    loop = 0;
    while (loop < beings_number)
    {
        city_sociability(beings, beings_number, &beings[loop]);
        loop++;
    }

    /*drives_sex(local_being, local_being->delta.awake, local_sim);*/
    
    loop = 0;
    while (loop < beings_number)
    {
        drives_fatigue(&beings[loop]);
        loop++;
    }
    
/*
    loop = 0;
    while (loop < beings_number)
    {
        n_byte2 local_brain_state[3];
        noble_being * local = &beings[loop];
        if(being_brainstates(local, (local->delta.awake == 0), local_brain_state))
        {
            n_byte *local_brain = being_brain(local);
            if (local_brain != 0L)
            {
                brain_cycle(local_brain, local_brain_state);
            }
        }
        loop++;
    }
 */
/*
 static void sim_brain_dialogue_loop(noble_simulation * local_sim, noble_being * local_being, void * data)
{
    n_byte     awake = 1;
    n_byte    *local_internal = being_braincode_internal(local_being);
    n_byte    *local_external = being_braincode_external(local_being);
    if(local_being->delta.awake == 0)
    {
        awake=0;
    }
    brain_dialogue(local_sim, awake, local_being, local_being, local_internal, local_external, being_random(local_being)%SOCIAL_SIZE);
    brain_dialogue(local_sim, awake, local_being, local_being, local_external, local_internal, being_random(local_being)%SOCIAL_SIZE);
}
 */
    
    loop = 0;
    while (loop < beings_number)
    {
        being_tidy_loop_no_sim(&beings[loop], &max_honor);
        loop++;
    }

/*
    being_loop(&sim, social_initial_loop, PROCESSING_LIGHT_WEIGHT);
*/
    if (max_honor)
    {
        loop = 0;
        while (loop < beings_number)
        {
            being_recalibrate_honor_loop_no_sim(&beings[loop]);
            loop++;
        }
    }
    /*
    being_loop(&sim, social_secondary_loop, PROCESSING_FEATHER_WEIGHT);

    {
        being_remove_loop2_struct * brls = being_remove_initial(&sim);
        if (sim.ext_death != 0L)
        {
            being_loop_no_thread(&sim, 0L, being_remove_loop1, 0L);
        }
        being_loop_no_thread(&sim, 0L, being_remove_loop2, brls);
        sim_being_remove_final(&sim, &brls);
    }
    */
}

void city_draw(void)
{
    n_uint loop = 0;
    
    gldraw_red();
    
    while (loop < beings_number)
    {
        n_vect2 line_start;
        n_vect2 line_end;
        n_vect2 facing;
        
        line_start.x = line_end.x = beings[loop].delta.location[0];
        line_start.y = line_end.y = beings[loop].delta.location[1];
        
        being_facing_vector(&beings[loop], &facing, 50);
        
        city_translate(&line_start);
        city_translate(&line_end);

        vect2_add(&facing, &line_start, &facing);
        
        gldraw_line(&line_start, &facing);
        
        line_start.x -= 5;
        line_end.x += 5;
        
        gldraw_line(&line_start, &line_end);
        
        line_start.x += 5;
        line_end.x -= 5;
        
        line_start.y -= 5;
        line_end.y += 5;
        
        gldraw_line(&line_start, &line_end);

        loop++;
    }
    matrix_account();
}




