#include <QtGui>
#include "cardmatch.h"
#include <QPushButton>
#include <QGridLayout> // manage the space
#include <QSignalMapper>
#include <QElapsedTimer>
#include <QTimer>
#include <QSize>
#include <QPalette>
#include <QBrush>
#include <QIcon>
#include <QFont>
#include <QFileDialog>
#include <QFileInfo>
#include <algorithm>
#include <random>
using namespace std;

vector<QString> word;
QTimer *timer;
QElapsedTimer et;
QString clkInfo;
vector<int> clkIndex;
vector<vector<int>> scoreInfo(100);
vector<vector<int>> timeInfo(100);
QSignalMapper *mapper;
QGridLayout *gridLayout;
bool pairs = false;
int finishedGame = 0;
int currPlace;
CardMatch::CardMatch(QWidget *parent)
    : QWidget(parent)
{
    QStringList list_ = QFileDialog::getOpenFileNames(this, "Select a file to open...", QDir::homePath());
    while(list_.size() != 15){
        list_.clear();
        list_ = QFileDialog::getOpenFileNames(this, "Select a file to open...", QDir::homePath());
    }
    et.start();
    timer = new QTimer(this);
    // keeps images' paths
    word = {list_[0], list_[1], list_[2], list_[3], list_[4], list_[5], list_[6], list_[7], list_[8],
            list_[9], list_[10], list_[11], list_[12], list_[13], list_[14],list_[0], list_[1], list_[2],
            list_[3], list_[4], list_[5], list_[6], list_[7], list_[8],list_[9], list_[10], list_[11],
            list_[12], list_[13], list_[14]};
    // shuffle the order
    shuffle(word.begin(), word.end(), default_random_engine(random_device()()));
    setupBoard();
}

CardMatch::~CardMatch()
{

}
/**
 * @brief CardMatch::setupBoard
 * Sets up the board for the first time
 */
void CardMatch::setupBoard()
{
    scoreInfo[0].push_back(0);
    timeInfo[0].push_back(0);
    int r = 0; // current row info
    gridLayout = new QGridLayout;
    lScore = new QLabel(tr("Score"));
    lP1 = new QLabel(tr("Player1"));
    lP1->setStyleSheet("QLabel { background-color : green; color : blue; }");
    lP2 = new QLabel(tr("Player2"));
    lP2->setStyleSheet("QLabel { background-color : red; color : blue; }");
    lGame = new QLabel(tr("Game No"));
    lGame_ = new QLabel(QString::number(gameNumber));
    lGame->setBuddy(lGame_);
    lHscore = new QLabel(tr("Highest Score"));
    lHscore_ = new QLabel(tr("0"));
    lHscore->setBuddy(lHscore_);
    lTime = new QLabel(tr("Time(secs)"));

    newGame = new QPushButton(tr("New Game"));
    connect(newGame, SIGNAL(clicked()), this, SLOT(newGameClicked()));
    prevRes = new QPushButton(tr("Prev. Result"));
    connect(prevRes, SIGNAL(clicked()), this, SLOT(prevResClicked()));
    nextRes = new QPushButton(tr("Next Results"));
    connect(nextRes, SIGNAL(clicked()), this, SLOT(nextResClicked()));
    currGame_ = new QPushButton(tr("Current Game"));
    connect(currGame_, SIGNAL(clicked()), this, SLOT(currGameClicked()));

    gridLayout->addWidget(lP1, r, 1);
    gridLayout->addWidget(lP2, r, 2);
    gridLayout->addWidget(lGame, r,3);
    gridLayout->addWidget(lGame_, r,4);
    gridLayout->addWidget(newGame, r,5);
    r++;

    lS1 = new QLabel("0");
    lS2 = new QLabel("0");
    lScore->setBuddy(lS1);
    lScore->setBuddy(lS2);

    lT1 = new QLabel("0");
    lT2 = new QLabel("0");
    lTime->setBuddy(lT1);
    lTime->setBuddy(lT2);

    gridLayout->addWidget(lScore, r, 0);
    gridLayout->addWidget(lS1, r, 1);
    gridLayout->addWidget(lS2, r, 2);
    gridLayout->addWidget(lHscore, r, 3);
    gridLayout->addWidget(lHscore_, r, 4);
    gridLayout->addWidget(prevRes, r, 5);
    r++;

    gridLayout->addWidget(lTime, r, 0);
    gridLayout->addWidget(lT1, r, 1);
    gridLayout->addWidget(lT2, r, 2);
    gridLayout->addWidget(currGame_, r, 4);
    gridLayout->addWidget(nextRes, r, 5);
    r++;


    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

    mapper = new QSignalMapper(this);
    for(int row = 0 + r; row < 5 + r; ++row){
        for(int column = 0; column < 6; column++){
            QPushButton *button = new QPushButton;
            QSize buttonSize(100,100);
            button->setFixedSize(buttonSize);
            button->setStyleSheet("QPushButton { font: bold italic large \"Times New Roman\";"
                                  "font-size: 30px;}");
            button->setText("?");
            gridLayout->addWidget(button, row, column);
            board.append(button);
            mapper->setMapping(button, board.count()-1);
            connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
        }
    }
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(handleButtonClick(int)));
    setLayout(gridLayout);
}

