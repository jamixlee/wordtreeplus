/*
 * source file for WTWordTreeView class
 * Original by: christian moellinger <ch.moellinger@gmail.com>
 * Modified by: Chung-Yeon Lee <cylee@bi.snu.ac.kr> and Beom-Jin Lee <bjlee@bi.snu.ac.kr>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 * 11/2013 - 12/2013 Project "InfoVis: Word Tree ++"
 */

// qt includes
#include <QMouseEvent>

// project includes
#include "WTWordTreeView.h"

WTWordTreeView::WTWordTreeView(QWidget *parent) :
    QGraphicsView(parent)
{
    m_bControlPressed = false;
}

void WTWordTreeView::mousePressEvent(QMouseEvent *e)
{
    emit OnClicked(e->x(), e->y(), m_bControlPressed);
}

void WTWordTreeView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
	m_bControlPressed = true;
}

void WTWordTreeView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
	m_bControlPressed = false;
}
