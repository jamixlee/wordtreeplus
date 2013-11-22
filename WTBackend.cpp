/*
 * source file for WTBackend class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */

// c++ includes
#include <cmath>
#include <iostream>

// QT includes
#include <QFile>
#include <QList>
#include <QLinkedList>
#include <QByteArray>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

// project includes
#include "WTBackend.h"
#include "WTVisualizedTree.h"

const int iWidthOffset = 20;
const bool bFilter = off;

WTBackend::WTBackend(QObject *parent) :
    QObject(parent)
{
    // nothing to do so far
}


void WTBackend::ItlDrawNode(int iStartX,
            int iStartY,
            int &rUsedHeight,
            QSharedPointer<WTBackend::TItlNode> spRootNode,
            QSharedPointer<WTVisualizedTree> spVisualizedTree)
{
    // get word frequency
    int iFrequencyOfWord = spRootNode->m_mCodesForWords[0].size();
    double dSquaredRootOfFrequency = std::sqrt((double) iFrequencyOfWord);

    // set font size
    QFont font;
    font.setPixelSize(9 * dSquaredRootOfFrequency);

    // we need a scene to determinate the width and height of drawed items
    QGraphicsScene rTestScene;

    // get word list
    QStringList lsWords = spRootNode->GetWordList();

    // we need to calculate the used width for this node
    int iUsedWidth = 0;

    // we need to calculate the used height for this node
    int iUsedHeightNode = 0;

    // we need a array of widths (to store the width for each word)
    QVector<int> vUsedWidthForWord;

    // we need a array of heights (to store the width for each word)
    QVector<int> vUsedHeightForWord;

    // this vector store the summed up widths, that means vSummedUpWidths[i] = sum(vUsedWidthForWord[0] + ... + vUsedWidthForWord[i-1])
    QVector<int> vSummedUpWidths;

    // this vector stores the end points of the connecting lines
    QVector<QPoint> vLineEndPoints;

    // now render each word
    for (int i=0; i < lsWords.size(); i++)
    {
    // create new text item
    QGraphicsTextItem *pNewTextItem = rTestScene.addText(QString(lsWords.at(i)), font);

    // store summed with (summed width without the width of the current one)
    vSummedUpWidths.push_back(iUsedWidth);

    // store used width and height
    vUsedWidthForWord.push_back(pNewTextItem->boundingRect().width());
    vUsedHeightForWord.push_back(pNewTextItem->boundingRect().height());

    // add used width fot this word to overall used width
    iUsedWidth += vUsedWidthForWord[i];

    // set used height to height of that word
    // TODO: use max()?
    iUsedHeightNode = vUsedHeightForWord[i];

    // remove item from scene, we don't need it anymore
    rTestScene.removeItem(pNewTextItem);

    // delete item
    delete pNewTextItem;
    pNewTextItem = NULL;
    }

    // make sure that we have all items
    Q_ASSERT(rTestScene.items().size() == 0);

    // get the number of children of this node
    int iChildren = spRootNode->NumChildren();

    int iWidthOfTextNode = iUsedWidth;
    int iUsedHeightForChildren = 0;

    // now draw the children (recursively call)
    for (int i=0; i < iChildren; i++)
    {
    // reserve variable which is set by
    int iUsedHeight = 0;

    // draw child node, set position
    // !! this method writes the used height in iUsedHeight !!
    ItlDrawNode(iStartX + iWidthOfTextNode + iWidthOffset,
          iStartY + iUsedHeightForChildren,
          iUsedHeight,
          spRootNode->GetChildren().at(i),
          spVisualizedTree);

    // store line end point, if any child was drawed
    if (iUsedHeight > 0)
    {
        // calculate x-coordinate of line end point
        int iLineEndX = iStartX + iWidthOfTextNode + iWidthOffset;

        // calculate y-coordinate of line end point
        int iLineEndY = iStartY + iUsedHeightForChildren + iUsedHeight * 0.5;

        // add line end point to vector
        vLineEndPoints.push_back(QPoint(iLineEndX, iLineEndY));
    }

    // add used height for that child to used height for the current node
    iUsedHeightForChildren += iUsedHeight;
    }

    // now loop through all rendered words, and store the info in the "visualized tree"-object
    for (int i=0; i < lsWords.size(); i++)
    {
    Q_ASSERT (spRootNode->m_mCodesForWords.size() == lsWords.size());

    // now we set the phrase together and start with the phrase of this node ...
    QString sPhrase(spRootNode->m_sPhrase);

    // ... and add the phrases of the children
    for (int j=0; j <=i; j++)
    {
        if (sPhrase.length() > 0)
        sPhrase.append(" ");
        sPhrase.append(lsWords.at(j));
    }

    // calculate starting points of connecting line
    int iLineStartPointX = iStartX + vSummedUpWidths[i];
    int iLineStartPointY = -1;

    if (iUsedHeightForChildren > 0)
        iLineStartPointY = iStartY + iUsedHeightForChildren / 2 - iUsedHeightNode / 2;
    else
        iLineStartPointY = iStartY;

    Q_ASSERT(iLineStartPointY != -1);

    // add word to "visualized tree"-object
    spVisualizedTree->AddRenderedWord(spRootNode->m_mCodesForWords[i],
                          lsWords.at(i),
                          sPhrase,
                          iLineStartPointX,
                          iLineStartPointY,
                          vUsedWidthForWord[i],
                          vUsedHeightForWord[i],
                          font.pixelSize());
    }

    // finally, add the connecting lines to the "visualized tree"-object
    for (QVector<QPoint>::iterator iter = vLineEndPoints.begin(); iter != vLineEndPoints.end(); iter++)
    {
    spVisualizedTree->AddRenderedLine(iStartX + iWidthOfTextNode,
                      iStartY + iUsedHeightForChildren * 0.5,
                      iter->x(),
                      iter->y(),
                      0,
                      0);
    }

    // return the used height
    rUsedHeight = std::max(iUsedHeightNode, iUsedHeightForChildren);
}

