#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <QGraphicsView>
#include <QGraphicsItem>

#include "timeline.h"
#include "com_protocol.h"

#define PRIMITIVE_HEIGHT 80

// resolve circular dependencies
class Timeline;

class Primitive : public QGraphicsRectItem
{
public:
    Primitive(QRect rect, Timeline *mom);
    ~Primitive();

    void setCenter(qreal x, bool snapAndCheck = true);
    void snapToGrid();
    void checkCollisions(bool allowDeletions = true);
    void cycleType();

    qreal x2();
    qreal width();
    com_pcode getType();
    void setType(com_pcode Ptype);

private:
    Timeline * parent;

    com_pcode type;

    int duration();
    QBrush style();

    void updateTypeAppearance();

};

#endif      // PRIMITIVE_H
