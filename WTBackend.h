/*
 * header file for WTBackend class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */

#ifndef WTBACKEND_H
#define WTBACKEND_H

#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>
#include <QMap>

class WTVisualizedTree;

class WTBackend : public QObject
{
    Q_OBJECT

public:
    /*! \name Public types */
    //@{
    //int filterCheck = 0;
	/// a coded phrase, which stores the whole phrase,
	/// a list of all words, and for each word an unique code
	struct TCodedPhrase
	{
	    QStringList m_lsWords;
	    QVector<long long int> m_vCodesForWords;
	};
    struct Initial_vis_data_set
    {
        QStringList Words;
        long long int counter_val;
    };
    //@}

    //@{
    QString initWord;
    //@}

    /*! \name Constructor / Destructor */
    //@{
	/// Constructor
	explicit WTBackend(QObject *parent = 0);
    //@}

    /*! \name Public methods */
    //@{
	/// This method creates a word tree, which can be visualized with WTTreeVisualizer
	QSharedPointer<WTVisualizedTree> CreateWordTree(QString sSearchPhrase);
    //@}

public slots:
    /*! \name Public slots */
    //@{
	/// this method loads a given file
    void LoadFile(QString sFilename, bool filtered);
    //@}

private:
    /*! \name Private types */
    //@{
	/// this class represents a node in the word tree
	class TItlNode
	{
        public:
            /*! \name Public attributes */
            //@{
            /// returns the word list
            QStringList GetWordList() { return m_lsWords; }

            /// returns the number of children
            int NumChildren() const { return m_vChildren.size(); }

            /// returns a vector with the children
            QVector<QSharedPointer<TItlNode> > GetChildren() { return m_vChildren; }
            //@}

            /*! \name Public methods */
            //@{
            /// adds a new child
            void AddChild(QSharedPointer<TItlNode> spNode) { m_vChildren.push_back(spNode); }
            //@}


            /*! \name Public variables, should be moved to private */
            //@{
            QStringList m_lsWords;
            QString m_sPhrase;
            QMap<int, QVector<long long int> > m_mCodesForWords;
            //@}

        private:
            /*! \name Private member variables */
            //@{
            QVector<QSharedPointer<TItlNode> > m_vChildren;
            //@}
	};
    //@}

    /*! \name Internal methods */
    //@{
	/// this method creates a word list of the given phrase
	QStringList ItlCreateWordList(QString sPhrase);
    void Initial_visual();
	/// this method recursively draws a given node in the given visualized tree object
	void ItlDrawNode(int iStartX,
		  int iStartY,
		  int &rUsedHeight,
		  QSharedPointer<WTBackend::TItlNode> spRootNode,
		  QSharedPointer<WTVisualizedTree> spVisualizedTree);

	/// this method generates a phrase tree, that means a data structure with connected nodes
	QSharedPointer<WTBackend::TItlNode> ItlCreatePhraseTree(QVector<TCodedPhrase> lPhrases);

	/// this medhod recursively generates a node from the given phrases
	QSharedPointer<TItlNode> ItlCreateNode(QVector<TCodedPhrase> lCodedPhrases, int iPosition);
    //@}

    /*! \name Internal member variables */
    //@{
	/// the coded phrases of the currently loaded file
	QVector<TCodedPhrase> m_vCodedPhrases;
    //@}
};

#endif // WTBACKEND_H
