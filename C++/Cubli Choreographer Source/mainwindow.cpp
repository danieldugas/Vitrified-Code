#include "mainwindow.h"
#include "ui_mainwindow.h"


// Active Delay function
void delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

// Dirty way of getting com_protocol.c data (Don't judge)
extern char   _sequence[];
extern size_t _seq_len;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("CUBLI Choreographer V1");
    // instantiate objects
    serialmanager = new SerialManager(this);
    comstatemanager = new ComStateManager(this);
    ui->rendererGraphicsView->setScene( new QGraphicsScene() );
    primitiveOnDisplay = new Primitive( QRect( 0, 0, 1, PRIMITIVE_HEIGHT ) , NULL );
    ui->rendererGraphicsView->scene()->addItem( primitiveOnDisplay );

    // Sinks and slots
    connect(serialmanager->serial,  SIGNAL(readyRead()),this,   SLOT(readData())             );
    connect(ui->plusButton,         SIGNAL(clicked()),  this,   SLOT(plusButtonAction())     );
    connect(ui->serialSettings,     SIGNAL(triggered()),this,   SLOT(showSettingsDialog())   );
    connect(ui->applyButton,        SIGNAL(clicked()),  this,   SLOT(applyButtonAction())    );
    connect(ui->playButton,         SIGNAL(clicked()),  this,   SLOT(playButtonAction())     );
#ifdef CHOREOGRAPHER_DEBUG
    connect(ui->sendButton,         SIGNAL(clicked()),  this,   SLOT(sendData())             );
    connect(ui->C2B1,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C2B1())           );
    connect(ui->C2B2,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C2B2())           );
    connect(ui->C2B3,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C2B3())           );
    connect(ui->C4B1,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C4B1())           );
    connect(ui->C4B2,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C4B2())           );
    connect(ui->C4B3,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C4B3())           );
    connect(ui->C4B4,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C4B4())           );
    connect(ui->C4B5,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C4B5())           );
    connect(ui->C5B1,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C5B1())           );
    connect(ui->C5B2,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C5B2())           );
    connect(ui->C5B3,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C5B3())           );
    connect(ui->C5B4,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C5B4())           );
    connect(ui->C5B5,               SIGNAL(clicked()),  this,   SLOT(DEBUG_C5B5())           );
#else
    ui->DEBUG->hide();
#endif
    //deal with widget sizes
    ui->rendererGraphicsView->setFixedSize(200,200);

    // initialize members
    for (auto &i : timelines      ) { i = NULL; }
    for (auto &i : timelineLabels ) { i = NULL; }
    updateCubeStatuses();
    showPrimitiveInfo();
    showComState( "1" );

    this->show();
    showSettingsDialog();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::showAppInfo(const QString& text)
{
    ui->appInfoLabel->setText(text);
}

void MainWindow::showComState(const QString& stateString )
{
#ifdef CHOREOGRAPHER_DEBUG
    ui->StateLabel->setText( "State: " + stateString );
#endif
}


void MainWindow::updateCubeStatuses()
{
    updateTimelines();
    // Manual labels
    ui->NewcubliLabel->setText("NewCubli - "+comstatemanager->cubeStatusAsString( COM_ID_NEWCUBLI ) );
    ui->Cubli2Label->setText(  "Cubli 2  - "+comstatemanager->cubeStatusAsString( COM_ID_CUBLI2   ) );
    // Update timeline labels
    int i = 0;
    for( auto &label : timelineLabels )
    {
        if ( label )
        {
            com_id cubeID = static_cast<com_id>(i) ;
            label->setText( QString( getComIDString( cubeID ) ) + " - " + comstatemanager->cubeStatusAsString( cubeID ) );
        }
        i++;
    }
}

