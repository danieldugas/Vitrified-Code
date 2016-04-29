#include "comstatemanager.h"

// Description:
//
// This class handles what to send in response to
// events such as button presses, or messages received.
// The bulk of the logic can be found in the update() function,
// (which could also have been named respond() based on its behavior.)


ComStateManager::ComStateManager()
{
    parent = NULL;
    printf("WARNING: Initialized ComStateManager without parent");

    // COPYPASTED FROM CONSTRUCTOR BELOW
    state=COM_STATE_IDLE;
    messageStatus = NOSENT;
    resetStateVariables();
    AGAIN = 0;
    for ( auto &i : cubeStatuses ) { i = DISCONNECTED; }
}

ComStateManager::ComStateManager(MainWindow* parentWindow) :
    parent(parentWindow)
{
    state=COM_STATE_IDLE;
    messageStatus = NOSENT;
    resetStateVariables();
    AGAIN = 0;
    for ( auto &i : cubeStatuses ) { i = DISCONNECTED; }
}

ComStateManager::~ComStateManager()
{

}

#define BREAK(X) X; break
void ComStateManager::update(com_mcode message_code,
        std::vector<char> * extra_content, com_id sender)
{
    // Directives for the communication state-machine:
    //  - each time update() is called, at most 1 message gets sent.
    //  - when possible, conversations are bilateral (one cube at a time.)
    //      ( does not apply to announcements, e.g. "starting" ? )
    //  - states can only modify state-general and state-exclusive variables.
    //  - state-exclusive variables can only be modified from within
    //      their respective state.
    //  - state-exclusive and state-general variables are reset to their
    //      default values whenever the state changes.


    // Loop the state machine until something gets sent
    AGAIN=1;
    while ( AGAIN )
    {
    if ( messageStatus == SENT   ) messageStatus = WAIT;
    if ( messageStatus == NOSENT ) messageStatus = NOWAIT;
    AGAIN=0;

    // state-governed actions
    switch (state)
    {

    case COM_STATE_IDLE :
        {
        switch (message_code)
        {

        case COM_BCODE_CHECK_CUBES :
            BREAK( SEND( COM_MCODE_CHECK, COM_ID_ALL, NULL, 0 ) );

        case COM_MCODE_CONNECT :
            // is it the first time this cube is heard?
            if ( cubeStatus(sender) == DISCONNECTED  )
            {
                setCubeStatus( sender, CONNECTING );
                BREAK(JUMP_STATE( COM_STATE_CONNECTING ));
            }
            BREAK( SEND( COM_MCODE_CONNECT, sender, NULL, 0 ) );

        case COM_MCODE_RECEIVED :
            if ( cubeStatus(sender) == CONNECTING )
            {
                BREAK(JUMP_STATE( COM_STATE_CONNECTING ));
            }
            BREAK( SEND_NOTHING() );

        case COM_BCODE_TRANSFER :
            BREAK( JUMP_STATE( COM_STATE_TRANSFER_TIMELINE ) );

        case COM_MCODE_WAITING :
            BREAK( SEND( COM_MCODE_ABORT_SND_TMLN, sender, NULL, 0 ) );

        case COM_BCODE_ABORT_TRANSFER :
            BREAK( SEND( COM_MCODE_ABORT_SND_TMLN,
                    COM_ID_ALL, NULL, 0 ) );

        case COM_BCODE_START_IN :
            BREAK( JUMP_STATE( COM_STATE_COUNTDOWN ) );

        case COM_MCODE_FAILED :
        case COM_BCODE_ABORT_PERFORMANCE :
            BREAK( SEND( COM_MCODE_STOP, COM_ID_ALL, NULL, 0) );

        case COM_MCODE_STOP :
            setCubeStatus( sender, SYNCED );
            parent->abortPerformanceFromComStateManager = true;
            BREAK( SEND_NOTHING() );

        case COM_MCODE_SHUTDOWN :
            setCubeStatus( sender, DISCONNECTED );
            parent->showSerialData(">> # of messages sent: " +
                                   QString::number(_messagesSentCount) + "\r\n");
            parent->showSerialData(">> # of messages collected: " +
                                   QString::number(_messagesCollectedCount) + "\r\n");
            BREAK( SEND_NOTHING() );


        default :
            BREAK( SEND_NOTHING() );

        }
        }
        break;

    case COM_STATE_CONNECTING :
        {
        switch (message_code)
        {

        case COM_BCODE_CHECK_CUBES :
            // useful is choreographer is stuck in COM_STATE_CONNECTING
            BREAK( JUMP_STATE( COM_STATE_IDLE ) );

        case COM_MCODE_CONNECT :
            if ( cubeStatus(sender) == DISCONNECTED ) { setCubeStatus( sender, CONNECTING ); }
            if ( cubeStatus(sender) != CONNECTING ) { BREAK( IGNORE() ); }
            BREAK( SEND( COM_MCODE_CONNECT , sender , NULL, 0) );

        case COM_MCODE_RECEIVED :
            if ( cubeStatus(sender) != CONNECTING ) { BREAK( IGNORE() ); }
            setCubeStatus( sender, NOTIMELINE );
            if ( cubeStatusesXcount( CONNECTING ) ) { BREAK( SEND_NOTHING() ); }
            BREAK( NEXT_STATE( COM_STATE_IDLE ) );

        default :
            BREAK( IGNORE() );


        }
        }
        break;

    case COM_STATE_TRANSFER_TIMELINE :
        {
        // Sending a timeline is a multi-step process
        // each step is completed upon confirmation
        // by the recipient(s)
        if ( currentCube != sender && COM_ID_COMPUTER != sender ) { BREAK( IGNORE() ); }
        switch (message_code)
        {

        case COM_BCODE_ABORT_TRANSFER :
            BREAK( JUMP_STATE( COM_STATE_IDLE ) );

        case COM_MCODE_RECEIVED :       // Same as below
        case COM_MCODE_RECEIVED_PART :  // Advance to next step
            // TODO: add hash to extra content for robustness?
            if ( messageStatus == WAIT ) { punchLastMessage(); step++; }
            else { BREAK( IGNORE() ); }
        case COM_BCODE_TRANSFER :  // Same as below
        case COM_MCODE_WAITING :   // Execute Current Step
            switch (step)
            {                   // Transfer Steps:
            case MAX_STEPS+1 :  // Transfer is over
                step = -1; setCubeStatus( currentCube, SYNCED ) ;
            case  -1 :           // Initialize transfer
                if ( !selectFirstSyncingCube() )
                {
                    BREAK( NEXT_STATE( COM_STATE_IDLE ) );
                }
                nParts = parent->getNTimelineParts(currentCube);
                BREAK( SEND( COM_MCODE_BGN_SND_TMLN,
                             currentCube , &nParts, 1 ) );

            default : // Transfer parts 1 to N
                if ( message_code == COM_MCODE_RECEIVED && step > 0 ) { step--; }
                if ( message_code == COM_MCODE_RECEIVED_PART && step > 0 )
                {
                    if ( !extra_content ) { step--; BREAK( IGNORE() ); }
                    if ( extra_content->at(0) < step-1 ) { step--; }
                }
                if ( step < nParts )
                {
                    parent->showAppInfo(
                                QString(getComIDString( currentCube ))
                                + ": "
                                + QString::number( 100 * step/nParts )
                                + "% Complete");
                    BREAK( SEND( COM_MCODE_SND_TMLN_PART,
                                 currentCube,
                                 parent->getTimelinePart( step,
                                                    currentCube ),
                                 COM_P_TIMELINE_PART_SIZE ) );
                }
                step = MAX_STEPS;
            case MAX_STEPS :   // Conclude transfer
                BREAK( SEND( COM_MCODE_END_SND_TMLN,
                        currentCube, NULL, 0 ) );

            }
            break;

        default :                  // Other actions
        // TODO:
        // for example, if the currentCube has not given a WAITING
        // code in a while but other messages have been received
        // assume it is disconnected
            BREAK( IGNORE() );

        }
        }
        break;

    case COM_STATE_COUNTDOWN :
        {
        switch ( message_code )
        {
        case COM_BCODE_START_IN :
            BREAK( SEND( COM_MCODE_COUNTDOWN,
                         COM_ID_ALL,
                         &(extra_content->at(0)),
                         1 ));

        case COM_BCODE_TRANSFER :
            BREAK( JUMP_STATE( COM_STATE_IDLE ) );

        case COM_MCODE_GOT_COUNT :
            setCubeStatus( sender, COUNTING );
            BREAK( SEND_NOTHING() );

        case COM_BCODE_STARTED :
            cubeStatusesXtoY( COUNTING, /* -> */ PERFORMING );
            BREAK( JUMP_STATE( COM_STATE_PERFORMING ) );

        case COM_MCODE_FAILED :
        case COM_BCODE_ABORT_PERFORMANCE :
            BREAK( JUMP_STATE( COM_STATE_IDLE ) );

        default :
            BREAK( IGNORE() );
        }
        }
        break;

    case COM_STATE_PERFORMING :
        {
        switch ( message_code )
        {
        case COM_BCODE_STARTED : // coming from countdown state
            //BREAK( SEND( COM_MCODE_STARTED, COM_ID_ALL, NULL, 0 ) );
            BREAK( SEND_NOTHING() );

        case COM_MCODE_FAILED :
            waitingForCubeToRecover[sender] = true;
            parent->showChoreographyFailure();
            BREAK( SEND( COM_MCODE_ACKNOWLEDGE, sender, NULL, 0 ) );

        case COM_MCODE_ACCUSE :
            waitingForCubeToResume[sender] = true;
            parent->showChoreographyFailure();
            BREAK( SEND( COM_MCODE_ACKNOWLEDGE, sender, NULL, 0 ) );

        case COM_MCODE_RECOVERED :
            waitingForCubeToRecover[sender] = false;
            waitingForCubeToResume[sender] = true;
            BREAK( SEND( COM_MCODE_ACKNOWLEDGE_REC, sender, NULL, 0 ) );

        case COM_MCODE_RESUME :
            waitingForCubeToResume[sender] = false;
            BREAK( SEND_NOTHING() );

        case COM_BCODE_RESUME_PERFORMANCE :
            BREAK( SEND( COM_MCODE_RESUME,
                         COM_ID_ALL,
                         &(extra_content->at(0)),
                         1 ));

        case COM_MCODE_SHUTDOWN :
            parent->abortPerformanceFromComStateManager = true;
        case COM_MCODE_STOP :
        case COM_BCODE_ABORT_PERFORMANCE :
            BREAK( JUMP_STATE( COM_STATE_IDLE ) );

        case COM_MCODE_FINISHED :
            setCubeStatus( sender, SYNCED );
            if ( cubeStatusesXcount( PERFORMING ) ) { BREAK( SEND_NOTHING() ); }
            BREAK( JUMP_STATE( COM_STATE_IDLE ) );

        default :
            BREAK( IGNORE() );
        }
        }
        break; // COM_STATE_COUNTDOWN

    default :       // Illegal or undefined state
        BREAK( NEXT_STATE( COM_STATE_IDLE ) );

    }
    parent->updateCubeStatuses();
#ifdef CHOREOGRAPHER_DEBUG
    parent->showComState( QString::number(state) );
    } // end of while loop
    if (messageStatus != SENT && messageStatus != NOSENT)
    {
        parent->showSerialData(QString(">>EXCEPTION: At end of update(), messageStatus is not finalized. \r\n"));
    }
#else
    } // end of while loop
