/*
 * header file for SearchHistory class
 * Original by: christian moellinger <ch.moellinger@gmail.com>
 * Modified by: Chung-Yeon Lee <cylee@bi.snu.ac.kr> and Beom-Jin Lee <bjlee@bi.snu.ac.kr>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 * 11/2013 - 12/2013 Project "InfoVis: Word Tree ++"
 */

#ifndef SEARCHHISTORY_H
#define SEARCHHISTORY_H

#include <QDialog>
#include <map>

class WTVisualizedTree;
class QListWidgetItem;

namespace Ui {
    class SearchHistory;
}

class SearchHistory : public QDialog
{
    Q_OBJECT

public:
    /*! \name Construction / Destruction */
    //@{
	/// Constructor
	explicit SearchHistory(QWidget *parent = 0);

	/// Destructor
	~SearchHistory();
    //@}

    /*! \name Methods */
    //@{
	/// adds a new search to the list and stores the result tree
	void AddNewSearch(QString sSearchTerm, QSharedPointer<WTVisualizedTree> spVisualizedTree);
    //@}

    /*! \name Methods */
    //@{
	/// returns the last search to the reference parameters and true as retun value if successfull, else false
	bool GetLastSearch(std::string &rsSearchPhrase, QSharedPointer<WTVisualizedTree> &rspVisualizedTree);
    //@}

signals:
    /*! \name Signals */
    //@{
	/// emitted when a new search is requested (e.g. by clicking on a entry in the search window)
	void EmitSearch(QString sSearchTerm);
    //@}

private slots:
    /*! \name Private slots */
    //@{
	/// handles the click on an item of the list
	void HandleItemClicked(QListWidgetItem * pClickedItem);

	/// clears the history
	void ClearHistory();
    //@}

private:
    /*! \name Private members */
    //@{
	Ui::SearchHistory *ui;

	std::string m_sLastSearchPhrase;
	QSharedPointer<WTVisualizedTree> m_spLastAddedTree;
	std::map<QString, QSharedPointer<WTVisualizedTree> > m_mAlreadyVisualizedTrees;
    //@}
};

#endif // SEARCHHISTORY_H
