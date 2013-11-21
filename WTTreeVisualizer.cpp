/*
 * source file for WTTreeVisualizer class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */


// c++ includes
#include <cmath>

// qt includes
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QTimer>

// project includes
#include "WTTreeVisualizer.h"

const int iFadeInTime = 500;	///< time to fade in (milliseconds)

WTTreeVisualizer::WTTreeVisualizer(QObject *parent) :
    QObject(parent)
{
    // initialize ptr to graphics view
    m_pGraphicsView = NULL;

    // initialize ms timer
    m_iMillisecondsLeft = -10;

    // connect timer with slot
    connect(&m_rTimer, SIGNAL(timeout()), this, SLOT(TimerStep()));

    // set timer interval
    m_rTimer.setInterval(50);

    // ! timer is not started yet!
}

WTTreeVisualizer::~WTTreeVisualizer()
{
    // stop timer, if started
    m_rTimer.stop();
}


void WTTreeVisualizer::ItlDrawTreeTransition(QSharedPointer<WTVisualizedTree> spTree1,
					  QSharedPointer<WTVisualizedTree> spTree2,
					  float fAlpha)
{
    Q_ASSERT(fAlpha >= 0.0f);
    Q_ASSERT(fAlpha <= 1.0f);
    Q_ASSERT(m_pGraphicsView != NULL);

    int iWidth = 1;//m_pGraphicsView->width();
    int iHeight = 1;//m_pGraphicsView->height();

    // create a new temporary scene
    QGraphicsScene *pNewScene = new QGraphicsScene();

    // create font
    QFont font;

    //render lines
    QVector<WTVisualizedTree::TVisualizedLineInfo> vLines1 = spTree1->GetRenderedLines();
    QVector<WTVisualizedTree::TVisualizedLineInfo> vLines2 = spTree2->GetRenderedLines();

    float fAlpha2 = 1.0f - fAlpha * 2;

    if (fAlpha2 < 0.0f)
	fAlpha2 = 0.0f;



    float fAlpha1 = (fAlpha - 0.5f) * 2;

    if (fAlpha1 < 0.0f)
	fAlpha1 = 0.0f;

    Q_ASSERT(fAlpha1 >= 0.0f);
    Q_ASSERT(fAlpha2 >= 0.0f);
    Q_ASSERT(fAlpha1 <= 1.0f);
    Q_ASSERT(fAlpha2 <= 1.0f);

    // create two pens (one for each tree)
    QPen pen1, pen2;
    int iRed = 255;
    int iGreen = 153;
    int iBlue = 0;

    // set properties of the pens
    pen1.setWidth(2);
    pen2.setWidth(2);

    // set color of pens
    pen1.setColor(QColor(iRed * fAlpha1 + 255 * (1.0 - fAlpha1),
			 iGreen * fAlpha1 + 255 * (1.0 - fAlpha1),
			 iBlue * fAlpha1 + 255 * (1.0 - fAlpha1)));

    pen2.setColor(QColor(iRed * fAlpha2 + 255 * (1.0 - fAlpha2),
			 iGreen * fAlpha2 + 255 * (1.0 - fAlpha2),
			 iBlue * fAlpha2 + 255 * (1.0 - fAlpha2)));

    // draw lines for tree2
    if (fAlpha > 0.5)
    for (QVector<WTVisualizedTree::TVisualizedLineInfo>::iterator iter = vLines1.begin(); iter != vLines1.end(); iter++)
    {
	WTVisualizedTree::TVisualizedLineInfo *pInfo1 = &(*iter);
	QPainterPath connectSpline(QPointF(pInfo1->dX1 * iWidth, pInfo1->dY1 * iHeight));


	unsigned int deltaX = (pInfo1->dX2 - pInfo1->dX1) * iWidth;

	connectSpline.cubicTo(pInfo1->dX1 * iWidth + deltaX / 2,
			      pInfo1->dY1 * iHeight,
			      pInfo1->dX1 * iWidth + deltaX / 2,
			      pInfo1->dY2 * iHeight,
			      pInfo1->dX2 * iWidth,
			      pInfo1->dY2 * iHeight);

	pNewScene->addPath(connectSpline, pen1);
    }


    // draw lines for tree 1
    if (fAlpha < 0.5)
    for (QVector<WTVisualizedTree::TVisualizedLineInfo>::iterator iter = vLines2.begin(); iter != vLines2.end(); iter++)
    {
	WTVisualizedTree::TVisualizedLineInfo *pInfo2 = &(*iter);
	QPainterPath connectSpline(QPointF(pInfo2->dX1 * iWidth, pInfo2->dY1 * iHeight));

	unsigned int deltaX = (pInfo2->dX2 - pInfo2->dX1) * iWidth;

	connectSpline.cubicTo(pInfo2->dX1 * iWidth + deltaX / 2,
			      pInfo2->dY1 * iHeight,
			      pInfo2->dX1 * iWidth + deltaX / 2,
			      pInfo2->dY2 * iHeight,
			      pInfo2->dX2 * iWidth,
			      pInfo2->dY2 * iHeight);

	pNewScene->addPath(connectSpline, pen2);
    }

    //render text
    QMap<long long int, WTVisualizedTree::TVisualizedWordInfo> *pInfoMapTree1 = spTree1->GetInfoMap();
    QMap<long long int, WTVisualizedTree::TVisualizedWordInfo> *pInfoMapTree2 = spTree2->GetInfoMap();


    for (QMap<long long int, WTVisualizedTree::TVisualizedWordInfo>::iterator iter=pInfoMapTree1->begin(); iter != pInfoMapTree1->end(); iter++)
    {
	WTVisualizedTree::TVisualizedWordInfo *pInfo1 = &(iter.value());

	if (pInfoMapTree2->contains(iter.key()))
	{
	    QMap<long long int, WTVisualizedTree::TVisualizedWordInfo>::iterator pInfo2 = pInfoMapTree2->find(iter.key());

	    if (pInfo1->bIsFirstElement)
	    {
		Q_ASSERT(pInfo1->iFontSize * fAlpha >= 0);
		font.setPixelSize(pInfo1->iFontSize * fAlpha + pInfo2.value().iFontSize * (1.0f - fAlpha));
		QGraphicsTextItem *pNewTextItem = pNewScene->addText(QString(pInfo1->sWord.data()), font);

		pNewTextItem->setPos((pInfo1->dLeft * fAlpha + pInfo2.value().dLeft * (1.0f - fAlpha)) * iWidth,
				     (pInfo1->dTop *fAlpha + (pInfo2.value().dTop * (1.0f - fAlpha))) * iHeight);

	    }
	}
	else
	{
	    if (pInfo1->bIsFirstElement && pInfo1->iFontSize * fAlpha > 3)
	    {
		Q_ASSERT(pInfo1->iFontSize * fAlpha >= 0);

		font.setPixelSize(pInfo1->iFontSize * fAlpha);


		QGraphicsTextItem *pNewTextItem = pNewScene->addText(QString(pInfo1->sWord.data()), font);

		pNewTextItem->setPos(pInfo1->dLeft * iWidth, pInfo1->dTop * iHeight);

		int rgbValue = (1.0 - fAlpha) * 255;

		if (fAlpha < 0.0)
		    rgbValue = 255;

		QColor color(rgbValue, rgbValue, rgbValue, 255);

		pNewTextItem->setDefaultTextColor(color);
	    }
	}
    }


    if (m_pGraphicsView->scene() != NULL)
	delete m_pGraphicsView->scene();

    m_pGraphicsView->setScene(pNewScene);
}