void MainWindow::updateTimelines()
{
    com_id cubeID = comstatemanager->getFirstNoTimelineCube();
    if ( !cubeID ) { return; }

    // if the cube doesn't already have a timeline make it one
    if ( !timelines[cubeID] )
    {
        comstatemanager->setCubeStatus(cubeID, UNSYNCED);
        makeTimeline(cubeID);
        showAppInfo("Added Timeline(s)");
        return;
    }

    // if it does then it is wrongly marked as NOTIMELINE : this sets it right (if is redundant)
    if ( timelines[cubeID] && comstatemanager->cubeStatus( cubeID ) == NOTIMELINE )
    {
        comstatemanager->setCubeStatus( cubeID, UNSYNCED );
    }
}

void MainWindow::showSerialData(const QString& text)
{
    ui->serialData->moveCursor (QTextCursor::End);
    ui->serialData->insertPlainText(text);
    ui->serialData->verticalScrollBar()->setValue(ui->serialData->verticalScrollBar()->maximum()); // Scrolls to the bottom

}

void MainWindow::showPrimitiveInfo(Primitive * example )
{
    com_pcode primitiveType;

    if ( example )
    {
        primitiveType = example->getType();
    }
    else
    {
        primitiveType = COM_PCODE_NONE;
    }

    // Update primitive on display
    primitiveOnDisplay->setType( primitiveType );
    primitiveOnDisplay->setCenter( 0 , /* snapAndCheck = */ false);

    // Update text
    switch( primitiveType )
    {
                                   //1        10        20        30        40        50        60        70        80 <<
    case COM_PCODE_NONE :
        primitiveOnDisplay->setPen(QPen(Qt::black));
        primitiveOnDisplay->setRect(-25, 0, 50, PRIMITIVE_HEIGHT);
        ui->primitiveTitle->setText("No Primitive Selected");
        ui->primitiveDescription->setText("  When you select a primitive, its description will appear here.\r\n"
                                          "\r\n"
                                          "  Primitives are actions that CUBLI can perform during a choreography.\r\n"
                                          "  They can be created in a Timeline, press the '+' button below to\r\n"
                                          "  connect with cubes and start editing their timelines.\r\n"
                                          "\r\n"
                                          "  General Tip: Make sure to train CUBLI and charge its batteries beforehand\r\n"
                                          "  for smoother choreographies.");
        break;
    case COM_PCODE_JUMP2G :
        ui->primitiveTitle->setText("Jump to Ground");
        ui->primitiveDescription->setText("  Whether balancing on an edge or a corner, CUBLI will jump down\r\n"
                                          "  to its favorite face.\r\n"
                                          "\r\n"
                                          "  In the timeline, click once on any primitive to transform it into another.");
        break;
    case COM_PCODE_JUMP2E :
        ui->primitiveTitle->setText("Jump to Edge");
        ui->primitiveDescription->setText("  CUBLI will jump from the ground to its edge. If it overshoots,\r\n"
                                          "  you can help it back up");
        break;
    case COM_PCODE_JUMP2C :
        ui->primitiveTitle->setText("Jump to Corner");
        ui->primitiveDescription->setText("  Whether from an edge or the ground, CUBLI will jump to its corner.\r\n"
                                          "\r\n"
                                          "  Note: this move can be rather difficult. Test in advance!");
        break;
    case COM_PCODE_1SPIN :
        ui->primitiveTitle->setText("Spin and Stop");
        ui->primitiveDescription->setText("  CUBLI starts spinning on its corner, waits a little, and stops.\r\n");
        break;

    default :
        ui->primitiveTitle->setText("N/A");
        ui->primitiveDescription->setText("Invalid primitive type");
        break;
    }

}

void MainWindow::sendMessage( com_mcode message_code, com_id recipient, char * extra_content, size_t extra_content_len )
{
    //creates the wrapped sequence, converts it to QByteArray and writes it to port
    send_message( message_code , recipient , extra_content , extra_content_len );
    QByteArray messagedata = QByteArray(_sequence,_seq_len);
    this->showSerialData(">>");
    for(size_t i = 0; i < _seq_len; i++) { this->showSerialData(QString(_sequence[i])); }
    this->showSerialData("\r\n");
    serialmanager->write(messagedata);
}