void WTBackend::LoadFile(QString filename)
{
    QFile file(filename);

    // open file
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray baFullText;

    // read in file contents in byte array
    baFullText = file.readAll();

    // close file
    file.close();

    // we use a stl-list instead of QList because the STL-list provides a method to sort
    std::list<QByteArray> lAllSubPhrases;   /// 요놈이 서브프레이즈 다 가지고 있는 stl-list

    int iCurrentPos=0;
    int iLength = baFullText.length();

    // now go through the text and split them up in phrases;
    // take '.' as split pattern

    QList<Initial_vis_data_set> count_all_phrase;

    while (iCurrentPos < iLength) // 문서 끝까지 loop
    {
        // search the next occurence of '.'
        int iNextPoint = iCurrentPos;

        // 쩜 아니면 스킵
        while (baFullText[iNextPoint] != '.' && iNextPoint < baFullText.length())
            iNextPoint++;

        // get current phrase
        // .mid 요놈은 시작 위치(iCurrentPos)부터 쩜 나오는 위치까지의 byte array를 리턴 (즉, 짤라서 가져옴)
        QByteArray baCurrentPhrase = baFullText.mid(iCurrentPos, iNextPoint - iCurrentPos);


        /* 여기서부터는 서브프레이즈 생성 */
        // now build all subphrases, that means take each word in the phrase as starting point
        // (e.g.: "that is cool" -> "that is cool", "is cool", "cool")

        // 먼저 현재 문장을 공백 기준으로 잘라서 단어 목록으로 저장하고
        QList<QByteArray> lSplittedPhrase = baCurrentPhrase.split(' ');
        int iSplittedSize = lSplittedPhrase.size(); // 몇개 단어인지를 세어서 아래 루프에서 limit으로 사용

        //범진 추가
        for (int iStartingWord=0; iStartingWord < iSplittedSize; iStartingWord++)
        {
            long long int size_count_all_phrase = count_all_phrase.size();
            for(int i =0 ; i < size_count_all_phrase; i++)
            {
            }
        }

        // loop starting word from 0..LastWord
        for (int iStartingWord=0; iStartingWord < iSplittedSize; iStartingWord++)
        {
            QByteArray baSubPhrase;

            // 맨 앞 단어를 하나씩 스킵하면서 서브프레이즈로 저장
            // add all later words of the phrase
            for (int k=iStartingWord; k < iSplittedSize; k++)
            {
                // 앞뒤 공백 없앤 단어를 차곡차곡 집어넣음
                baSubPhrase.append(lSplittedPhrase[k].trimmed());

                // 방금 집어넣은 단어 뒤에 공백을 하나 넣음 (마지막 놈 빼고)
                if (k < iSplittedSize - 1)
                    baSubPhrase.append(" ");
            }

            // write sub phrase in list of all subphrases (합치기)
            lAllSubPhrases.push_back(baSubPhrase.trimmed());
        }

        // set position forward
        iCurrentPos = iNextPoint+1;
    }


    // remove unwanted chars (new line, char(10) and char(13))
    // char(10)은 line feed, char(13)은 carriage return, 결국 모두 new line
    // 혹시 탭이 있을 수 있으므로, char(9)도 검사해서 빼야할 듯...
    for (std::list<QByteArray>::iterator iter = lAllSubPhrases.begin(); iter != lAllSubPhrases.end(); iter++)
    {
        QByteArray *pCurrentPhrase = &(*iter);

        /// 관사, 부사, 조사 같은거 필터링할 부분은 여기임 (충연)
        ///
        ///
        if (bFilter)
        {
            for (int i=pCurrentPhrase->length() - 1; i >= 0; i--)
            {
                if (pCurrentPhrase->at(i) == (char) 13 ||
                    pCurrentPhrase->at(i) == (char) 10)
                    pCurrentPhrase->remove(i,1);
            }
        }
        else
        {
            for (int i=pCurrentPhrase->length() - 1; i >= 0; i--)
            {
                if (pCurrentPhrase->at(i) == (char) 13 ||
                    pCurrentPhrase->at(i) == (char) 10)
                    pCurrentPhrase->remove(i,1);
            }
        }
    }


    // sort list with all subphrases (정렬)
    lAllSubPhrases.sort();

    /*for (std::list<QByteArray>::iterator iter = lAllSubPhrases.begin(); iter != lAllSubPhrases.end(); iter++)
    {
    std::cout << (*iter).data() << std::endl;
    }*/


    // generate coded phrases - each word gets an unique code
    // 얘네들한테도 유니크 코드 부여
    long long int iCode = 0;

    for (std::list<QByteArray>::iterator iter = lAllSubPhrases.begin(); iter != lAllSubPhrases.end(); iter++)
    {
        // get phrase
        QString sPhrase(*iter);

        // create new coded phrase
        TCodedPhrase tNewCodedPhrase;

        // set lsWords
        tNewCodedPhrase.m_lsWords = ItlCreateWordList(sPhrase);

        // generate code for each word
        for(int i=0; i < tNewCodedPhrase.m_lsWords.size(); i++)
        {
            // new codes: integer 하나씩 늘려가면서 순서대로 코드 부여
            tNewCodedPhrase.m_vCodesForWords.push_back(iCode++);
        }

        // store new coded phrase in vector
        m_vCodedPhrases.push_back(tNewCodedPhrase);
    }
    //q_assert 는 자바의 test catch 같은거
    Q_ASSERT (m_vCodedPhrases.size() == lAllSubPhrases.size());
}