#endif

}

com_id ComStateManager::getFirstConnectedCube()
{
    // TODO: include Performing and counting cubes
    return getFirstCubeOfStatusX( NOTIMELINE, UNSYNCED, SYNCED, SYNCING );
}

com_id ComStateManager::getFirstNoTimelineCube()
{
    return getFirstCubeOfStatusX( NOTIMELINE );
}

com_id ComStateManager::getNextConnectedCube( int starting_after )
{
    return getNextCubeOfStatusX( NOTIMELINE, UNSYNCED, SYNCED, SYNCING, starting_after );
}

com_id ComStateManager::getNextPerformingCube( int starting_after )
{
    return getNextCubeOfStatusX( PERFORMING, starting_after );
}

int ComStateManager::nCubesToSync()
{
    return cubeStatusesXtoY( UNSYNCED, SYNCING, NOT_A_CUBE_STATUS );
}

int ComStateManager::nCubesSynced()
{
    return cubeStatusesXcount( SYNCED );
}

int ComStateManager::nCubesUnsynced()
{
    return cubeStatusesXcount( UNSYNCED );
}

int ComStateManager::nCubesPerformingOrCounting()
{
    return cubeStatusesXtoY( PERFORMING, COUNTING, NOT_A_CUBE_STATUS );
}

