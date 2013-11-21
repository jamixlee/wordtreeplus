/*
 * source file for SearchHistory class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */

// qt-gui includes
#include "ui_SearchHistory.h"

// qt includes
#include <QList>
#include <QListWidget>

// project includes
#include "SearchHistory.h"
#include "WTVisualizedTree.h"

SearchHistory::SearchHistory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchHistory)
{
    ui->setupUi(this);


    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(HandleItemClicked(QListWidgetItem*)));
}

SearchHistory::~SearchHistory()
{
    delete ui;
}

void SearchHistory::AddNewSearch(QString sSearchTerm, QSharedPointer<WTVisualizedTree> spVisualizedTree)
{
    if (m_mAlreadyVisualizedTrees.find(sSearchTerm) == m_mAlreadyVisualizedTrees.end())
    {
	//add visualized tree to map
	m_mAlreadyVisualizedTrees[sSearchTerm] = spVisualizedTree;

	//add search term to item list
	ui->listWidget->addItem(QString("(") + sSearchTerm + QString(")"));
    }

    m_sLastSearchPhrase = sSearchTerm.toStdString();
    m_spLastAddedTree = spVisualizedTree;
}

void SearchHistory::HandleItemClicked(QListWidgetItem * pClickedItem)
{
    QString sClickedSearchTerm = pClickedItem->text().mid(1, pClickedItem->text().length() - 2);

    Q_ASSERT(m_mAlreadyVisualizedTrees.find(sClickedSearchTerm) != m_mAlreadyVisualizedTrees.end());

    //QSharedPointer<WTVisualizedTree> spVisualizedTree = m_mAlreadyVisualizedTrees[sClickedSearchTerm];

    emit EmitSearch(sClickedSearchTerm);
}

bool SearchHistory::GetLastSearch(std::string &rsSearchPhrase, QSharedPointer<WTVisualizedTree> &rspVisualizedTree)
{
    if (m_mAlreadyVisualizedTrees.size() > 0)
    {
	rsSearchPhrase = m_sLastSearchPhrase;
	rspVisualizedTree = m_spLastAddedTree;

	return true;
    }
    else
	return false;
}

void SearchHistory::ClearHistory()
{
    ui->listWidget->clear();
}
