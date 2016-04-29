#include "primitive.h"


Primitive::Primitive(QRect rect, Timeline * mom) : QGraphicsRectItem(rect)
{
    type = COM_PCODE_NONE;
    cycleType();
    parent = mom;
}

Primitive::~Primitive()
{
}

void Primitive::setCenter(qreal x, bool snapAndCheck )
{
    this->setX( x - this->rect().width()/2 );
    if ( snapAndCheck )
    {
        snapToGrid();
        checkCollisions(/* allowDeletions = */ false);
    }
}

void Primitive::checkCollisions( bool allowDeletions )
{
    // attempt to move the conflicting item one step backwards
    if ( !this->collidingItems().isEmpty() )
    {
        qreal dx = this->x2() - this->collidingItems().back()->x();
        if ( this->x() <= ((Primitive*)this->collidingItems().back())->x() )
        {
            this->moveBy(-dx,0); snapToGrid();
        }
    }
    // if there is no space, move it forward until one is found
    while ( !this->collidingItems().isEmpty() || this->x() < 0)
    {
        if ( ( this->x2() + PIXELS_PER_UNIT ) > TIMELINE_MAX_PX )
        {
            //if the end of timeline is reached delete this;
            if ( allowDeletions ) { if ( parent )  { parent->deletePrimitive(this); } }
            return;
        }

        this->moveBy(PIXELS_PER_UNIT, 0); snapToGrid();
    }
}

void Primitive::snapToGrid()
{
    // keep in scene
    if ( this->x() < 0) this->setX(0);
    if ( this->x2() > TIMELINE_MAX_PX ) this->setX( TIMELINE_MAX_PX - this->width() );
    // snap to grid
    qreal offset = fmod(this->x(), PIXELS_PER_UNIT );
    if ( offset ) { this->setX( this->x() - offset );
#ifdef CHOREOGRAPHER_DEBUG
    if ( parent ) { parent->showSerialData(QString::number(qCeil(offset))); }
#endif
    }
}

void Primitive::cycleType()
{
    // this defines the order in which primitives are cycled
    switch (type)
    {
        case COM_PCODE_NONE   : setType(COM_PCODE_JUMP2G); break;
        case COM_PCODE_JUMP2G : setType(COM_PCODE_JUMP2E); break;
        case COM_PCODE_JUMP2E : setType(COM_PCODE_JUMP2C); break;
        case COM_PCODE_JUMP2C : setType(COM_PCODE_1SPIN ); break;
        case COM_PCODE_1SPIN  : setType(COM_PCODE_JUMP2G); break;
        default :
            printf("invalid type in cycleType()"); setType(COM_PCODE_JUMP2E); break;
    }
    snapToGrid();
}

void Primitive::setType(com_pcode Ptype)
{
    type = Ptype;
    updateTypeAppearance();
}

qreal Primitive::x2()
{
    return this->x()+this->width();
}

com_pcode Primitive::getType()
{
    return type;
}

void Primitive::updateTypeAppearance()
{
    // set the width
    qreal x = this->x();
    this->setRect(0, 0, width(), PRIMITIVE_HEIGHT);
    this->setX(x);
    // set the color based on type
    this->setBrush(style());
    this->setPen(Qt::NoPen);
}

int Primitive::duration()
{
    int result = get_primitive_duration( type );
    if ( result != -1 ) return result;

    printf("invalid type in duration()");
    return 1;
}

QBrush Primitive::style()
{
    // brush for primitive
    switch (type)
    {
        case COM_PCODE_NONE   : return QBrush(Qt::white);
        case COM_PCODE_JUMP2E : return QBrush(Qt::blue);
        case COM_PCODE_JUMP2C : return QBrush(Qt::green);
        case COM_PCODE_JUMP2G : return QBrush(Qt::red);
        case COM_PCODE_1SPIN  : return QBrush(Qt::yellow);
    default : printf("invalid type in typeBrush()"); return QBrush(Qt::black);
    }
}

qreal Primitive::width()
{
    return (qreal) S_TO_PX(duration());
}