int ComStateManager::setPerformingOrCountingCubesToDisconnected()
{
    return cubeStatusesXtoY( PERFORMING, COUNTING, /* -> */ DISCONNECTED );
}

int ComStateManager::setSyncedCubesToDisconnected()
{
    return cubeStatusesXtoY( SYNCED, /* -> */ DISCONNECTED );
}

int ComStateManager::setCubesToSyncing()
{
    return cubeStatusesXtoY( UNSYNCED, SYNCED, /* -> */ SYNCING);
}

int ComStateManager::setCubesToUnsynced()
{
    return cubeStatusesXtoY( SYNCING, SYNCED, /* -> */ UNSYNCED );
}

int ComStateManager::setCubeStatus(com_id cubeID, cube_status status)
{
    cubeStatuses[cubeID] = status;
    return 1;
}

cube_status ComStateManager::cubeStatus(com_id cubeID)
{
    return cubeStatuses[cubeID];
}

QString ComStateManager::cubeStatusAsString(com_id cubeID)
{
    return cubeStatusToString( cubeStatus( cubeID ) );
}

int ComStateManager::nCubesWaitingToRecover()
{
    int n = 0;
    for ( auto i : waitingForCubeToRecover ) { if ( i ) { n++; } }

    return n;
}

int ComStateManager::nCubesWaitingToResume()
{
    int n = 0;
    for ( auto i : waitingForCubeToResume ) { if ( i ) { n++; } }

    return n;
}

