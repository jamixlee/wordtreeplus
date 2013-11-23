/*
 * source file for MainWindow class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 03/2011 - 06/2011 Project "InfoVis: Word Tree"
 */

// qt-gui includes
#include "ui_MainWindow.h"
#include <QGraphicsView>

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
QString hfWord;

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

        // 불러온 문서내 최고빈도수를 가지는 단어로 그래프 출력

        // 출력
        /*
        if (m_pTreeVisualizer != NULL)
            delete m_pTreeVisualizer;
        m_pTreeVisualizer = new WTTreeVisualizer();
        m_pTreeVisualizer->SetGraphicsView(ui->graphicsView);
        */
        // pCurrentBackend->Total_SplittedPhrase;
        //Total_index

        Search(pCurrentBackend->initWord);
        //Search("Harry ");
    }
}
void MainWindow::FindWordIndex(QString search_phrase)
{
    if (Total_index.size() > 0)
        Total_index.clear();

    int total_size = pCurrentBackend->Total_SplittedPhrase.size(); //모든 phrase별로 다 담고 변수 사이즈 즉 sentence 총갯수
    int complete_size = 0; // 단어 총갯수 즉 위치 찾기용 +- 1차이는 별 지장이 없을듯 overview이기 때문에

    for (int i =0; i<total_size; i++) // 총 sentecne 수에서 1문장씩 뒤져보자
    {
        QList<QByteArray> Temp_Phrase; // 임시 sentence 담을 곳
        Temp_Phrase.append(pCurrentBackend->Total_SplittedPhrase[i]); //담아
         // sentence에 단어 갯수 저장
        for(int j = 0 ; j<Temp_Phrase.size();j++ ) // 단어 탐색
        {
             // 검색단어와 문장속 단어 일치하나 검사용 변수
            QString CMP_string(Temp_Phrase[j]);
            bool found_index = (search_phrase==CMP_string); // 단어 일치 검사
            if (found_index == 1) //일치를 찾았다3
            {
                int index_pos;
                index_pos = complete_size+j+1;
                Total_index.append(index_pos); //저장
            }
        }
        complete_size += Temp_Phrase.size();
    }
    double divide_ratio = 18*106;
    if (complete_size/divide_ratio>1)
    {
        divide_ratio = complete_size/divide_ratio;

        for(int i = 0;i<Total_index.size();i++)
        {
            Total_index[i] = Total_index[i]/divide_ratio;
            if (Total_index[i]<1)
                Total_index[i]=1;
            if (Total_index[i]>1908)
                Total_index[i]=1908;
        }
    }

    for (int i=0; i<1908; i++)
        imhere[i] = 0;

    for (int i=0; i<Total_index.size(); i++)
    {
        imhere[Total_index[i]] = 1;
    }
}

void MainWindow::Search(QString search_phrase)
{
    QSharedPointer<WTVisualizedTree> spNewTree = pCurrentBackend->CreateWordTree(search_phrase);

    m_pSearchHistory->AddNewSearch(search_phrase, spNewTree);

    m_pTreeVisualizer->VisualizeWordTree(spNewTree);

    FindWordIndex(search_phrase.trimmed());
    //FindWordIndex(spNewTree.data()->m_sSearchPhrase);

    /// 그림 그려보자
    // create a new temporary scene
    QGraphicsScene *pRightScene = new QGraphicsScene();

    // draw and show
    int margin = 10;
    int boxWidth = 10;
    int boxHeight = 5;
    int k=0;

    for (int i=0; i<106; i++)
    {
        for (int j=0; j<18; j++)
        {
            if (imhere[k])
                pRightScene->addRect(margin+(j*boxWidth), margin+(i*boxHeight), boxWidth, boxHeight, QPen(Qt::NoPen), QBrush(QColor("black")));
            else
                pRightScene->addRect(margin+(j*boxWidth), margin+(i*boxHeight), boxWidth, boxHeight, QPen(Qt::NoPen), QBrush(QColor("gray")));
            k++;
        }
    }

    // show
    if (ui->miniView->scene() != NULL)
        delete ui->miniView->scene();
    ui->miniView->setScene(pRightScene);
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

/// Filtering 체크박스 체크 여부에 따른 액션
void MainWindow::on_checkBox_stateChanged(int arg1) //TRUE: 2 / FALSE: 0
{
    //fName = "F:/InfoVis/_wordtree/wordtreeplus-git/build/debug/text files/test1.txt";

    if (ui->checkBox->isChecked())      // 필터링 사용(checked)
    {
        // 기존 트리 지우고 새로 생성, 이때 필터링함
        if (pCurrentBackend != NULL)
            delete pCurrentBackend;
        pCurrentBackend = new WTBackend();
        pCurrentBackend->LoadFile(fName, true);

        // 화면 초기화
        if (m_pTreeVisualizer != NULL)
            delete m_pTreeVisualizer;
        m_pTreeVisualizer = new WTTreeVisualizer();
        m_pTreeVisualizer->SetGraphicsView(ui->graphicsView);

        // 출력
        ForceSearch();
    }
    else                                // 필터링 미사용(unchecked)
    {
        if (pCurrentBackend != NULL)
            delete pCurrentBackend;
        pCurrentBackend = new WTBackend();
        pCurrentBackend->LoadFile(fName, false);

        if (m_pTreeVisualizer != NULL)
            delete m_pTreeVisualizer;
        m_pTreeVisualizer = new WTTreeVisualizer();
        m_pTreeVisualizer->SetGraphicsView(ui->graphicsView);

        ForceSearch();
    }
}
