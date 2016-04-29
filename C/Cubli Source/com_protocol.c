#include "com_protocol.h"


#ifdef COM_VERSION_CPP
// global passing variables
char    _sequence[64];
size_t  _seq_len = 0;
// set com id for this machine
#define COM_SETMACHINEID const com_id this_machine_id = COM_ID_COMPUTER

#else
#include "CubliParameters.h"
#define COM_SETMACHINEID const com_id this_machine_id = _cubliParameters.this_cube_id
#endif

// Private Protocol Parameters
#define HASH_SIZE (2)
#define MESSAGE_CODE_SIZE (1)
#define MAX_PACKET_SIZE (64) // TODO: what if a message is longer?
#define STRT_BYTE ((unsigned char) 0xFF)

// Communications performance log data
int _messagesSentCount;
int _messagesCollectedCount;


unsigned long generate_hash(char *key, size_t len)
{
    //This function generates the hash for a char sequence
    //according to the bitwise XOR bernstein algorithm

    unsigned long hash = 0;

    size_t i;
    for(i = 0; i < len; i++)
    {
        hash = (33 * hash) ^ (unsigned char)*key++;
    }

    return hash;
}

int compare_hash(unsigned long hash1, unsigned long hash2)
{

    
    unsigned long MASK = 0x00000000;
    unsigned long FIRST_BYTE = 0x000000FF;
    size_t i;
    for(i = 0; i < HASH_SIZE; i++) { MASK |= FIRST_BYTE << i*8; }

    return ( (hash1 & MASK) == (hash2 & MASK) );

}

// Public functions:
// ----------------

int send_message(com_mcode message_code, com_id dest_id,
                 char* extra_content, size_t extra_content_len)
{
    // Creates a protocol-confirming sequence from the message code

	COM_SETMACHINEID;

#ifdef COM_DEBUG
    printf("<Debug: in function send_message\r\n");
#endif

    size_t i;
    char message[COM_P_MAX_MESSAGE_SIZE] = {0};
    message[0]=(char)(message_code + COM_CHAROFFSET);
    for ( i = 0; i < extra_content_len; i++)
    {
        message[MESSAGE_CODE_SIZE+i] = extra_content[i];
    }
#ifdef COM_VERBOSE
    // When debugging append verbose message reference
    //
    // adds a 0 before the verbose part - marks end of extra content
    size_t after_0 = MESSAGE_CODE_SIZE + extra_content_len + 1;
    strcat( message + after_0, get_mcode_string(message_code) );
    char offset_extra_content[COM_P_MAX_EXTRA_CONTENT_SIZE+1] = {0};
    for(i = 0; i < extra_content_len; i++)
    {
        offset_extra_content[i] = extra_content[i] + COM_CHAROFFSET;
    }
    strcat(message + after_0,offset_extra_content);
    strcat(message + after_0,"|");
    // skips the zero
    size_t len = strlen(message+after_0)+after_0;
#else
    int len = strlen(message);
#endif
    size_t seq_len           = 1+1+2+len+HASH_SIZE;
    size_t seq_len_excl_hash = 1+1+2+len;
    char sequence[MAX_PACKET_SIZE] = {0};
    unsigned long hash = 0;

    sequence[0] = STRT_BYTE; //Startbyte
    sequence[1] = ((char)len)             + COM_CHAROFFSET; //Message Length
    sequence[2] = ((char)this_machine_id) + COM_CHAROFFSET; //Sender
    sequence[3] = ((char)dest_id)         + COM_CHAROFFSET; //Recipient
    for(i = 0; i < len; i++) { sequence[4+i] = message[i]; } //Message
    hash = generate_hash(sequence,seq_len_excl_hash);
    for(i = 0; i < HASH_SIZE; i++) { 
        sequence[4+len+i] = (hash >> (i*8)) & 0xFF; } //Hash
    
    // Outputs sequence
#ifdef COM_VERSION_CPP
    for(i = 0; i < seq_len; i++) { _sequence[i] = sequence[i]; }
    _seq_len = seq_len;
#else
    for(i = 0; i < seq_len; i++) { printf("%c", sequence[i]);} printf("\r\n");
#endif
    
#ifdef COM_DEBUG
    printf(">\r\n");
#endif

    // Communications Performance Logging
    _messagesSentCount++;

    return 0;
}

