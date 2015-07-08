#include "figure.h"

QString figureTypeToString(FigureType type) {
    QString resType;

    switch (type) {
    case FIGURE_TYPE_KING:
        resType = "king";
        break;
    case FIGURE_TYPE_QUEEN:
        resType = "queen";
        break;
    case FIGURE_TYPE_ROOK:
        resType = "rook";
        break;
    case FIGURE_TYPE_BISHOP:
        resType = "bishop";
        break;
    case FIGURE_TYPE_KNIGHT:
        resType = "knight";
        break;
    case FIGURE_TYPE_PAWN:
        resType = "pawn";
        break;
    default:
        resType = "pawn";
        break;
    }

    return resType;
}

QString figureColorToString(FigureColor color) {
    if (color == FIGURE_COLOR_WHITE) {
        return "w";
    } else if (color == FIGURE_COLOR_BLACK) {
        return "b";
    } else {
        return "";
    }
}

Figure::Figure(QObject *parent) : QObject(parent)
{
    m_Type = FIGURE_TYPE_PAWN;
    m_Color = FIGURE_COLOR_WHITE;
    m_Row = -1;
    m_Column = -1;
    m_CanMove = true;
    m_AtInitial = true;
}

Figure::Figure(FigureType type, FigureColor color, int row, int column, bool canMove){
    m_Type = type;
    m_InitialType = type;
    m_Color = color;
    m_Row = row;
    m_Column = column;
    m_CanMove = canMove;
    m_AtInitial = true;
}

QString Figure::type() {
    return figureTypeToString(m_Type);
}

QString Figure::color() {
    return figureColorToString(m_Color);
}

FigureColor Figure::getColor() {
    return m_Color;
}

FigureType Figure::getType() {
    return m_Type;
}

int Figure::getRow() {
    return m_Row;
}

int Figure::getColumn() {
    return m_Column;
}

bool Figure::canMove() {
    return m_CanMove;
}

void Figure::setCanMove(bool state) {
    m_CanMove = state;
    emit canMoveChanged(m_CanMove);
}

bool Figure::getIsAtInitial() {
    return m_AtInitial;
}

void Figure::setIsAtInitial(bool state) {
    m_AtInitial = state;
}

void Figure::moveTo(int row, int column) {
    m_Row = row;
    m_Column = column;

    if (boardIndex() < 0) {
        emit visibleChanged(false);
    } else {
        emit visibleChanged(true);
    }

    m_AtInitial = false;

    emit boardIndexChanged(boardIndex());
}

int Figure::boardIndex() {
    return m_Row * 8 + m_Column;
}

void Figure::setType(FigureType type) {
    m_Type = type;
    emit typeChanged(this->type());
}

void Figure::resetType() {
    m_Type = m_InitialType;
    emit typeChanged(this->type());
}

bool Figure::visible() {
    if (boardIndex() < 0) {
        return false;
    }

    return true;
}