/**
 * @brief CardMatch::newGameClicked
 * Initialize the game when the "New Game" button clicked
 */
void CardMatch::newGameClicked()
{
    int s1 = lS1->text().toInt();
    int s2 = lS2->text().toInt();
    int totalScore = s1 + s2;
    int chkX = lGame_->text().toInt();
    if(chkX == gameNumber && totalScore < 15){
        scoreInfo[gameNumber].push_back(s1);
        scoreInfo[gameNumber].push_back(s2);
        timeInfo[gameNumber].push_back(lT1->text().toInt());
        timeInfo[gameNumber].push_back(lT2->text().toInt());
    }
    et.restart();
    timer->start(1000);
    clicked = 0;
    paired = 0;
    pairs = false;
    clkIndex.clear();
    gameNumber++;
    lGame_->setText(QString::number(gameNumber));
    lS1->setText(QString::number(0));
    lS2->setText(QString::number(0));
    lT1->setText(QString::number(0));
    lT2->setText(QString::number(0));
    for(int i=0; i<30; ++i){
        board.at(i)->hide();
        turnBack(i);
    }
    shuffle(word.begin(), word.end(), default_random_engine(random_device()()));
}

/**
 * @brief CardMatch::prevResClicked
 * Gives statistics related with the previous game
 */
void CardMatch::prevResClicked()
{
    int labelGame = lGame_->text().toInt();
    int s1 = lS1->text().toInt();
    int s2 = lS2->text().toInt();
    int totalScore = s1 + s2;
    if(labelGame > 1){
        timer->stop();
        if(labelGame == gameNumber){
            if(totalScore < 15){
                scoreInfo[gameNumber].push_back(s1);
                scoreInfo[gameNumber].push_back(s2);
                timeInfo[gameNumber].push_back(lT1->text().toInt());
                timeInfo[gameNumber].push_back(lT2->text().toInt());
            }
        }
        int x = labelGame - 1;
        lGame_->setText(QString::number(x));
        lS1->setText(QString::number(scoreInfo[x][0]));
        lS2->setText(QString::number(scoreInfo[x][1]));
        lT1->setText(QString::number(timeInfo[x][0]));
        lT2->setText(QString::number(timeInfo[x][1]));
    }
}

/**
 * @brief CardMatch::nextResClicked
 * Gives statistics related with the next game
 */
void CardMatch::nextResClicked()
{
    int labelGame = lGame_->text().toInt();

    if(labelGame < gameNumber){
        int x = labelGame + 1;
        lGame_->setText(QString::number(x));
        lS1->setText(QString::number(scoreInfo[x][0]));
        lS2->setText(QString::number(scoreInfo[x][1]));
        lT1->setText(QString::number(timeInfo[x][0]));
        lT2->setText(QString::number(timeInfo[x][1]));

        if(x == gameNumber){
            et.start();
            timer->start(1000);
        }

    }
}

/**
 * @brief CardMatch::turnDown
 * Decides how the cards turn down according the paired relation
 */
void CardMatch::turnDown(){
    if(pairs){
        for(int i = 0; i < 2; i++){
            int ind = clkIndex[i];
            QPushButton *button = board.at(ind);
            //button->setText(" ");
            //button->setPalette(QPalette());
            button->hide();

            button = new QPushButton;
            button->setText(" ");
            QSize buttonSize(100,100);
            button->setFixedSize(buttonSize);
            button->setStyleSheet("QPushButton { background-color : grey;}");
            gridLayout->addWidget(button, ind/6 + 3, ind%6);
            board.replace(ind, button);
            mapper->setMapping(button, ind);
            connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
        }
        clkIndex.clear();
        pairs = false;
    }else{
        for(int i = 0; i < 2; i++){
           int ind = clkIndex[i];
           QPushButton *button = board.at(ind);
           button->hide();
           turnBack(ind);
        }
        clkIndex.clear();
    }
}

/**
 * @brief CardMatch::gameOver
 * Freeze the time and looks for the highest score
 */
