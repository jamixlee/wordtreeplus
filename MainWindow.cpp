/*
 * source file for MainWindow class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */

// qt-gui includes
#include "ui_MainWindow.h"

// qt includes
#include <QScrollBar>
#include <QFileDialog>
#include <QCloseEvent>
//#include <QtOpenGL>

// project includes
#include "MainWindow.h"
#include "WTBackend.h"
#include "WTTreeVisualizer.h"
#include "SearchHistory.h"

//WTBackend rBackend;
WTBackend *pCurrentBackend = 0;

// filename backup (CYLEE)
QString fName;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_pSearchHistory = NULL;
    m_pTreeVisualizer = NULL;

    ui->setupUi(this);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(LoadFile()));

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(Quit()));
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(HandleSearchBoxValueChanged(QString)));
    connect(ui->graphicsView, SIGNAL(OnClicked(int,int, bool)), this, SLOT(HandleClickOnTree(int,int, bool)));


  //  ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::DoubleBuffer)));
  //  ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

 //   connect(ui->actionEnable_OpenGL, SIGNAL(triggered()), this, SLOT(HandleEnableOpenGL()));
 //   connect(ui->actionDisable_OpenGL, SIGNAL(triggered()), this, SLOT(HandleDisableOpenGL()));

    m_pTreeVisualizer = new WTTreeVisualizer();
    m_pTreeVisualizer->SetGraphicsView(ui->graphicsView);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pTreeVisualizer;
}

void MainWindow::LoadFile()
{
    QString filename = QFileDialog::getOpenFileName();

    if (filename.isNull() == false)
    {
        if (pCurrentBackend != NULL)
            delete pCurrentBackend;

        pCurrentBackend = new WTBackend();

        fName = filename;   // 파일 이름 백업하자, 필터링 체크하고 Reload할 때 필요하니깐

        pCurrentBackend->LoadFile(filename, false);
    }

}

void MainWindow::Search(QString search_phrase)
{
    QSharedPointer<WTVisualizedTree> spNewTree = pCurrentBackend->CreateWordTree(search_phrase);

    m_pSearchHistory->AddNewSearch(search_phrase, spNewTree);

    m_pTreeVisualizer->VisualizeWordTree(spNewTree);
}

void MainWindow::ForceSearch()
{
    QString sValue = ui->lineEdit->text();

    if (sValue.length() > 0)
	    Search(sValue);
}


void MainWindow::HandleSearchBoxValueChanged(QString sValue)
{
    if (sValue.length() > 0 && sValue.at(sValue.length() - 1) == ' ')
    {
	    Search(sValue);
    }
}

void MainWindow::HandleClickOnTree(int iX, int iY, bool bControlPressed)
{
    QSharedPointer<WTVisualizedTree> spTree;
    std::string sSearchPhrase;

    int iScrolledY = ui->graphicsView->verticalScrollBar()->value();
    iY += iScrolledY;

    int iScrolledX = ui->graphicsView->horizontalScrollBar()->value();
    iX += iScrolledX;

    bool bOk = m_pSearchHistory->GetLastSearch(sSearchPhrase, spTree);

    if (bOk)
    {
        QString sWord;

        bool bOk2 = false;

        if (bControlPressed)
            bOk2 = spTree->GetWordUnderMouseClick(iX, iY, sWord);
        else
            bOk2 = spTree->GetPhraseUnderMouseClick(iX, iY, sWord);

        if (bOk2)
        {
            Search(sWord);
        }
    }
}

void MainWindow::SetSearchHistoryWindow(SearchHistory *pSearchHistory)
{
    Q_ASSERT (m_pSearchHistory == NULL);

    //set ptr to search history window
    m_pSearchHistory = pSearchHistory;

    Q_ASSERT (m_pSearchHistory != NULL);

    connect(ui->openSearchHistoryButton, SIGNAL(clicked()), m_pSearchHistory, SLOT(show()));
    connect(m_pSearchHistory, SIGNAL(EmitSearch(QString)), this, SLOT(Search(QString)));
    connect(ui->actionOpen, SIGNAL(triggered()), m_pSearchHistory, SLOT(ClearHistory()));
    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_stateChanged(int arg1)));
}

void MainWindow::Quit()
{
    // hide the graphics view
    ui->graphicsView->hide();

    // delete the tree visualizer
    delete m_pTreeVisualizer;
    m_pTreeVisualizer = NULL;
    delete pCurrentBackend;

    // close
    close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // also close search history
    m_pSearchHistory->close();

    // always accept event
    event->accept();
}

/*
void MainWindow::HandleEnableOpenGL()
{
    ui->actionDisable_OpenGL->setEnabled(true);
    ui->actionEnable_OpenGL->setEnabled(false);

    ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->graphicsView->update();
}


void MainWindow::HandleDisableOpenGL()
{
    ui->actionDisable_OpenGL->setEnabled(false);
    ui->actionEnable_OpenGL->setEnabled(true);

    ui->graphicsView->setViewport(0);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->graphicsView->update();
}
*/

void MainWindow::on_checkBox_stateChanged(int arg1) //TRUE: 2 / FALSE: 0
{
    if (ui->checkBox->isChecked())
    {
        if (pCurrentBackend != NULL)
            delete pCurrentBackend;
        pCurrentBackend = new WTBackend();
        pCurrentBackend->LoadFile(fName, true);
        ForceSearch();
        //this->repaint();
        this->show();
    }
    else
    {
        if (pCurrentBackend != NULL)
            delete pCurrentBackend;
        pCurrentBackend = new WTBackend();
        pCurrentBackend->LoadFile(fName, false);
        ForceSearch();
        this->repaint();
    }
}
