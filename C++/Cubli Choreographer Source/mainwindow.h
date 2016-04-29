#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QTime>
#include <vector>
#include <array>

#include "timeline.h"
#include "serialmanager.h"
#include "settingsdialog.h"
#include "com_protocol.h"
#include "comstatemanager.h"
#include "primitive.h"

#define CHOREOGRAPHER_DEBUG


// Circular dependencies
class SerialManager;
class Timeline;
class ComStateManager;
class Primitive;

namespace Ui {
class MainWindow;
}

typedef enum appstatuses {
    NOT_AN_APP_STATUS   ,

    NORMAL              ,

    BUSYCHECKING        ,
    BUSYSYNCING         ,
    BUSYCOUNTING        ,
    BUSYPERFORMING      ,
    BUSYABORTING        ,

    N_APP_STATUSES
} app_status;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();
    void    showAppInfo( const QString& text );
    void    showComState( const QString& stateString );
    void    updateCubeStatuses();
    void    updateTimelines();
    void    showSerialData( const QString& text );
    void    showPrimitiveInfo( Primitive * example = NULL );
    void    sendMessage(com_mcode message_code, com_id recipient , char * extra_content, size_t extra_content_len);
    char    getNTimelineParts( com_id cubeID );
    char *  getTimelinePart(int n, com_id cubeID );
    void    showChoreographyFailure();
    void    showChoreographyRecovery();

    bool    abortPerformanceFromComStateManager;

private slots:
    void plusButtonAction();
    void showSettingsDialog();
    void readData();
    void applyButtonAction();
    void playButtonAction();
#ifdef CHOREOGRAPHER_DEBUG
    void sendData();
    void DEBUG_C2B1();
    void DEBUG_C2B2();
    void DEBUG_C2B3();
    void DEBUG_C4B1();
    void DEBUG_C4B2();
    void DEBUG_C4B3();
    void DEBUG_C4B4();
    void DEBUG_C4B5();
    void DEBUG_C5B1();
    void DEBUG_C5B2();
    void DEBUG_C5B3();
    void DEBUG_C5B4();
    void DEBUG_C5B5();
    void DEBUG_EMULATE_INCOMING_MESSAGE(com_mcode message_code, com_id sender, com_id recipient);
#endif

private:
    Ui::MainWindow * ui;
    std::array<Timeline*, COM_N_IDS> timelines;
    std::array<QLabel*, COM_N_IDS> timelineLabels;
    SerialManager * serialmanager;
    ComStateManager * comstatemanager;
    app_status currentappstatus;
    Primitive * primitiveOnDisplay;

    QTime choreographyTime;
    int   accumulatedMilliseconds; // keeps track of time elapsed before timer was reset
    bool  inChoreographyFailure;

    int  getChoreographyLengthInSeconds();

    void makeTimeline(com_id cube_id);
    void showTimeline(com_id cube_id);
    void hideTimeline(com_id cube_id);
    void showLabel(QLabel * label);
    void hideLabel(QLabel * label);
    void showGraphicsView( QGraphicsView *view );
    void hideGraphicsView( QGraphicsView *view );

    void updateAndSyncTimelines();
    void abortSync();

    void startPerformance();
    void duringPerformance();
    void abortPerformance();

    void performanceResumingSequence();

    void busyChecking();
    void busySyncing();
    void busyCounting(char count);
    void busyPerforming();
    void busyAborting();
    void notBusy();

    // Called when the user closes the window
    void closeEvent (QCloseEvent *event);
};

#endif // MAINWINDOW_H
