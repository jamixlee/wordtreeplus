/*
 * header file for MainWindow class
 * Original by: christian moellinger <ch.moellinger@gmail.com>
 * Modified by: Chung-Yeon Lee <cylee@bi.snu.ac.kr> and Beom-Jin Lee <bjlee@bi.snu.ac.kr>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 * 11/2013 - 12/2013 Project "InfoVis: Word Tree ++"
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QGraphicsScene>

class SearchHistory;
class WTTreeVisualizer;
class WTBackend;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*! \name Constructor / Destructor */
    //@{
	/// Constructor
	explicit MainWindow(QWidget *parent = 0);
    QList< int > Total_index;
    bool imhere[1908];
	/// Destructor
	~MainWindow();
    //@}

    /*! \name Setter */
    //@{
	/// sets the search history window
	void SetSearchHistoryWindow(SearchHistory *pSearchHistory);
    //@}
    void FindWordIndex(QString search_phrase);
protected:
    /*! \name Overrided events */
    //@{
	/// handles close event
	void closeEvent(QCloseEvent *event);
    //@}

private slots:
    /*! \name public slots */
    //@{
	/// provides opening dialog and loads a file
	void LoadFile();

	/// searches for a phrase and visualizes the word tree
	void Search(QString search_phrase);

	/// handles changes in the search box (and sometimes, calls Search() )
	void HandleSearchBoxValueChanged(QString sValue);

	/// handles clicking on the tree
	void HandleClickOnTree(int iX, int iY, bool bControlPressed);

	/// handles clicking on "create word tree" button
	void ForceSearch();

    /// handles changes of the check box
    void on_checkBox_stateChanged(int arg1);

	/*
	/// handles clicking on "enable opengl"
	void HandleEnableOpenGL();

	/// handles clicking on "disable opengl"
	void HandleDisableOpenGL();
	*/

	/// quits the program
	void Quit();
    //@}

private:
    /*! \name Member variables */
    //@{
	/// the ui
	Ui::MainWindow *ui;

	/// ptr to the search history window
	SearchHistory *m_pSearchHistory;

	/// ptr to the tree visualizer
	WTTreeVisualizer *m_pTreeVisualizer;

    //QGraphicsView *m_pGraphicsView2;
    //@}
};

#endif // MAINWINDOW_H
