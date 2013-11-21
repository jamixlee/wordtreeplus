/*
 * source file for WTVisualizedTree class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */

// stl includes
#include <iostream>

// qt includes
#include <QtGlobal>
#include <QString>

// project includes
#include "WTVisualizedTree.h"

WTVisualizedTree::WTVisualizedTree(QString sSearchPhrase)
{
    m_sSearchPhrase = sSearchPhrase;
}

void WTVisualizedTree::AddRenderedWord(QVector<long long int> vWordIds, QString sWord, QString sPhrase, double dLeft, double dTop, int iWidth, int iHeight,int iFontSize)
{
    Q_ASSERT (vWordIds.size() > 0);

    for (QVector<long long int>::iterator iter=vWordIds.begin(); iter != vWordIds.end(); iter++)
    {
	Q_ASSERT(m_mInfoMap.find(*iter) == m_mInfoMap.end());

	m_mInfoMap[*iter].bIsFirstElement = (iter == vWordIds.begin());
	m_mInfoMap[*iter].dLeft = dLeft;
	m_mInfoMap[*iter].dTop = dTop;
	m_mInfoMap[*iter].sPhrase = sPhrase;
	m_mInfoMap[*iter].iWidth = iWidth;
	m_mInfoMap[*iter].iHeight = iHeight;
	m_mInfoMap[*iter].iFontSize = iFontSize;
	m_mInfoMap[*iter].sWord = sWord;
    }

}

void WTVisualizedTree::AddRenderedLine(double dX1, double dY1, double dX2, double dY2, long long int lStartChar, long long int lEndChar)
{
    TVisualizedLineInfo line;
    line.dX1 = dX1;
    line.dX2 = dX2;
    line.dY1 = dY1;
    line.dY2 = dY2;
    line.lStartWord = lStartChar;
    line.lEndWord = lEndChar;

    m_vRenderedLines.push_back(line);
}

bool WTVisualizedTree::GetWordUnderMouseClick(int iX, int iY, QString &sResult)
{
    for (QMap<long long int, TVisualizedWordInfo>::iterator iter=m_mInfoMap.begin(); iter != m_mInfoMap.end(); iter++)
    {
	if (iter.value().dLeft < iX && (iter.value().dLeft + iter.value().iWidth > iX) &&
		iter.value().dTop < iY && (iter.value().dTop + iter.value().iHeight > iY))
	{
	    sResult = iter.value().sWord.trimmed();
	    return true;
	}
    }

    return false;
}

bool WTVisualizedTree::GetPhraseUnderMouseClick(int iX, int iY, QString &sResult)
{
    for (QMap<long long int, TVisualizedWordInfo>::iterator iter=m_mInfoMap.begin(); iter != m_mInfoMap.end(); iter++)
    {
	if (iter.value().dLeft < iX && (iter.value().dLeft + iter.value().iWidth > iX) &&
		iter.value().dTop < iY && (iter.value().dTop + iter.value().iHeight > iY))
	{
	    sResult = iter.value().sPhrase;

	    // this is a hack to remove the wrong space before commas
	    for (int i=1; i < sResult.length(); i++)
	    {
		if (sResult.at(i) == ',' && sResult.at(i-1) == ' ')
		    sResult = sResult.remove(i - 1, 1);
	    }
	    return true;
	}
    }

    return false;
}