// Private methods:
// ---------------


com_id ComStateManager::getFirstCubeOfStatusX(cube_status X)
{
    return getFirstCubeOfStatusX(X, NOT_A_CUBE_STATUS);
}

com_id ComStateManager::getFirstCubeOfStatusX(cube_status X1, cube_status X2)
{
    return getFirstCubeOfStatusX(X1, X2, NOT_A_CUBE_STATUS);
}

com_id ComStateManager::getFirstCubeOfStatusX(cube_status X1, cube_status X2, cube_status X3)
{
    return getFirstCubeOfStatusX(X1, X2, X3, NOT_A_CUBE_STATUS);
}

com_id ComStateManager::getFirstCubeOfStatusX(cube_status X1, cube_status X2, cube_status X3, cube_status X4)
{
    return getNextCubeOfStatusX(X1, X2, X3, X4, -1);
}

com_id ComStateManager::getNextCubeOfStatusX(cube_status X, int starting_after)
{
    return getNextCubeOfStatusX(X, NOT_A_CUBE_STATUS, starting_after);
}

com_id ComStateManager::getNextCubeOfStatusX(cube_status X1, cube_status X2, int starting_after)
{
    return getNextCubeOfStatusX(X1, X2, NOT_A_CUBE_STATUS, starting_after);
}

com_id ComStateManager::getNextCubeOfStatusX(cube_status X1, cube_status X2, cube_status X3, int starting_after)
{
    return getNextCubeOfStatusX(X1, X2, X3, NOT_A_CUBE_STATUS, starting_after);
}

com_id ComStateManager::getNextCubeOfStatusX(cube_status X1, cube_status X2, cube_status X3, cube_status X4, int starting_after)
{
    int index = 0;
    for ( auto i : cubeStatuses )
    {
        if ( index > starting_after )
        {
            if ( ( X1 == i ) || ( X2 == i ) || ( X3 == i ) || ( X4 == i ) )
            {
                return static_cast<com_id>( index );
            }
        }

        index ++;
    }
    return COM_ID_NOID;

}

