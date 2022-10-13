/*
 Timer module
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

#ifndef __TIMER_H
#define __TIMER_H

// Globals for inline access
extern float elapsedtime;
extern int   currentframe;
extern float gCurrentTime;

// Timer functions
void    timer_init         (void);
void    timer_reset        (void);
void    timer_update       (int paused);
#define timer_getelapsed() (elapsedtime)
#define timer_getframe()   (currentframe)
#define timer_getcurrent() (gCurrentTime)

#endif