char MainWindow::getNTimelineParts( com_id cubeID )
{
    return timelines[cubeID]->parts.size();
}

char * MainWindow::getTimelinePart(  int n, com_id cubeID )
{
    if ( n < 0 ) { showSerialData("Negative n in getTimelinePart() \r\n"); }
    return timelines[cubeID]->parts[n].array;
}

void MainWindow::showChoreographyFailure()
{
    if ( inChoreographyFailure ) return;

    showAppInfo("Waiting for all cubes to recover...");
    inChoreographyFailure = true;

    accumulatedMilliseconds += choreographyTime.restart();
}


void MainWindow::showChoreographyRecovery()
{
    if ( !inChoreographyFailure ) return;

    showAppInfo("Performing");
    inChoreographyFailure = false;

    choreographyTime.restart();
}

// Slots:
// -----

void MainWindow::plusButtonAction()
{
    showAppInfo("Checking for cubes");
    busyChecking();
    for ( int i = 0; i < 10; i++)
    {
    comstatemanager->update( COM_BCODE_CHECK_CUBES, NULL, COM_ID_COMPUTER );
    delay(100);
    }

    notBusy();
    showAppInfo("");

    com_id cubeID = comstatemanager->getFirstConnectedCube();
    if ( !cubeID ) { showAppInfo(QString("No cube connected")); return; }
}

void MainWindow::showSettingsDialog()
{
    serialmanager->showSettingsDialog();
}


void MainWindow::readData()
{

    // read and display data
    QByteArray dataread = serialmanager->read();
    for(int i = 0; i < dataread.length();i++)
    {
        this->showSerialData(QString(dataread.data()[i]));
    }

    // reassemble cut-up messages
    static QByteArray toDecrypt;
    if( toDecrypt.length() > COM_P_MAX_MESSAGE_SIZE ) {toDecrypt.clear();}
    toDecrypt.append(dataread);

    com_mcode message_code = COM_MCODE_NOMESSAGE;
    com_id sender;
    char extra_content[COM_P_MAX_EXTRA_CONTENT_SIZE] = {0};
    size_t extra_content_len = 0;
    size_t chars_read;
    chars_read = decrypt_message(toDecrypt.data(),toDecrypt.length(), &message_code,&sender,extra_content,&extra_content_len);

    // make a vector of the extra content
    std::vector<char> * extra_content_vec = NULL;
    if ( extra_content_len != 0 )
    {
    extra_content_vec = new std::vector<char>;
    for ( size_t i = 0; i < extra_content_len; i++ ) { extra_content_vec->push_back( extra_content[i] ); }
    }

    if(chars_read)
    {
        toDecrypt.clear();
        //showSerialData(" ACK\r\n");
        comstatemanager->update( message_code, extra_content_vec, sender );
    }
    else
    {
        ui->serialData->insertHtml("<font color=\"#FFE6E6\">(x)<\font><font color=\"black\"> <\font><br>"); // allows visual recognition of incomplete messages
    }
}

void MainWindow::applyButtonAction()
{
    busySyncing();
    updateAndSyncTimelines();
    notBusy();
    updateCubeStatuses();
}

void MainWindow::playButtonAction()
{
    switch ( currentappstatus )
    {
    case NORMAL :
        startPerformance();
        break;

    case BUSYSYNCING :
        abortSync();
        break;

    case BUSYCOUNTING :
    case BUSYPERFORMING :
        abortPerformance();
        showAppInfo("Performance stopped");
        break;

    default :
        break;
    }
    updateCubeStatuses();
}


