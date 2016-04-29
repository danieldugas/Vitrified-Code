#include "timeline.h"

Timeline::Timeline(MainWindow *mom) : QGraphicsScene(mom),
    parent(mom)
{
    view = new QGraphicsView(this, parent);
    selected = NULL;
    endTime = 0;

    leftBoundVisual = new QGraphicsRectItem( QRect(0, -20, -TIMELINE_MAX_PX, PRIMITIVE_HEIGHT+40) );
        leftBoundVisual->setOpacity(0.1);
        leftBoundVisual->setBrush(QBrush(Qt::black));
        leftBoundVisual->setPen(Qt::NoPen);
        addItem(leftBoundVisual);
    rightBoundVisual = new QGraphicsRectItem( QRect(TIMELINE_MAX_PX, -20, 2*TIMELINE_MAX_PX, PRIMITIVE_HEIGHT+40) );
        rightBoundVisual->setOpacity(0.1);
        rightBoundVisual->setBrush(QBrush(Qt::black));
        rightBoundVisual->setPen(Qt::NoPen);
        addItem(rightBoundVisual);

    this->setSceneRect(0, 0, TIMELINE_MAX_PX, PRIMITIVE_HEIGHT);

    scrollVisual = new QGraphicsRectItem( QRect(0, 0, 0, 0 ) );
        scrollVisual->setVisible(false);
        scrollVisual->setOpacity(0.2);
        scrollVisual->setBrush(QBrush(Qt::darkGreen));
        scrollVisual->setPen(Qt::NoPen);
}

Timeline::~Timeline()
{
    //TODO: make sure all is gone.
}



QGraphicsView * Timeline::getView()
{
    return view;
}

void Timeline::showSerialData(const QString& text)
{
    parent->showSerialData(text);
}


void Timeline::updateParts()
{
    parts.clear();
    Part * part;

    // Build timeline info
    // sort primitives
    sortPrimitives();
    // add implied primitives
    addImpliedPrimitives();

    // zeroth part
    part = new Part;
    part->array[0] = (char) 0;
    part->array[1] = (char) COM_PCODE_NONE;
    part->array[2] = ( primitives.empty() ? 0 : calculatePause( NULL, primitives.front() ) );
    part->array[3] = 0;
    parts.push_back( *part );

    // primitive parts
    std::vector<Primitive *>::iterator it;
    int count;
    Primitive * next;
    for ( it = primitives.begin(), count = 1; it != primitives.end(); ++it, count++ )
    {
        if ( it == primitives.end()-1 ) { next = NULL; }
        else { next = *(it+1); }

        part = new Part;
        part->array[0] = (char) count;
        part->array[1] = (char) (*it)->getType();
        part->array[2] =        calculatePause( *it, next );
        parts.push_back( *part );

        if ( it == primitives.end()-1 ) { endTime = PX_TO_S( (*it)->x2() ); }
    }

}

void Timeline::showChoreographyProgress( int elapsedSeconds )
{
    scrollVisual->setRect(0, -20, elapsedSeconds, PRIMITIVE_HEIGHT+40);
    if( !scrollVisual->isVisible() )
    {
        addItem(scrollVisual);
        scrollVisual->setVisible(true);
    }
    view->centerOn(QPointF(elapsedSeconds, 0));
}

void Timeline::hideChoreographyProgress()
{
    scrollVisual->setRect(0, 0, 0, 0);
    if( scrollVisual->isVisible() )
    {
        removeItem(scrollVisual);
        scrollVisual->setVisible(false);
    }
}

// Private Methods:
// ---------------

Primitive * Timeline::addPrimitive()
{
    return addPrimitive(COM_PCODE_NONE);
}

Primitive * Timeline::addPrimitive(com_pcode Ptype)
{
    Primitive * temp = new Primitive( QRect( 0, 0, 1, PRIMITIVE_HEIGHT ) , this );
    if ( Ptype ) { temp->setType(Ptype); }
    this->addItem(temp);
    return temp;
}

