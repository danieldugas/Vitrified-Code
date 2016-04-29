#ifndef COM_PROTOCOL_H
#define COM_PROTOCOL_H

// WARNING: This file is used by both the CUBLI source code and the
// choreography source code (which links to it with a relative path).
// Be aware that modifying/moving it affects the two projects.

#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Debugging flags
#define COM_VERBOSE
// #define COM_DEBUG

// Offset for making message data human-readable when verbose
#ifdef COM_VERBOSE
#define COM_CHAROFFSET ('0')
#else
#define COM_CHAROFFSET (0)
#endif

// Public Com Parameters
#define COM_P_MAX_EXTRA_CONTENT_SIZE (4)
#define COM_P_MAX_MESSAGE_SIZE (64)
#define COM_P_TIMELINE_DT (1) // [seconds]
#define COM_P_TIMELINE_MAX_PARTS (120) // has to fit in a char
#define COM_P_TIMELINE_PART_SIZE (3)
#define COM_P_COUNTDOWN_DELAY (500)
#define COM_P_RETRY_DELAY (500)

// Communications performance logging data
extern int _messagesSentCount;
extern int _messagesCollectedCount;

typedef enum communicationIDs {
    COM_ID_NOID     ,

    COM_ID_ALL      ,
    COM_ID_COMPUTER ,

    COM_ID_OLDCUBLI ,
    COM_ID_NEWCUBLI ,
    COM_ID_CUBLI2   ,

    COM_N_IDS
} com_id;

typedef enum commmunicationstates
{
    // Cubli states &
    // Choreographer states
    COM_STATE_CONNECTING        ,
    COM_STATE_IDLE              ,
    COM_STATE_TRANSFER_TIMELINE ,
    COM_STATE_PING_CUBES        ,
    COM_STATE_DISCONNECT        ,
    COM_STATE_COUNTDOWN         ,
    COM_STATE_PERFORMING        ,

    // deprecated
    COM_STATE_START             ,
    COM_STATE_INITIALIZING      ,
    COM_STATE_CHECKING_IN       ,
    COM_STATE_SAYING            ,
    COM_STATE_TELLING           ,
    COM_STATE_CHECK             ,
    COM_STATE_ANSWERED          ,

    COM_N_STATES
} com_state;

typedef enum messagecodes
{
    COM_MCODE_NOTHING           ,
    COM_MCODE_NOMESSAGE         ,

    // General
    COM_MCODE_ERROR             ,
    COM_MCODE_WAITING           ,
    COM_MCODE_CHECK             ,
    COM_MCODE_RECEIVED          ,
    COM_MCODE_RECEIVED_PART     ,
    COM_MCODE_SHUTDOWN          ,
    COM_MCODE_CONNECT           ,
    COM_MCODE_DISCONNECT        ,
    // Timeline transfer
    COM_MCODE_BGN_SND_TMLN      ,
    COM_MCODE_END_SND_TMLN      ,
    COM_MCODE_ABORT_SND_TMLN    ,
    COM_MCODE_SND_TMLN_PART     ,
    // Choreography
    COM_MCODE_COUNTDOWN         ,
    COM_MCODE_GOT_COUNT         ,
    COM_MCODE_STARTED           ,
    COM_MCODE_SUCCEEDED         ,
    COM_MCODE_FAILED            ,
    COM_MCODE_ACCUSE            ,
    COM_MCODE_STUCK             ,
    COM_MCODE_ACKNOWLEDGE       ,
    COM_MCODE_ACKNOWLEDGE_REC   ,
    COM_MCODE_RECOVERED	        ,
    COM_MCODE_STOP              ,
    COM_MCODE_RESUME            ,
    COM_MCODE_FINISHED          ,

    // Button codes
    COM_BCODE_CHECK_CUBES       ,
    COM_BCODE_START_IN          ,
    COM_BCODE_STARTED           ,
    COM_BCODE_TRANSFER          ,
    COM_BCODE_ABORT_TRANSFER    ,
    COM_BCODE_ABORT_PERFORMANCE ,
    COM_BCODE_RESUME_PERFORMANCE,

    COM_MCODE_DEBUG             ,

    COM_N_MESSAGE_CODES // keep last: defines the size
} com_mcode;

typedef enum primitivecodes
{
    COM_PCODE_NONE   ,

    COM_PCODE_JUMP2E ,
	COM_PCODE_JUMP2C ,
    COM_PCODE_JUMP2G ,
	COM_PCODE_1SPIN  ,

    COM_PCODE_FIN    ,

    COM_N_PCODES
} com_pcode;

int    send_message(    com_mcode message_code, com_id dest_id,
                        char* extra_content, size_t extra_content_len);
size_t decrypt_message( char* buffer, unsigned int max_bytes,
                        com_mcode* message_code, com_id* sender,
                        char* extra_content, size_t* extra_content_len);

#ifdef COM_VERBOSE
const char * get_mcode_string(com_mcode code);
#endif

#ifdef COM_VERSION_CPP
const char * getComIDString(com_id ID);
#endif

int get_primitive_duration( com_pcode primitive );


#endif // COM_PROTOCOL_H
