#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <QObject>
#include <QQmlListProperty>
#include "figure.h"
#include "statetype.h"
#include "figureturn.h"

class BoardState : public QObject
{   
    Q_OBJECT
    Q_PROPERTY(bool whitesTurnAvailable READ whitesTurnAvailable NOTIFY whitesTurnAvailableChanged)
    Q_PROPERTY(bool blacksTurnAvailable READ blacksTurnAvailable NOTIFY blacksTurnAvailableChanged)
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
private:
    FigureColor m_TurnColor;
    int m_Board[8][8];
    StateType m_CurrentState;

    QList<Figure*> m_Whites;
    QList<Figure*> m_Blacks;

    bool m_HistoryModeActive;
    QList<FigureTurn> m_HistoryLog;
    QList<FigureTurn> m_Log;

private: //methods
    void initStateArray();

    bool isFigureThere(int row, int col, FigureColor color);
    bool isMyOnThePath(Figure *figure, int endRow, int endCol);
    bool isEnemyInTheMiddle(Figure *figure, int endRow, int endCol);

    void moveHistoryFigureTo(Figure *figure, int endIndex);
    void addToLog(Figure *figure, int newIndex, bool enemyKilled);

public:
    explicit BoardState(QObject *parent = 0);

    Q_INVOKABLE void init();
    Q_INVOKABLE void initBoard(bool fromHistory);
    Q_INVOKABLE bool loadFromFile(QString filename);
    Q_INVOKABLE void loadAccepted();
    Q_INVOKABLE bool saveToFile(QString filename);
    Q_INVOKABLE void stopGame();
    Q_INVOKABLE void historyBackward();
    Q_INVOKABLE void historyForward();
    Q_INVOKABLE void historyEnd();
    Q_INVOKABLE Figure *getFigureAt(int index);

    Q_INVOKABLE bool turnAvailable(QString color);
    Q_INVOKABLE bool isAvailableForFigure(Figure *figure, int endIndex);
    Q_INVOKABLE void moveFigureTo(Figure *figure, int endIndex);

    bool whitesTurnAvailable();
    bool blacksTurnAvailable();
    int state();

signals:
   void whitesTurnAvailableChanged(bool available);
   void blacksTurnAvailableChanged(bool available);

   void stateChanged(int state);
};

#endif // BOARDSTATE_H