void Timeline::deletePrimitive( Primitive * toDelete)
{
    unselect();
    removeItem(toDelete);
    delete toDelete;
}


void Timeline::addImpliedPrimitives()
{
    // Assumes a sorted primitives array
    Primitive * prev_primitive = NULL;
    int added_count = 0;

    std::vector<Primitive *>::iterator i;
    std::vector<Primitive *>::iterator j; // inner loop
    for ( i=primitives.begin(); i != primitives.end(); ++i )
    {
        Primitive * this_primitive = *i;

        // determine if a primitive is needed
        com_pcode missing_pcode;
        if (prev_primitive)
        {
            missing_pcode = conflict( prev_primitive->getType(), this_primitive->getType() );
        }
        else
        {
            missing_pcode = conflict( COM_PCODE_NONE, this_primitive->getType() );
        }

        // if necessary add the missing primitive and move the rest
        if ( missing_pcode )
        {
            added_count++;
            Primitive * missing_primitive = addPrimitive(missing_pcode);
            missing_primitive->setOpacity(0.4);
            qreal missing_x =  this_primitive->x() - missing_primitive->width();
            missing_primitive->setX( missing_x );

            // determine wether to move the rest
            qreal move_next_by;
            if (prev_primitive) { move_next_by = prev_primitive->x2() - missing_primitive->x() ; }
            else { move_next_by = 0 - missing_primitive->x() ; }
            missing_primitive->moveBy(( (move_next_by > 0) ? move_next_by : 0), 0);

            qreal prev_x2 = missing_primitive->x2();
            for (j = i; j != primitives.end() ; ++j)
            {
                if ( move_next_by > 0 )
                {
                    (*j)->moveBy(move_next_by,0);
                    // prepare the next loop
                    if ( 0 )
                    {
                    // can't work because the added primitives are not in primitives[]
                    printf(" HERE BE SEGFAULTS ");
                    move_next_by = prev_x2 - (*j)->x();
                    prev_x2 = (*j)->x2();
                    primitives.insert(i, missing_primitive); i++;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        // prepare the next loop
        prev_primitive = this_primitive;
    }

    sortPrimitives();
    removeOutOfBoundsPrimitives();
    findPrimitives();
    sortPrimitives();
}

void Timeline::removeOutOfBoundsPrimitives()
{
    std::vector<Primitive *>::iterator i;
    for ( i=primitives.begin(); i != primitives.end(); ++i )
    {
        Primitive * this_primitive = *i;

        if ( this_primitive->x2() > TIMELINE_MAX_PX )
        {
            deletePrimitive(this_primitive);
        }
    }
}

void Timeline::findPrimitives()
{
    // inefficient: did not find a better way
    primitives.clear();
    // Remove Items which are not primitives
    removeItem(leftBoundVisual);
    removeItem(rightBoundVisual);
    for (QGraphicsItem * a : this->items())
    {
        primitives.push_back( dynamic_cast<Primitive*>(a) );
    }
    addItem(leftBoundVisual);
    addItem(rightBoundVisual);
}

void Timeline::sortPrimitives()
{

    findPrimitives();

    struct x_less_than
    {
        inline bool operator() (const Primitive * primitive1, const Primitive * primitive2)
        {
            return (primitive1->x() < primitive2->x());
        }
    };

    std::sort(primitives.begin(), primitives.end(), x_less_than());
}

com_pcode Timeline::conflict(com_pcode first, com_pcode second)
{
    // returns the necessary primitive for solving a conflict
    // Note: COM_PCODE_NONE can represent the start of choreography

    // here are defined conflicts between primitives
    if (first == COM_PCODE_JUMP2G && second == COM_PCODE_JUMP2G) { return COM_PCODE_JUMP2E; }
    if (first == COM_PCODE_JUMP2E && second == COM_PCODE_JUMP2E) { return COM_PCODE_JUMP2G; }
    if (first == COM_PCODE_JUMP2C && second == COM_PCODE_JUMP2E) { return COM_PCODE_JUMP2G; } // TODO: implement C2E and remove it here?
    if (first == COM_PCODE_JUMP2C && second == COM_PCODE_JUMP2C) { return COM_PCODE_JUMP2G; }
        // spin conflicts
    if (first != COM_PCODE_JUMP2C && second == COM_PCODE_1SPIN ) { return COM_PCODE_JUMP2C; }
    if (first == COM_PCODE_1SPIN  && second == COM_PCODE_1SPIN ) { return COM_PCODE_NONE;   }
    if (first == COM_PCODE_1SPIN  && second != COM_PCODE_JUMP2G) { return COM_PCODE_JUMP2G; }
    return COM_PCODE_NONE; // no conflict
}

char Timeline::calculatePause(Primitive * first, Primitive * second )
{
    if ( !first ) { return (char) static_cast<int>( PX_TO_U( second->x() ) ); }
    if ( !second ) { return (char) 0; }
    return (char)  static_cast<int>( PX_TO_U( second->x() - first->x2() ) );
}

void Timeline::select(Primitive * target)
{
    if ( target )
    {
        selected = target;
        selected->setOpacity(0.4);
    }
}

void Timeline::unselect()
{
    if (selected)
    {
        selected->setOpacity(1);
        selected = NULL;
    }
    forgetLastPress();
}

void Timeline::forgetLastPress()
{
    lastpress.setX(-100);
}

// Protected Methods:
// -----------------

void Timeline::mousePressEvent(QGraphicsSceneMouseEvent*event)
{
    // Get Click Position
    const QPointF point = event->scenePos();

    // Ignore clicks outside of bounds
    if ( point.x() < 0 || point.x() > TIMELINE_MAX_PX ) { return; }

    // Get Target
    Primitive * target = dynamic_cast<Primitive*>( itemAt(point, QTransform()) );

    // Right button
    if(event->buttons() & Qt::RightButton)
    {
        if (target)
        {
            this->removeItem(target);
            delete target;
        }
        unselect();
    }

    // Left Button
    if(event->buttons() & Qt::LeftButton)
    {
        lastpress = point;
        if (target)
        {
            // clicked on a primitive
            select(target);
        }
    }
}

void Timeline::mouseMoveEvent(QGraphicsSceneMouseEvent*event)
{
    // Cursor location
    const QPointF point = event->scenePos();

    // While mouse is pressed and moving
    if(event->buttons() & Qt::LeftButton)
    {
        if ( selected )
        {
            // move the selected primitive
            selected->setCenter( point.x() );
        }
        else
        {
            // user click-dragging the scene

            // TODO: select many primitives?
        }
    }

    if(event->buttons() & Qt::RightButton)
    {
        Primitive * target = dynamic_cast<Primitive*>( itemAt(point, QTransform()) );
        if (target)
        {
            deletePrimitive( target );
        }
        unselect();

    }
}

void Timeline::mouseReleaseEvent(QGraphicsSceneMouseEvent*event)
{
    QPointF point = event->scenePos();
    qreal selected_has_moved_by = qSqrt( qPow( point.x() - lastpress.x() , 2 ) + qPow( point.y() - lastpress.y() , 2 ) );
    if ( selected_has_moved_by < 2 )
    {
        if ( selected )
        {
            // cycle primitive type
            selected->cycleType();
        }
        else
        {
            // clicked and released on empty space
            selected = addPrimitive();
            selected->setCenter( point.x() );
        }
    }

    // Warning: after checkCollisions selected may be nulled
    if ( selected ) { selected->checkCollisions(); }

    parent->showPrimitiveInfo( selected );

    unselect();

}

void Timeline::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // same as a regular click
    mousePressEvent(event);
}