void WTTreeVisualizer::VisualizeWordTree(QSharedPointer<WTVisualizedTree> spWordTree)
{

	// set millisecondsLeft to half fade in time, if it is the first tree
	// (so we see only the fade in, not the fade out of the old tree)
	if (m_spNewVisualizedWordTree.isNull())
	{
	    m_iMillisecondsLeft = iFadeInTime / 2;

	    m_spNewVisualizedWordTree = spWordTree;
	    m_rTimer.start();
	}
	else
	{
	    // only draw new tree, if it's not the same as before
	    if (spWordTree->GetSearchPhrase().compare(m_spLastVisualizedWordTree->GetSearchPhrase()) != 0)
	    {
		m_iMillisecondsLeft = iFadeInTime;

		m_spNewVisualizedWordTree = spWordTree;
		m_rTimer.start();
	    }
	}
}

void WTTreeVisualizer::SetGraphicsView(QGraphicsView *pGraphicsView)
{
    Q_ASSERT (m_pGraphicsView == NULL);

    m_pGraphicsView = pGraphicsView;
}

void WTTreeVisualizer::TimerStep()
{
    Q_ASSERT(m_spNewVisualizedWordTree.isNull() == false);

    float fAlpha = (m_iMillisecondsLeft / (double) iFadeInTime);

    if (fAlpha < 0.0f)
	fAlpha = 0.0f;

    if (m_spLastVisualizedWordTree.isNull())
    {
	ItlDrawTreeTransition(m_spNewVisualizedWordTree, m_spNewVisualizedWordTree, fAlpha);
    }
    else
	ItlDrawTreeTransition(m_spNewVisualizedWordTree, m_spLastVisualizedWordTree, 1.0f - fAlpha);

    if (m_iMillisecondsLeft > 0)
	m_iMillisecondsLeft -= m_rTimer.interval();
    else
    {
	m_spLastVisualizedWordTree = m_spNewVisualizedWordTree;
	m_rTimer.stop();
    }
}