#define MAX_LOOPS (3)
size_t decrypt_message(char* buffer, unsigned int max_bytes,
                       com_mcode* message_code, com_id* sender,
                       char* extra_content, size_t *extra_content_len)
{
    // This Function checks if the first max_bytes bytes in a buffer
    // contain a valid message, and unwraps it if the recipient is
    // this cube.

	COM_SETMACHINEID;

#ifdef COM_DEBUG
    printf("<Debug: in function decrypt_message\r\n");
#endif

    // characters read counter
    size_t i = 0;

    // loop in case a good message follows a bad one
    int loop_counter = 0;
    while( i < max_bytes && loop_counter < MAX_LOOPS ) {
    loop_counter++;

    // declarations
    size_t count = 0;
    size_t chars_read = 0;
    char sequence[MAX_PACKET_SIZE] = {0};
    size_t len = 0;
    size_t seq_len = 0;
    size_t seq_len_excl_hash = 0;
    com_id recipient;
    unsigned long hash = 0;
    *message_code = COM_MCODE_NOMESSAGE;

    // ignore anything before a start byte
    while((unsigned char)buffer[i]!=STRT_BYTE && i<max_bytes) { 
        i++; }

    // return if reached the end of buffer
    if ( i == max_bytes ) { break; }

    // read message length
    len         = (size_t) (buffer[i+1] - COM_CHAROFFSET   );
    seq_len           = 1+1+2+len+HASH_SIZE;
    seq_len_excl_hash = 1+1+2+len;
    if ( len > COM_P_MAX_MESSAGE_SIZE ) { continue; }

    // extract sequence according to length
    while( count < seq_len && i < max_bytes) {
        sequence[count] = buffer[i]; i++; count++; }
    chars_read = i;

    // read wrapped data
    *sender     = (com_id) (sequence[2] - COM_CHAROFFSET);
    recipient   = (com_id) (sequence[3] - COM_CHAROFFSET);

    // read message
    char message[COM_P_MAX_MESSAGE_SIZE] = {0};
    for(i = 0; i < len; i++) { message[i] = sequence[4+i]; }
    // read hash
    for(i = 0; i < HASH_SIZE; i++) { 
        hash = ((unsigned char)sequence[4+len+i] << (i*8)) | hash; }

    // verify hash
    unsigned long long hash_result = generate_hash(sequence,seq_len_excl_hash);
    if( !compare_hash(hash,hash_result) ) {
#ifdef COM_DEBUG
        printf("Error in com_protocol decrypt_message: incorrect hash,");
        printf(" is %lu,", hash);
        printf(" should be %lu.\r\n", hash_result);
        printf(">\r\n");
#endif
        continue; }

    //  verify recipient
    if((recipient!=this_machine_id) & (recipient!=COM_ID_ALL)) { 
        continue; }

    // read message code and content
    *message_code = (com_mcode)(message[0] - COM_CHAROFFSET);
    *extra_content_len = len - MESSAGE_CODE_SIZE
#ifdef COM_VERBOSE
                    - ( 1+strlen(get_mcode_string(*message_code))+1 );
    *extra_content_len = *extra_content_len / 2
#endif
                                                ;

    // read extra content
    for(i = 0; i < *extra_content_len; i++)
    {
       extra_content[i] = message[i+MESSAGE_CODE_SIZE];
    }

#ifdef COM_DEBUG
    //Debug: outputs buffer and sequence
    for(i = 0; i < max_bytes; i++) { printf("%c", buffer[i]);} printf("\r\n");
    for(i = 0; i < count; i++) { printf("%c", sequence[i]);} printf("\r\n");
    for(i = 0; i < len; i++) { printf("%c", message[i]);} printf("\r\n");
    printf(">\r\n");
#endif
    
    // Communications Performance Logging
    _messagesCollectedCount++;

    return chars_read;


   } // while loop

    return 0;
}

// String functions:
// ----------------

