#ifndef TIMELINE_STATUS_H
#define TIMELINE_STATUS_H

#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "com_protocol.h"


// for use in Choreography comstatemanager
int initialize_timeline( char n_parts );
int set_timeline_part( char * part_string );
int apply_timeline();

// for use in choreography actionManagement
com_pcode get_ongoing_primitive( int current_time );

#endif // TIMELINE_STATUS_H