void CardMatch::gameOver(){
    finishedGame++;
    for(int i = 0; i < 2; i++){
        int ind = clkIndex[i];
        QPushButton *button = board.at(ind);
        button->hide();
        button = new QPushButton;
        button->setText(" ");
        QSize buttonSize(100,100);
        button->setStyleSheet("QPushButton { background-color : grey;}");
        button->setFixedSize(buttonSize);
        gridLayout->addWidget(button, ind/6 + 3, ind%6);
        board.replace(ind, button);
        mapper->setMapping(button, ind);
        connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
    }
    clkIndex.clear();
    int s1 = lS1->text().toInt();
    int s2 = lS2->text().toInt();
    int max_ = s1;
    if(max_ < s2) max_ = s2;
    int s3 = lHscore_->text().toInt();
    if(s3 < max_) lHscore_->setText(QString::number(max_));
    scoreInfo[gameNumber].push_back(s1);
    scoreInfo[gameNumber].push_back(s2);
    timeInfo[gameNumber].push_back(lT1->text().toInt());
    timeInfo[gameNumber].push_back(lT2->text().toInt());
    timer->stop();
}

/**
 * @brief CardMatch::incPoint
 * Increments the winner point
 */
void CardMatch::incPoint()
{
    int winner = clicked%4;
    if(winner == 2){
        int p = lS1->text().toInt() + 1;
        lS1->setText(QString::number(p));
    }else{
        int p = lS2->text().toInt() + 1;
        lS2->setText(QString::number(p));
    }
}

/**
 * @brief CardMatch::handleButtonClick
 * Handle when the card button is clicked
 * @param index takes index of the card button
 */
void CardMatch::handleButtonClick(int index)
{
    int labelGame = lGame_->text().toInt();
    if(gameNumber == labelGame){
        if(index < 0 || index >= board.size()) return;
      QPushButton *button = board.at(index);
        if(button->text() == "?"){
         clicked++;
         if(clicked%4 == 0 || clicked%4 == 1){
             lP1->setStyleSheet("QLabel { background-color : green; color : blue; }");
             lP2->setStyleSheet("QLabel { background-color : red; color : blue; }");
         }else{
             lP1->setStyleSheet("QLabel { background-color : red; color : blue; }");
             lP2->setStyleSheet("QLabel { background-color : green; color : blue; }");
         }
         button->setText(" ");
         QString text = word[index];
         button->setIcon(QIcon(text));
         QSize buttonSize(80 ,95);
         button->setIconSize(buttonSize);
          if(clicked%2 == 0){
             clkIndex.push_back(index);
             if(clkInfo == text){
                 paired++;
                 incPoint();
                 pairs = true;
                    if(paired == 15){
                     gameOver();
                    }
                }
            }else{
             if(clkIndex.size()==2){
                 turnDown();
                }
                clkInfo = text;
                clkIndex.push_back(index);
            }
        }
    }
}

/**
 * @brief CardMatch::showTime
 * Shows the time
 */
void CardMatch::showTime()
{
    int clk = clicked%4;
    int time;
    if(clk == 0 || clk == 1){
       time = lT1->text().toInt();
       time += et.elapsed()/1000;
       et.restart();
       lT1->setText(QString::number(time));
    }else{
        time = lT2->text().toInt();
        time += et.elapsed()/1000;
        lT2->setText(QString::number(time));
        et.restart();
    }
    timer->start(1000);
}

/**
 * @brief CardMatch::currGameClicked
 * Turns back to current game
 */
void CardMatch::currGameClicked()
{
    int labelGame = lGame_->text().toInt();

    if(labelGame < gameNumber){
        int x = gameNumber;
        lGame_->setText(QString::number(x));
        lS1->setText(QString::number(scoreInfo[x][0]));
        lS2->setText(QString::number(scoreInfo[x][1]));
        lT1->setText(QString::number(timeInfo[x][0]));
        lT2->setText(QString::number(timeInfo[x][1]));
        et.start();
        timer->start(1000);
    }
}

/**
 * @brief CardMatch::turnBack
 * Returns the button as the initialized state
 * @param i index of the card button
 */
void CardMatch::turnBack(int i)
{
    QPushButton *button = new QPushButton;
    button->setStyleSheet("QPushButton { font: bold italic large \"Times New Roman\";"
                          "font-size: 30px;}");
    button->setText("?");
    QSize buttonSize(100,100);
    button->setFixedSize(buttonSize);
    gridLayout->addWidget(button, i/6 + 3, i%6);
    board.replace(i, button);
    mapper->setMapping(button, i);
    connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
}
