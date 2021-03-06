/*
 * header file for WTWordTreeView class
 * Original by: christian moellinger <ch.moellinger@gmail.com>
 * Modified by: Chung-Yeon Lee <cylee@bi.snu.ac.kr> and Beom-Jin Lee <bjlee@bi.snu.ac.kr>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 * 11/2013 - 12/2013 Project "InfoVis: Word Tree ++"
 */

#ifndef WTWORDTREEVIEW_H
#define WTWORDTREEVIEW_H

#include <QGraphicsView>

/// this class is simply an inherited class of QGraphicsView
/// which only reimplements some event handling to react on mouse clicks

class WTWordTreeView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit WTWordTreeView(QWidget *parent = 0);

protected:
    void mousePressEvent (QMouseEvent * e);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    bool m_bControlPressed;
signals:
    void OnClicked(int, int, bool);

public slots:

};

#endif // WTWORDTREEVIEW_H
