#include "timeline_status.h"

struct part
{
    com_pcode primitive;
    int pause_units;
    int start_time; // [seconds]
};

struct timeline
{
    int n_parts;
    int n_parts_set;
    struct part parts[COM_P_TIMELINE_MAX_PARTS+1];
};

static struct timeline temp_timeline;
static struct timeline main_timeline;

// Choreographer functions :
// ------------------------

int initialize_timeline( char n_parts )
{
    temp_timeline.n_parts = n_parts;
    temp_timeline.n_parts_set = 0;

    return 1;
}

int set_timeline_part( char * part_string )
{
    int n = (unsigned char) part_string[0];

    // check that this part is the expected one
    if ( temp_timeline.n_parts_set < n     ) { return 0; }
    if ( n <  temp_timeline.n_parts_set -1 ) { return 0; }
 
    // if the computer resent the previous part
    if ( n == temp_timeline.n_parts_set -1 ) { return 1; }


    // if this is the expected next part ( n == n_parts_set )
    // create the part
    struct part temp_part;
    temp_part.primitive   = part_string[1];
    temp_part.pause_units = part_string[2];
    if ( n == 0 ) temp_part.start_time = 0;
    if ( n  > 0 ) temp_part.start_time = 
            temp_timeline.parts[n-1].start_time
          + get_primitive_duration( temp_timeline.parts[n-1].primitive )
          + temp_timeline.parts[n-1].pause_units * COM_P_TIMELINE_DT ;

    // set it in the transferring timeline
    temp_timeline.parts[n] = temp_part;
    temp_timeline.n_parts_set++;

    return 1;
}

int apply_timeline()
{
	// check that the timeline was fully initialized
	if ( temp_timeline.n_parts != temp_timeline.n_parts_set ) { return 0; }

	// create an extra end_part containing the end of choreography
	struct part end_part;
	int n = temp_timeline.n_parts;
	end_part.primitive  = COM_PCODE_FIN;
	end_part.pause_units = 0;
	if ( n == 0 ) end_part.start_time = 0;
	if ( n  > 0 ) end_part.start_time =
			temp_timeline.parts[n-1].start_time
			+ get_primitive_duration( temp_timeline.parts[n-1].primitive )
			+ temp_timeline.parts[n-1].pause_units * COM_P_TIMELINE_DT ;

	// set it in the finished timeline
	temp_timeline.parts[temp_timeline.n_parts] = end_part;

    main_timeline = temp_timeline;
    return 1;
}

// Cubli functions:
// ---------------

com_pcode get_ongoing_primitive( int current_time )
{
    if ( current_time < 0 ) return COM_PCODE_NONE;

    int i;
    // parts 0 until the end part ( not counted in n_parts hence '<=' )
    for ( i = 0; i <= main_timeline.n_parts; i++ )
    {
        if ( current_time < main_timeline.parts[i].start_time )
            return main_timeline.parts[i-1].primitive;
    }

    return COM_PCODE_FIN;
}

// TODO: function to select the current part, and transform into a primitive