#ifdef COM_VERSION_CPP
const char * getComIDString(com_id ID)
{
    if ( ID == COM_ID_NOID     ) return "No ID"     ;

    if ( ID == COM_ID_ALL      ) return "All"       ;
    if ( ID == COM_ID_COMPUTER ) return "Computer"  ;

    if ( ID == COM_ID_OLDCUBLI ) return "Old Cubli" ;
    if ( ID == COM_ID_NEWCUBLI ) return "New Cubli" ;
    if ( ID == COM_ID_CUBLI2   ) return "Cubli 2"   ;

    return "INVALID ID";
}
#endif

#ifdef COM_VERBOSE
const char * get_mcode_string(com_mcode code)
{
    if ( code == COM_MCODE_NOTHING           ) return "|"                  ;
    if ( code == COM_MCODE_NOMESSAGE         ) return "|?"                 ;


    if ( code == COM_MCODE_ERROR             ) return "|Error"             ;
    if ( code == COM_MCODE_WAITING           ) return "|Waiting"           ;
    if ( code == COM_MCODE_CHECK             ) return "|Check"             ;
    if ( code == COM_MCODE_RECEIVED          ) return "|Received"          ;
    if ( code == COM_MCODE_RECEIVED_PART     ) return "|Received:"         ;
    if ( code == COM_MCODE_SHUTDOWN          ) return "|Shutting Down"     ;
    if ( code == COM_MCODE_CONNECT           ) return "|Connect"           ;
    if ( code == COM_MCODE_DISCONNECT        ) return "|Disconnect"        ;

    if ( code == COM_MCODE_BGN_SND_TMLN      ) return "|BeginTT:"          ;
    if ( code == COM_MCODE_END_SND_TMLN      ) return "|FinishTT"          ;
    if ( code == COM_MCODE_ABORT_SND_TMLN    ) return "|AbortTT"           ;
    if ( code == COM_MCODE_SND_TMLN_PART     ) return "|TTData:"           ;

    if ( code == COM_MCODE_COUNTDOWN         ) return "|Start in:"         ;
    if ( code == COM_MCODE_GOT_COUNT         ) return "|Got count"         ;
    if ( code == COM_MCODE_STARTED           ) return "|C'est parti!"      ;
    if ( code == COM_MCODE_SUCCEEDED         ) return "|Move successful"   ;
    if ( code == COM_MCODE_FAILED            ) return "|Move failed"       ;
    if ( code == COM_MCODE_ACCUSE            ) return "|Other cube failed" ;
    if ( code == COM_MCODE_STUCK             ) return "|Failed and stuck"  ;
    if ( code == COM_MCODE_ACKNOWLEDGE       ) return "|Acknowledged"      ;
    if ( code == COM_MCODE_ACKNOWLEDGE_REC   ) return "|Ack. Recovery"     ;
    if ( code == COM_MCODE_RECOVERED         ) return "|Recovered"         ;
    if ( code == COM_MCODE_STOP              ) return "|Stop Performance"  ;
    if ( code == COM_MCODE_RESUME            ) return "|Resume in:";
    if ( code == COM_MCODE_FINISHED          ) return "|Ended Performance" ;


    if ( code == COM_BCODE_CHECK_CUBES       ) return "|BCODE"             ;
    if ( code == COM_BCODE_START_IN          ) return "|BCODE"             ;
    if ( code == COM_BCODE_STARTED           ) return "|BCODE"             ;
    if ( code == COM_BCODE_TRANSFER          ) return "|BCODE"             ;
    if ( code == COM_BCODE_ABORT_TRANSFER    ) return "|BCODE"             ;
    if ( code == COM_BCODE_ABORT_PERFORMANCE ) return "|BCODE"             ;
    if ( code == COM_BCODE_RESUME_PERFORMANCE) return "|BCODE"             ;

    if ( code == COM_MCODE_DEBUG             ) return "|DEBUG"             ;

    return "|ILLEGAL_MCODE";
}
#endif

int get_primitive_duration( com_pcode primitive )
{
    // duration of each primitive in seconds
    switch ( primitive )
    {
    case COM_PCODE_NONE   : return  0;
    case COM_PCODE_JUMP2E : return 10;
    case COM_PCODE_JUMP2C : return 10;
    case COM_PCODE_JUMP2G : return  3;
    case COM_PCODE_1SPIN  : return 25; // depends on the surface!

    default : return -1;
    }
}


// vim: set foldmethod=syntax:
