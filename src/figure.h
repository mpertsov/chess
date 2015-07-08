#ifndef FIGURE_H
#define FIGURE_H

#include "figurecolor.h"
#include "figuretype.h"

#include <QObject>

QString figureColorToString(FigureColor color);

class Figure : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString color READ color NOTIFY colorChanged)
    Q_PROPERTY(int boardIndex READ boardIndex NOTIFY boardIndexChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(bool canMove READ canMove NOTIFY canMoveChanged)
public:
    explicit Figure(QObject *parent = 0);
    explicit Figure(FigureType type, FigureColor color, int row, int column, bool canMove);

private:
    bool m_CanMove;
    FigureType m_InitialType;
    FigureType m_Type;
    FigureColor m_Color;
    int m_Row;
    int m_Column;
    bool m_AtInitial;

public:
    QString type();
    QString color();
    bool visible();
    bool canMove();

    int getRow();
    int getColumn();
    FigureColor getColor();
    FigureType getType();
    bool getIsAtInitial();

    int boardIndex();
    void resetType();
    void setType(FigureType type);
    void setCanMove(bool state);
    void setIsAtInitial(bool state);
    void moveTo(int row, int column);

signals:
    void typeChanged(QString newType);
    void colorChanged(QString color);
    void boardIndexChanged(int newBoardIndex);
    void visibleChanged(bool visible);
    void canMoveChanged(bool state);
};

#endif // FIGURE_H