void WTBackend::Initial_visual()
{
    QVector<TCodedPhrase> m_vCodedPhrases;
}

/* This method creates a word tree, which can be visualized with 'WTTreeVisualizer' */
QSharedPointer<WTVisualizedTree> WTBackend::CreateWordTree(QString sSearchPhrase)
{
    QStringList lSearchedWords = ItlCreateWordList(sSearchPhrase);

    QVector<TCodedPhrase> vMatchedPhrases;

    for (QVector<WTBackend::TCodedPhrase>::iterator iter=m_vCodedPhrases.begin(); iter != m_vCodedPhrases.end(); iter++)
    {
        int i=0;

        for (i=0; i < lSearchedWords.size(); i++)
        {
        if (iter->m_lsWords.size() <= i || lSearchedWords.at(i).compare(iter->m_lsWords.at(i)) != 0)
            break;
        }

        if (i == lSearchedWords.size())
        {
        /*for (int j=0; j < iter->m_lsWords.size(); j++)
            std::cout << iter->m_lsWords[j].toStdString() << " ";

        std::cout << std::endl;*/

        vMatchedPhrases.push_back(*iter);
        }

    }

    // if any phrase matched, create a tree
    if (vMatchedPhrases.size() > 0)
    {
    // create a phrase tree
    QSharedPointer<WTBackend::TItlNode> pRootNode = ItlCreatePhraseTree(vMatchedPhrases);

    // create a (initially empty) "visualized tree" object
    QSharedPointer<WTVisualizedTree> spNewWordTree(new WTVisualizedTree(sSearchPhrase));

    int iUsedHeight;

    // draw nodes and add it to the visualized tree
    ItlDrawNode(0, 0, iUsedHeight, pRootNode, spNewWordTree);

    // return visualized tree
    return spNewWordTree;
    }
    else
    // return empty tree
    return QSharedPointer<WTVisualizedTree>(new WTVisualizedTree(sSearchPhrase));


}



