#ifndef COMSTATEMANAGER_H
#define COMSTATEMANAGER_H

#include "com_protocol.h"
#include "mainwindow.h"
#include "timeline.h"

#define MAX_STEPS (256)

typedef enum messagestatuses {
      WAIT,
    NOWAIT,

      RCVD,

      SENT,
    NOSENT,
} msg_status;

typedef enum cubestatuses {
    NOT_A_CUBE_STATUS   ,
    ANY_CUBE_STATUS     ,

    // disconnected
    DISCONNECTED        ,

    // transition
    CONNECTING          ,
    DISCONNECTING       ,

    // connected
    NOTIMELINE          ,
    UNSYNCED            ,
    SYNCING             ,
    SYNCED              ,

    // choreography
    COUNTING            ,
    PERFORMING          ,

    N_CUBE_STATUSES
} cube_status;


class ComStateManager
{
public:
    ComStateManager();
    ComStateManager(MainWindow *parentWindow);
    ~ComStateManager();
//     void setConnectedCubes();
//     void setSyncedCubes();
    void update(com_mcode message_code, std::vector<char> * extra_content, com_id sender);

    com_id getFirstConnectedCube();
    com_id getFirstNoTimelineCube();
    com_id getNextConnectedCube(  int starting_after );
    com_id getNextPerformingCube( int starting_after );
    int nCubesToSync();
    int nCubesSynced();
    int nCubesUnsynced();
    int nCubesPerformingOrCounting();
    int setPerformingOrCountingCubesToDisconnected();
    int setSyncedCubesToDisconnected();
    int setCubesToSyncing();
    int setCubesToUnsynced();
    int setCubeStatus(com_id cubeID, cube_status status);
    cube_status cubeStatus(com_id cubeID);
    QString cubeStatusAsString( com_id cubeID );

    int  nCubesWaitingToRecover();
    int  nCubesWaitingToResume();

private:
    MainWindow *parent;
    com_state state;
    std::array<cube_status, COM_N_IDS> cubeStatuses;
    msg_status messageStatus;
    int AGAIN;
    // state-exclusive
    //   in COM_STATE_TRANSFER_TIMELINE
    int step;
    com_id currentCube;
    char nParts;
    //   in COM_STATE_PERFORMING
    std::array<bool, COM_N_IDS> waitingForCubeToRecover;
    std::array<bool, COM_N_IDS> waitingForCubeToResume ;
    bool attemptingRecovery;

    // functions
    com_id getFirstCubeOfStatusX(cube_status X);
    com_id getFirstCubeOfStatusX(cube_status X1, cube_status X2);
    com_id getFirstCubeOfStatusX(cube_status X1, cube_status X2, cube_status X3);
    com_id getFirstCubeOfStatusX(cube_status X1, cube_status X2, cube_status X3, cube_status X4);
    com_id getNextCubeOfStatusX(cube_status X, int starting_after);
    com_id getNextCubeOfStatusX(cube_status X1, cube_status X2, int starting_after);
    com_id getNextCubeOfStatusX(cube_status X1, cube_status X2, cube_status X3, int starting_after);
    com_id getNextCubeOfStatusX(cube_status X1, cube_status X2, cube_status X3, cube_status X4, int starting_after);
    int cubeStatusesXcount(cube_status X);
    int cubeStatusesXtoY(cube_status X, cube_status Y);
    int cubeStatusesXtoY(cube_status X1, cube_status X2, cube_status Y);
    QString cubeStatusToString( cube_status X );
    int  selectFirstSyncingCube();
    void punchLastMessage();
    void resetStateVariables();

    // Switch-terminating functions
    //   ( Must only be called through the macro BREAK() )
    void NEXT_STATE(com_state nextStateId);
    void JUMP_STATE(com_state nextStateId);
    void SEND(com_mcode message_code, com_id recipient , char *extra_content, size_t extra_content_len);
    void SEND_NOTHING();
    void IGNORE();

};

#endif // COMSTATEMANAGER_H
