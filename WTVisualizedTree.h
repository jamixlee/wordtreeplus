/*
 * header file for WTVisualizedTree class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */

#ifndef WTVISUALIZEDTREE_H
#define WTVISUALIZEDTREE_H

#include <QVector>
#include <QMap>
#include <QString>

/*! This class represents a visualized tree.
    A visualized tree is an object which stores all the necessary information
    to draw the tree (words and lines together with their positions)
 */
class WTVisualizedTree
{
public:
    /*! \name Public types */
    //@{
	/// a struct which holds all necessary info about a word
	struct TVisualizedWordInfo
	{
	    double dLeft;
	    double dTop;
	    int iWidth;
	    int iHeight;
	    int iFontSize;

	    QString sWord;
	    QString sPhrase;

	    bool bIsFirstElement;
	};

	/// a struct which holds all necessary info about a connection line
	struct TVisualizedLineInfo
	{
	    double dX1, dY1;
	    double dX2, dY2;

	    /// the code of the word where the line begins
	    long long int lStartWord;

	    /// the code of the word where the line ends
	    long long int lEndWord;
	};
    //@}

    /*! \name Construction / Destruction */
    //@{
	/// Constructor
	WTVisualizedTree(QString sSearchPhrase);
    //@}


    /*! \name Public methods */
    //@{
	/// this method adds a word to the visualized tree
	void AddRenderedWord(QVector<long long int> vWordIds,
			    QString sWord,
			    QString sPhrase,
			    double dLeft,
			    double dTop,
			    int iWidth,
			    int iHeight,
			    int iFontSize);

	/// this method adds a connection line to the visualized tree
	void AddRenderedLine(double dX1,
			     double dY1,
			     double dX2,
			     double dY2,
			     long long int lStartChar,
			     long long int lEndChar);
    //@}


    /*! \name Public attributes */
    //@{
	/// this method returns the connection lines
	QVector<TVisualizedLineInfo> GetRenderedLines() { return m_vRenderedLines; };

	/// this method returns a info map with the word code as keys,
	/// for each existing word in the tree the word info is returned
	/// (even if some words of the input file are rendered as the same
	/// word in the tree, that's why the words are accessed by its code)
	QMap<long long int, TVisualizedWordInfo>	*GetInfoMap() { return &m_mInfoMap; };

	/// writes the word under the mouse click to sResult and "true" as return value if there
	/// is any, else it returns false
	bool GetWordUnderMouseClick(int iX, int iY, QString &sResult);

	/// writes the phrase under the mouse click to sResult and "true" as return value if there
	/// is any, else it returns false
	bool GetPhraseUnderMouseClick(int iX, int iY, QString &sResult);

	/// returns the phrase which was used to generate this tree
	QString GetSearchPhrase() { return m_sSearchPhrase; };
    //@}

private:
    /*! \name Private member variables */
    //@{
	QMap<long long int, TVisualizedWordInfo>	m_mInfoMap;
	QVector<TVisualizedLineInfo> m_vRenderedLines;
	QString m_sSearchPhrase;
    //@}

};

#endif // WTVISUALIZEDTREE_H