int ComStateManager::cubeStatusesXcount(cube_status X)
{
    return cubeStatusesXtoY(X, NOT_A_CUBE_STATUS);
}

int ComStateManager::cubeStatusesXtoY(cube_status X, cube_status Y)
{
    return cubeStatusesXtoY(X, NOT_A_CUBE_STATUS, Y);
}

int ComStateManager::cubeStatusesXtoY(cube_status X1, cube_status X2, cube_status Y)
{
    int count = 0;

    if ( ( Y == ANY_CUBE_STATUS ) || ( Y == N_CUBE_STATUSES ) )
    { return -1; }

    for ( auto &i : cubeStatuses )
    {
        if ( ( i == X1 ) || ( i == X2) || ( X1 == ANY_CUBE_STATUS ) )
        {
            count++;
            if ( Y != NOT_A_CUBE_STATUS )
            {
                i = Y;
            }
        }
    }

    return count;
}

QString ComStateManager::cubeStatusToString( cube_status X )
{
    if ( X == NOT_A_CUBE_STATUS ) return "NOT_A_CUBE_STATUS"  ;
    if ( X == ANY_CUBE_STATUS   ) return "ANY_CUBE_STATUS  "  ;


    if ( X == DISCONNECTED      ) return "DISCONNECTED     "  ;


    if ( X == CONNECTING        ) return "CONNECTING       "  ;
    if ( X == DISCONNECTING     ) return "DISCONNECTING    "  ;


    if ( X == NOTIMELINE        ) return "NOTIMELINE       "  ;
    if ( X == UNSYNCED          ) return "UNSYNCED         "  ;
    if ( X == SYNCING           ) return "SYNCING          "  ;
    if ( X == SYNCED            ) return "SYNCED           "  ;


    if ( X == COUNTING          ) return "COUNTING         "  ;
    if ( X == PERFORMING        ) return "PERFORMING       "  ;

    return "ILLEGAL STATUS";
}

int ComStateManager::selectFirstSyncingCube()
{
    currentCube = getFirstCubeOfStatusX( SYNCING );
    return static_cast<int>(currentCube);
}

void ComStateManager::punchLastMessage()
{
    // 'punches' the last message, marking it as already handled
    messageStatus = RCVD;
}

void ComStateManager::resetStateVariables()
{
    // Here are the default values for state-exclusive variables.

    //   in COM_STATE_TRANSFER_TIMELINE
    step = -1;
    currentCube = COM_ID_NOID;
    nParts = 0;
    //   in COM_STATE_PERFORMING
    for ( auto &i : waitingForCubeToRecover   ) { i = false       ; }
    for ( auto &i : waitingForCubeToResume    ) { i = false       ; }
    attemptingRecovery = false;
    //   in COM_STATE_XXX
    // ...

}


// Switch-terminating functions:
// ----------------------------

void ComStateManager::NEXT_STATE(com_state nextStateId)
{
    resetStateVariables();
    state = nextStateId;
    SEND_NOTHING();
}

void ComStateManager::JUMP_STATE(com_state nextStateId)
{
    // same as NEXT_STATE but with no wait
    AGAIN=1;
    NEXT_STATE( nextStateId );
}

void ComStateManager::SEND(com_mcode message_code, com_id recipient, char * extra_content, size_t extra_content_len)
{
    messageStatus = SENT;
    parent->sendMessage( message_code , recipient , extra_content , extra_content_len );
}

void ComStateManager::SEND_NOTHING()
{
    messageStatus = NOSENT;
}

void ComStateManager::IGNORE()
{
    if ( messageStatus ==   RCVD ) printf("Error in IGNORE() from state %i", state);
    if ( messageStatus == NOWAIT ) messageStatus = NOSENT;
    if ( messageStatus ==   WAIT ) messageStatus =   SENT;
}

// vim: set foldmethod=syntax:
