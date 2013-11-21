/*
 * header file for WTTreeVisualizer class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */

#ifndef WTTREEVISUALIZER_H
#define WTTREEVISUALIZER_H

#include <QObject>
#include <QSharedPointer>
#include <QGraphicsScene>
#include <QTimer>
#include "WTBackend.h"
#include "WTVisualizedTree.h"

/*! This class takes a WTVisualizedTree object and redraws it (on a graphics scene / view)
 */
class WTTreeVisualizer : public QObject
{
    Q_OBJECT
public:
    /*! \name Construction / Destruction */
    //@{
	/// Constructor
	explicit WTTreeVisualizer(QObject *parent = 0);

	/// Destructor
	~WTTreeVisualizer();
    //@}


    /*! \name Public methods */
    //@{
	/// this method sets the graphics view where the output should be drawn
	void SetGraphicsView(QGraphicsView *pGraphicsView);

	/// this method draws a visualized tree on the output view
	void VisualizeWordTree(QSharedPointer<WTVisualizedTree> spWordTree);
    //@}
private slots:
    void TimerStep();

private:
    /*! \name Private methods */
    //@{
	/// renders a tree transition between two trees
	void ItlDrawTreeTransition(QSharedPointer<WTVisualizedTree> spTree1,
				QSharedPointer<WTVisualizedTree> spTree2,
				float fAlpha);
    //@}

    /*! \name Private members */
    //@{
	QSharedPointer<WTVisualizedTree> m_spLastVisualizedWordTree;
	QSharedPointer<WTVisualizedTree> m_spNewVisualizedWordTree;
	QGraphicsView *m_pGraphicsView;
	QTimer m_rTimer;

	int m_iMillisecondsLeft;
    //@}

};

#endif // WTTREEVISUALIZER_H