QSharedPointer<WTBackend::TItlNode> WTBackend::ItlCreateNode(QVector<WTBackend::TCodedPhrase> lCodedPhrases, int iPosition)
{
    Q_ASSERT(lCodedPhrases.size() > 0);

    // create a new node
    QSharedPointer<TItlNode> spNewNode (new TItlNode());

    // first, we need to know how many words the longest phrase has
    int iMaxWords = 0;

    for (QVector<WTBackend::TCodedPhrase>::iterator iter = lCodedPhrases.begin(); iter != lCodedPhrases.end(); iter++)
    {
    QStringList *plWordList = &(iter->m_lsWords);
    if (plWordList->size() > iMaxWords)
        iMaxWords = plWordList->size();
    }

    Q_ASSERT(iMaxWords > 0);

    // create a temporary list of coded phrases for all fitting phrases
    QVector<WTBackend::TCodedPhrase> vTempList;

    //go through all words
    for (int i = iPosition; i < iMaxWords; i++)
    {
        // the magic word list (the list where the other phrases should match)
        QStringList *plMagicWordList = NULL;

        // go through all phrases, and test if the current word (outer loop) is the same
        for (QVector<WTBackend::TCodedPhrase>::iterator iter = lCodedPhrases.begin(); iter != lCodedPhrases.end(); iter++)
        {
            // some helper ptrs, only because direct use of iterators seems to be too complicated for my IDE
            // which refuses to provide things like code completion or other help
            WTBackend::TCodedPhrase *pCurrentCodedPhrase = &(*iter);
            QStringList *plCurrentWordList = &(iter->m_lsWords);

            //if the magic word list is not set, the word list of the current phrase is used
            if (plMagicWordList == NULL)
            {
                plMagicWordList = plCurrentWordList;
                vTempList.clear();
                vTempList.push_back(*pCurrentCodedPhrase);
            }
            else
            // otherwise, we compare if the current phrase has the same word at position i as the
            // magic word list has
            {
                Q_ASSERT(plMagicWordList != NULL);

                if ((plCurrentWordList->size() <= i) ||
                    (plMagicWordList->size() <= i) ||
                    (plMagicWordList->at(i).compare(plCurrentWordList->at(i)) != 0))
                {
                    // if NOT, we use the current TempList (if not empty) to create a new child node ...
                    if (!vTempList.empty())
                    {
                        spNewNode->AddChild(ItlCreateNode(vTempList, i));
                        vTempList.clear();
                    }

                    // ... and use the current word list as new magic word list
                    plMagicWordList = plCurrentWordList;

                    Q_ASSERT(vTempList.empty() == true);

                    // and add the current phrase to the temp list
                    vTempList.push_back(*pCurrentCodedPhrase);
                }
                else
                {
                    // if the word matches, we only add the current phrase to the temp list
                    vTempList.push_back(*pCurrentCodedPhrase);
                }
            }
        }

        // make sure that temp list is not longer than list with coded phrases :-)
        Q_ASSERT(vTempList.size() <= lCodedPhrases.size());

        // if there are some entries in the temp list but not all,
        // we must create another child node
        if (vTempList.empty() == false && vTempList.size() != lCodedPhrases.size())
        {
            // create new child node with entries of temp list
            spNewNode->AddChild(ItlCreateNode(vTempList, i));

            // clear temp list
            vTempList.clear();

            // we stop the loop because we splitted up our tree already
            break;
        }
        else
        {
            // if all phrases matched the current word, we add this word to the current node

            // add word
            spNewNode->m_lsWords.push_back(plMagicWordList->at(i));

            // clear sPhrase of node ...
            spNewNode->m_sPhrase.clear();

            // ... and add all words 0..iPosition
            for (int j=0; j < iPosition; j++)
            {
                if (j>0)
                    spNewNode->m_sPhrase.append(" ");

                spNewNode->m_sPhrase.append(plMagicWordList->at(j));
            }

            // get the position of the last word in the lsWords-List
            int iLastWordPosition = spNewNode->m_lsWords.size() - 1;

            // now collect the codes of the words which are collapsed to one word
            for (QVector<WTBackend::TCodedPhrase>::iterator iter = lCodedPhrases.begin(); iter != lCodedPhrases.end(); iter++)
            {
                Q_ASSERT (iter->m_vCodesForWords.size() > i);

                spNewNode->m_mCodesForWords[iLastWordPosition].push_back(iter->m_vCodesForWords[i]);
            }

            Q_ASSERT(spNewNode->m_lsWords.size() == spNewNode->m_mCodesForWords.size());
            Q_ASSERT (spNewNode->m_mCodesForWords[iLastWordPosition].size() == lCodedPhrases.size());
        }
    }

    // return created node
    return spNewNode;
}


