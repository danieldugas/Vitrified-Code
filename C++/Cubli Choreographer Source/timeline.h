#ifndef TIMELINE_H
#define TIMELINE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <cmath>
#include <QtMath>
#include "mainwindow.h"
#include "primitive.h"

// grid resolution in pixels
#define PIXELS_PER_SECOND 5
#define PX_TO_S(PX) ((PX) / PIXELS_PER_SECOND)
#define S_TO_PX(S)  ((S)  * PIXELS_PER_SECOND)
#define PIXELS_PER_UNIT (S_TO_PX(COM_P_TIMELINE_DT))
#define PX_TO_U(PX) ((PX) / PIXELS_PER_UNIT  )
#define U_TO_PX(U)  ((U)  * PIXELS_PER_UNIT  )
#define TIMELINE_MAX_T  (COM_P_TIMELINE_MAX_PARTS * 6)
#define TIMELINE_MAX_PX (S_TO_PX(TIMELINE_MAX_T ))
#define TIMELINE_MAX_U  (PX_TO_U(TIMELINE_MAX_PX))


// resolve circular dependencies
class MainWindow;
class Primitive;

struct Part{
    char array[COM_P_TIMELINE_PART_SIZE];
};

class Timeline : public QGraphicsScene
{
public:
    Timeline(MainWindow *parent);
    ~Timeline();

    QGraphicsView * getView();
    void showSerialData(const QString &text);

    std::vector<Part> parts;
    void updateParts();

    int endTime;

    void showChoreographyProgress( int elapsedSeconds );
    void hideChoreographyProgress();

    void deletePrimitive(Primitive * toDelete);

private:
    QGraphicsView *view;
    MainWindow *parent;

    Primitive * selected;
    QPointF lastpress;

    QGraphicsRectItem * scrollVisual;
    QGraphicsRectItem * leftBoundVisual;
    QGraphicsRectItem * rightBoundVisual;

    Primitive * addPrimitive();
    Primitive * addPrimitive(com_pcode Ptype);


    std::vector<Primitive *> primitives;

    void addImpliedPrimitives();
    void removeOutOfBoundsPrimitives();
    void findPrimitives();
    void sortPrimitives();
    com_pcode conflict(com_pcode first, com_pcode second);
    char calculatePause( Primitive * first, Primitive * second );


    void select(Primitive * target);
    void unselect();
    void forgetLastPress();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*event);
};

#endif // TIMELINE_H