#ifdef CHOREOGRAPHER_DEBUG
void MainWindow::sendData()
{
    QString data = ui->sendTextEdit->toPlainText();
    this->showSerialData(QString(">>"));
    this->showSerialData(data);
    this->showSerialData(QString("\r\n"));
    serialmanager->write( data.toStdString().c_str() );
}
void MainWindow::DEBUG_C2B1(){this->sendMessage(COM_MCODE_DEBUG ,COM_ID_ALL , NULL, 0 );}
void MainWindow::DEBUG_C2B2(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_NOMESSAGE      ,COM_ID_COMPUTER,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C2B3(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_NOMESSAGE      ,COM_ID_COMPUTER,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C4B1(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_WAITING        ,COM_ID_NEWCUBLI,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C4B2(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_RECEIVED       ,COM_ID_NEWCUBLI,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C4B3(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_CONNECT        ,COM_ID_NEWCUBLI,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C4B4(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_NOMESSAGE      ,COM_ID_NEWCUBLI,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C4B5(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_NOMESSAGE      ,COM_ID_NEWCUBLI,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C5B1(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_WAITING        ,COM_ID_CUBLI2  ,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C5B2(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_RECEIVED       ,COM_ID_CUBLI2  ,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C5B3(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_CONNECT        ,COM_ID_CUBLI2  ,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C5B4(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_NOMESSAGE      ,COM_ID_CUBLI2  ,COM_ID_COMPUTER);}
void MainWindow::DEBUG_C5B5(){this->DEBUG_EMULATE_INCOMING_MESSAGE(COM_MCODE_NOMESSAGE      ,COM_ID_CUBLI2  ,COM_ID_COMPUTER);}
void MainWindow::DEBUG_EMULATE_INCOMING_MESSAGE(com_mcode message_code, com_id sender, com_id recipient)
{
    send_message(message_code,recipient,NULL,0);
    _sequence[2] = (char)sender+'0';
    for(size_t i = 0; i < _seq_len; i++) { this->showSerialData(QString(_sequence[i])); }
    this->showSerialData("\r\n");
    comstatemanager->update(message_code,NULL,sender);
}
#endif

// Private:
// -------

int  MainWindow::getChoreographyLengthInSeconds()
{
    int length = 0;

    for (auto &t : timelines)
    {
        if ( !t ) { continue; }
            if ( t->endTime > length) { length = t->endTime; }
    }

    return length;
}

void MainWindow::makeTimeline(com_id cube_id)
{
    // make the timeline
    Timeline * temp_timeline = new Timeline(this);
    timelines[cube_id] = temp_timeline;

    // make the timeline label
    QLabel * temp_label = new QLabel();
    temp_label->setText(QString( getComIDString(cube_id)) );
    timelineLabels[cube_id] = temp_label;

    // show the timeline
    showTimeline(cube_id);
}

void MainWindow::showTimeline(com_id cube_id)
{
    if ( !timelines[cube_id] )      { return; }
    if ( !timelineLabels[cube_id] ) { return; }

    // show the timeline label
    showLabel( timelineLabels[cube_id] );
    updateCubeStatuses();

    // show the timeline
    Timeline * temp_timeline = timelines[cube_id];
    showGraphicsView( temp_timeline->getView() );
}

void MainWindow::hideTimeline(com_id cube_id)
{
    if ( !timelines[cube_id] )      { return; }
    if ( !timelineLabels[cube_id] ) { return; }
    // hide the label
    hideLabel( timelineLabels[cube_id] );

    // hide the timeline
    Timeline * temp_timeline = timelines[cube_id];
    hideGraphicsView(temp_timeline->getView());
}

void MainWindow::showLabel(QLabel * label)
{
    ui->timelineLayout->addWidget( label );
}

void MainWindow::hideLabel(QLabel * label)
{
    ui->timelineLayout->removeWidget( label );
}

void MainWindow::showGraphicsView(QGraphicsView *view)
{
    ui->timelineLayout->addWidget(view);
    view->setFixedHeight(110);
}

void MainWindow::hideGraphicsView(QGraphicsView *view)
{
    ui->timelineLayout->removeWidget(view);

}

static int giveUpNow;
void MainWindow::updateAndSyncTimelines()
{
    // update timeline parts
    for ( auto &i : timelines )
    {
        if ( i ) { i->updateParts(); }
    }

    // TODO: move from here on out to a comstatemanager method ?
    // Check that cubes need to be synced
    if ( !comstatemanager->setCubesToSyncing() )
    { showAppInfo("Nothing to sync"); return;  }

    // Try Syncing the cubes
    showAppInfo("Syncing...");
    int giveUpAfter = 100;
    giveUpNow = 0;
    for ( int i = 0; i < giveUpAfter && !giveUpNow; i++)
    {
        // Check if all timelines sent
        if ( !comstatemanager->nCubesToSync() )
        {
            // TODO: update cube indicators
            // ...
            showAppInfo("Sync successful");
            return;
        }

        // Proceed with transfer / force resend of last message
        comstatemanager->update( COM_BCODE_TRANSFER, NULL, COM_ID_COMPUTER );

        // TODO: update fancy "syncing" animation
        // ...

        // Give the transfer a chance to complete
        delay(COM_P_RETRY_DELAY);
    }

    // Failure to sync
    showAppInfo("Sync Unsuccessful");
    comstatemanager->update( COM_BCODE_ABORT_TRANSFER,
                             NULL, COM_ID_COMPUTER );
    comstatemanager->setCubesToUnsynced();

}

void MainWindow::abortSync()
{
    comstatemanager->update( COM_BCODE_ABORT_TRANSFER, NULL, COM_ID_COMPUTER );

    giveUpNow = 1;
    notBusy();
}

void MainWindow::startPerformance()
{

    if ( !comstatemanager->nCubesSynced()   )
    { showAppInfo("No synced timeline to play"); return; }

    giveUpNow = 0;
    for ( char countdown = 10; countdown > 0 && !giveUpNow; countdown-- )
    {
        busyCounting(countdown);
        showAppInfo("Starting in "+QString::number(countdown)+"...");

        comstatemanager->update( COM_BCODE_START_IN,
                                 new std::vector<char>(1,countdown),
                                 COM_ID_COMPUTER );

        delay( COM_P_COUNTDOWN_DELAY );

        // TODO: make this an option
        if ( countdown < 3 && comstatemanager->nCubesSynced() )
        {
            comstatemanager->setSyncedCubesToDisconnected();
            abortPerformance();
            comstatemanager->update( COM_BCODE_ABORT_PERFORMANCE,
                                     NULL, COM_ID_COMPUTER );
            showAppInfo("Performance cancelled");
        }
    }

    if ( giveUpNow ) {
        return; }


    showAppInfo("Started");

    // timekeeping variables
    choreographyTime.start();
    accumulatedMilliseconds = 0;
    inChoreographyFailure  = false;

    comstatemanager->update( COM_BCODE_STARTED, NULL, COM_ID_COMPUTER );
    busyPerforming();

    duringPerformance();

}

void MainWindow::duringPerformance()
{
    int elapsedSeconds = 0;
    abortPerformanceFromComStateManager = false;

    // Choreography Timekeeping
    for ( ;; )
    {
        delay( COM_P_TIMELINE_DT * 1000 );

        // TODO: terminate if app is closed
        // ...

        // stop choreography due to cube order
        if ( abortPerformanceFromComStateManager )
        {
            abortPerformanceFromComStateManager = false;
            showAppInfo("Choreography stopped by cube");
            abortPerformance();
        }

        // inform cubes when choreography is over
        if ( elapsedSeconds >= getChoreographyLengthInSeconds()  )
        {
            delay( 1000 );
            showAppInfo("Choreography complete");
            abortPerformance();
        }

        // terminate if the choreography was ended
        if ( ui->timelinesWidget->isEnabled() )
        {
            for (auto &t : timelines)
            {
                if ( !t ) continue;
                t->hideChoreographyProgress();
            }
            break;
        }

        // update the elapsed time
        if ( choreographyTime != QTime(0,0,0,0) )
        {
            if ( inChoreographyFailure )
            {
                elapsedSeconds = accumulatedMilliseconds / 1000;
            }
            else
            {
                elapsedSeconds = ( accumulatedMilliseconds + choreographyTime.elapsed() ) / 1000 ;
            }
        }

        // show progress in timelines
        for (auto &t : timelines)
        {
            // check that timeline exists
            if ( !t ) continue;
            // TODO: check that cube is performing
            // ...

            t->showChoreographyProgress( S_TO_PX( elapsedSeconds ) );
        }

        // Detect when cubes need a resume order and give it
        performanceResumingSequence();
    }
}

void MainWindow::abortPerformance()
{
    giveUpNow = 1;
    busyAborting();
    for ( int i = 10; i != 0 && comstatemanager->nCubesPerformingOrCounting() ; i-- )
    {
        comstatemanager->update( COM_BCODE_ABORT_PERFORMANCE,
                                 NULL, COM_ID_COMPUTER );

        delay(COM_P_RETRY_DELAY);
    }

    // mark cubes that didn't respond as disconnected
    comstatemanager->setPerformingOrCountingCubesToDisconnected();

    // timekeeping variables
    choreographyTime = QTime(0,0,0,0);
    accumulatedMilliseconds = 0;
    inChoreographyFailure = false;

    notBusy();

}

void MainWindow::performanceResumingSequence()
{
    // if a cube is still failed do nothing
    if ( comstatemanager->nCubesWaitingToRecover() > 0 ) { return; }

    // if all cubes are fine make sure choreography runs and return
    if ( comstatemanager->nCubesWaitingToResume() == 0 )
    {
        showChoreographyRecovery();
        return;
    }

    // If cubes are waiting give resume countdown
    for ( char countdown = 10; countdown > 0; countdown-- )
    {
        comstatemanager->update( COM_BCODE_RESUME_PERFORMANCE,
                                 new std::vector<char>(1,countdown),
                                 COM_ID_COMPUTER );
        delay( COM_P_COUNTDOWN_DELAY );
    }

    // if all cubes got the message resume choreography
    if ( comstatemanager->nCubesWaitingToResume() == 0 )
    {
        showChoreographyRecovery();
    }
}

void MainWindow::busyChecking()
{
    currentappstatus = BUSYCHECKING;
    // Can't use any buttons
    ui->timelinesWidget->setEnabled(false);
    ui->applyButton->setEnabled(false);
    ui->playButton->setEnabled(false);
}

void MainWindow::busySyncing()
{
    currentappstatus = BUSYSYNCING;
    // Can't use the play or apply buttons
    ui->timelinesWidget->setEnabled(false);
    ui->applyButton->setEnabled(false);

    ui->playButton->setText("Syncing");
}

void MainWindow::busyCounting(char count)
{
    currentappstatus = BUSYCOUNTING;
    ui->timelinesWidget->setEnabled(false);
    ui->applyButton->setEnabled(false);

    ui->playButton->setText(QString::number(count));
}

void MainWindow::busyPerforming()
{
    currentappstatus = BUSYPERFORMING;
    ui->timelinesWidget->setEnabled(false);
    // Play button becomes a stop button
    // ...
    ui->playButton->setText("Stop");

}

void MainWindow::busyAborting()
{
    currentappstatus = BUSYABORTING;
    ui->timelinesWidget->setEnabled(false);
    ui->applyButton->setEnabled(false);
    ui->playButton->setEnabled(false);
    ui->playButton->setText("Stopped");
}

void MainWindow::notBusy()
{
    currentappstatus = NORMAL;
    // resets all the inputs to normal
    ui->timelinesWidget->setEnabled(true);
    ui->playButton->setEnabled(true);
    ui->applyButton->setEnabled(true);

    ui->playButton->setText("Play");

}

// Make sure to close the com port if the app is closed
void MainWindow::closeEvent (QCloseEvent *event)
{

    abortPerformance();
}