QSharedPointer<WTBackend::TItlNode> WTBackend::ItlCreatePhraseTree(QVector<WTBackend::TCodedPhrase> lPhrases)
{
    Q_ASSERT(lPhrases.size() > 0);

    return ItlCreateNode(lPhrases, 0);
}


/* 검색 문장으로부터 단어 분리하여 저장 */
QStringList WTBackend::ItlCreateWordList(QString sPhrase)
{
    // get list of words (공백을 기준으로 자르고)
    QStringList lWords = sPhrase.split(" ", QString::SkipEmptyParts);

    // create linkedlist for manipulating word list (링크드리스트 형태로 저장, 아래 작업을 원활하게 하기 위해?)
    QLinkedList<QString> lTempWords = QLinkedList<QString>::fromStdList(lWords.toStdList());

    // make sure the lists are equal
    Q_ASSERT(lTempWords.size() == lWords.size());

    // search for commas and create a new word from each of it
    // 콤마도 따로 나눠서 저장, 공백으로 분리가 안되었으므로 지금 처리함
    for (QLinkedList<QString>::iterator iter=lTempWords.begin(); iter != lTempWords.end(); iter++)
    {
        if (iter->length() > 1)
        {
            if (iter->at(iter->length()-1) == ',')
            {
                *iter = iter->mid(0, iter->length() - 1);
                lTempWords.insert(iter+1, QString(","));
            }
        }
    }
    // 이걸 다시 StringList 형태로 바꿔서 리턴
    QStringList lReturn = QList<QString>::fromStdList(lTempWords.toStdList());

    return lReturn;
}
