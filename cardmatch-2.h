#ifndef CARDMATCH_H
#define CARDMATCH_H

#include <QWidget>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QLineEdit>

class CardMatch : public QWidget
{
    Q_OBJECT

public:
    CardMatch(QWidget *parent = 0);
    void buttonWasClicked(int);
    void turnBack(int);
    void turnDown();
    void gameOver();
    void incPoint();
    ~CardMatch();

public:
    int gameNumber = 1;
    int paired = 0;
    int clicked = 0;
    QList<QPushButton*> board;
    QPushButton *newGame, *prevRes, *nextRes, *currGame_;
    QLabel *lP1, *lP2;
    QLabel *lScore, *lTime;
    QLabel *lS1, *lS2;
    QLabel *lT1, *lT2;
    QLabel *lGame, *lGame_;
    QLabel *lHscore,*lHscore_;
    void setupBoard();
public slots:
    void newGameClicked();
    void prevResClicked();
    void nextResClicked();
    void handleButtonClick(int);
    void showTime();
    void currGameClicked();
};

#endif // CARDMATCH_H
