#include "boardstate.h"
#include <QFileDialog>
#include <QDebug>

using std::max;
using std::abs;

BoardState::BoardState(QObject *parent) : QObject(parent) {
    m_TurnColor = FIGURE_COLOR_NONE;
    m_HistoryModeActive = false;
}

void BoardState::init() {
    m_CurrentState = STATE_INITIAL;
    emit stateChanged(m_CurrentState);

    // init whites
    QList<FigureType> whites;
    whites << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN
           << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN << FIGURE_TYPE_ROOK << FIGURE_TYPE_KNIGHT
           << FIGURE_TYPE_BISHOP << FIGURE_TYPE_QUEEN << FIGURE_TYPE_KING << FIGURE_TYPE_BISHOP << FIGURE_TYPE_KNIGHT
           << FIGURE_TYPE_ROOK;
    for (int i = 0; i < whites.length(); ++i) {
        int row = 6 + i / 8;
        int col = i % 8;
        m_Whites.append(new Figure(whites[i], FIGURE_COLOR_WHITE, row, col, false));
    }

    // init blacks
    QList<FigureType> blacks;
    blacks << FIGURE_TYPE_ROOK << FIGURE_TYPE_KNIGHT << FIGURE_TYPE_BISHOP << FIGURE_TYPE_QUEEN << FIGURE_TYPE_KING
           << FIGURE_TYPE_BISHOP << FIGURE_TYPE_KNIGHT << FIGURE_TYPE_ROOK << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN
           << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN << FIGURE_TYPE_PAWN
           << FIGURE_TYPE_PAWN;
    for (int i = 0; i < blacks.length(); ++i) {
        int row = i / 8;
        int col = i % 8;
        m_Blacks.append(new Figure(blacks[i], FIGURE_COLOR_BLACK, row, col, false));
    }

    initStateArray();
}

void BoardState::initBoard(bool fromHistory) {
    m_TurnColor = FIGURE_COLOR_WHITE;
    m_Log.clear();

    if (fromHistory == false) {
        m_HistoryLog.clear();
        m_HistoryModeActive = false;
        m_CurrentState = STATE_PLAY;
    } else {
        m_CurrentState = STATE_LOAD;
    }

    emit stateChanged((int)m_CurrentState);

    initStateArray();

    for (int i = 0; i < 16; ++i) {
        int whitesRow = 6 + i / 8;
        int blacksRow = i / 8;
        int col = i % 8;

        m_Whites[i]->moveTo(whitesRow, col);
        m_Whites[i]->setCanMove(fromHistory == false);
        m_Whites[i]->setIsAtInitial(true);
        m_Blacks[i]->moveTo(blacksRow, col);
        m_Blacks[i]->setCanMove(fromHistory == false);
        m_Blacks[i]->setIsAtInitial(true);
    }

    emit whitesTurnAvailableChanged(true);
    emit blacksTurnAvailableChanged(false);
}

void BoardState::initStateArray() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int val = 0;
            if (i < 2) {
                val = FIGURE_COLOR_BLACK;
            } else if (i >= 6) {
                val = FIGURE_COLOR_WHITE;
            }

            m_Board[i][j] = val;
        }
    }
}

bool BoardState::loadFromFile(QString filename) {
    QList<FigureTurn> newLog;

    if (filename.contains("file:///")) {
        if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
            filename = filename.replace("file:///", "");
        } else {
            filename = filename.replace("file:///", "/");
        }
    }
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly) == false) {
        qDebug() << "error opening " << filename << ": " << file.errorString();
        return false;
    }

    QDataStream in(&file);

    while(in.atEnd() == false) {
        qint8 color;
        in >> color;
        if (in.atEnd()) {
            return false;
        }

        qint32 startIndex;
        in >> startIndex;
        if (in.atEnd()) {
            return false;
        }

        qint32 endIndex;
        in >> endIndex;
        if (in.atEnd()) {
            return false;
        }

        qint8 enemyKilled;
        in >> enemyKilled;

        FigureTurn turn;
        turn.color = (FigureColor)color;
        turn.startBoardIndex = startIndex;
        turn.endBoardIndex = endIndex;
        turn.enemyKilled = enemyKilled;
        newLog.append(turn);
    }

    file.close();

    m_HistoryModeActive = true;
    m_HistoryLog = newLog;

    return true;
}

void BoardState::loadAccepted() {
    m_HistoryModeActive = false;
    m_HistoryLog.clear();

    foreach (Figure *whiteFigure, m_Whites) {
        whiteFigure->setCanMove(true);
    }

    foreach (Figure *blackFigure, m_Blacks) {
        blackFigure->setCanMove(true);
    }

    m_CurrentState = STATE_PLAY;
    emit stateChanged((int)m_CurrentState);
}

bool BoardState::saveToFile(QString filename) {
    if (filename.contains("file:///")) {
        if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
            filename = filename.replace("file:///", "");
        } else {
            filename = filename.replace("file:///", "/");
        }
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly) == false) {
        qDebug() << "error saving " << filename << ": " << file.errorString();
        return false;
    }

    QDataStream out(&file);

    foreach (FigureTurn turn, m_Log) {
        out << (qint8)turn.color
            << (qint32)turn.startBoardIndex
            << (qint32)turn.endBoardIndex
            << (qint8)turn.enemyKilled;
    }

    file.close();

    return true;
}

Figure *BoardState::getFigureAt(int index) {
    if (index >= 48) {
        // whites
        index -= 48;
        return m_Whites[index];
    } else {
        return m_Blacks[index];
    }

    return m_Whites[0];
}

void BoardState::historyEnd() {
    while (m_Log.length() < m_HistoryLog.length()) {
        historyForward();
    }
}

void BoardState::historyBackward() {
    if (m_Log.length() == 0) {
        return;
    }

    FigureTurn turn = m_Log.last();

    if (turn.color == FIGURE_COLOR_WHITE) {
        for (int i = 0; i < m_Whites.length(); ++i) {
            if (m_Whites[i]->boardIndex() == turn.endBoardIndex) {
                int currentIndex = m_Whites[i]->boardIndex();
                moveHistoryFigureTo(m_Whites[i], turn.startBoardIndex);

                if (turn.enemyKilled) {
                    foreach (Figure *f, m_Blacks) {
                        if (f->boardIndex() == -currentIndex) {
                            f->moveTo(-f->getRow(), -f->getColumn());
                            m_Board[f->getRow()][f->getColumn()] = -1;
                            break;
                        }
                    }
                }
                break;
            }
        }
    } else if (turn.color == FIGURE_COLOR_BLACK) {
        for (int i = 0; i < m_Blacks.length(); ++i) {
            if (m_Blacks[i]->boardIndex() == turn.endBoardIndex) {
                int currentIndex = m_Blacks[i]->boardIndex();
                moveHistoryFigureTo(m_Blacks[i], turn.startBoardIndex);

                if (turn.enemyKilled) {
                    foreach (Figure *f, m_Whites) {
                        if (f->boardIndex() == -currentIndex) {
                            f->moveTo(-f->getRow(), -f->getColumn());
                            m_Board[f->getRow()][f->getColumn()] = 1;
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    m_Log.erase(m_Log.end() - 1);
    m_TurnColor = turn.color;

    emit whitesTurnAvailableChanged(m_TurnColor == FIGURE_COLOR_WHITE);
    emit blacksTurnAvailableChanged(m_TurnColor == FIGURE_COLOR_BLACK);
}

void BoardState::historyForward() {
    if (m_Log.length() == m_HistoryLog.length()) {
        return;
    }

    FigureTurn turn = m_HistoryLog[m_Log.length()];

    if (turn.color == FIGURE_COLOR_WHITE) {
        for (int i = 0; i < m_Whites.length(); ++i) {
            if (m_Whites[i]->boardIndex() == turn.startBoardIndex) {
                moveHistoryFigureTo(m_Whites[i], turn.endBoardIndex);
                break;
            }
        }
    } else if (turn.color == FIGURE_COLOR_BLACK) {
        for (int i = 0; i < m_Blacks.length(); ++i) {
            if (m_Blacks[i]->boardIndex() == turn.startBoardIndex) {
                moveHistoryFigureTo(m_Blacks[i], turn.endBoardIndex);
                break;
            }
        }
    }

    m_Log.append(turn);

    if (turn.color == FIGURE_COLOR_WHITE) {
        m_TurnColor = FIGURE_COLOR_BLACK;
    } else {
        m_TurnColor = FIGURE_COLOR_WHITE;
    }

    emit whitesTurnAvailableChanged(m_TurnColor == FIGURE_COLOR_WHITE);
    emit blacksTurnAvailableChanged(m_TurnColor == FIGURE_COLOR_BLACK);
}

void BoardState::addToLog(Figure *figure, int newIndex, bool enemyKilled) {
    FigureTurn turn;
    turn.color = figure->getColor();
    turn.startBoardIndex = figure->boardIndex();
    turn.endBoardIndex = newIndex;
    turn.enemyKilled = enemyKilled;

    m_Log.append(turn);
}

bool BoardState::isFigureThere(int row, int col, FigureColor color) {
    if (m_Board[row][col] == color) {
        return true;
    }

    return false;
}

bool BoardState::isMyOnThePath(Figure *figure, int endRow, int endCol) {
    int startRow = figure->getRow();
    int startCol = figure->getColumn();
    FigureColor color = figure->getColor();

    if (startRow != endRow && startCol != endCol) {
        if (abs(startRow - endRow) != abs(startCol - endCol)) {
            return false;
        } else {
            int steps = max(startRow - endRow, endRow - startRow);
            int rowMult = startRow > endRow ? -1 : 1;
            int colMult = startCol > endCol ? -1 : 1;
            int currentRow = startRow + rowMult;
            int currentCol = startCol + colMult;
            while (steps > 0) {
                if (isFigureThere(currentRow, currentCol, color)) {
                    return true;
                }

                currentRow += rowMult;
                currentCol += colMult;
                steps--;
            }

            return false;
        }
    } else if (startRow != endRow) {
        int steps = max(startRow - endRow, endRow - startRow);
        int rowMult = startRow > endRow ? -1 : 1;
        int currentRow = startRow + rowMult;
        while (steps > 0) {
            if (isFigureThere(currentRow, startCol, color)) {
                return true;
            }

            currentRow += rowMult;
            steps--;
        }

        return false;
    } else if (startCol != endCol) {
        int steps = max(startCol - endCol, endCol - startCol);
        int colMult = startCol > endCol ? -1 : 1;
        int currentCol = startCol + colMult;
        while (steps > 0) {
            if (isFigureThere(startRow, currentCol, color)) {
                return true;
            }
            currentCol += colMult;
            steps--;
        }

        return false;
    }

    return false;
}

bool BoardState::isEnemyInTheMiddle(Figure *figure, int endRow, int endCol) {
    int startRow = figure->getRow();
    int startCol = figure->getColumn();
    FigureColor enemyColor = figure->getColor() == FIGURE_COLOR_WHITE ? FIGURE_COLOR_BLACK : FIGURE_COLOR_WHITE;

    if (startRow != endRow && startCol != endCol) {
        if (abs(startRow - endRow) != abs(startCol - endCol)) {
            return false;
        } else {
            int steps = max(startRow - endRow, endRow - startRow);
            int currentRow = startRow;
            int currentCol = startCol;
            while (steps > 0) {
                if (isFigureThere(currentRow, currentCol, enemyColor)) {
                    return true;
                }

                if (startRow > endRow) {
                    currentRow--;
                } else {
                    currentRow++;
                }

                if (startCol > endCol) {
                    currentCol--;
                } else {
                    currentCol++;
                }

                steps--;
            }

            return false;
        }
    } else if (startRow != endRow) {
        int steps = max(startRow - endRow, endRow - startRow);
        int currentRow = startRow;
        while (steps > 0) {
            if (isFigureThere(currentRow, startCol, enemyColor)) {
                return true;
            }

            if (startRow > endRow) {
                currentRow--;
            } else {
                currentRow++;
            }

            steps--;
        }

        return false;
    } else if (startCol != endCol) {
        int steps = max(startCol - endCol, endCol - startCol);
        int currentCol = startCol;
        while (steps > 0) {
            if (isFigureThere(startRow, currentCol, enemyColor)) {
                return true;
            }

            if (startCol > endCol) {
                currentCol--;
            } else {
                currentCol++;
            }

            steps--;
        }

        return false;
    }

    return false;
}

bool BoardState::isAvailableForFigure(Figure *figure, int endIndex) {
    int startIndex = figure->boardIndex();
    int startRow = startIndex / 8;
    int startCol = startIndex % 8;
    int endRow = endIndex / 8;
    int endCol = endIndex % 8;
    FigureType type = figure->getType();
    FigureColor color = figure->getColor();
    FigureColor enemyColor = color == FIGURE_COLOR_WHITE ? FIGURE_COLOR_BLACK : FIGURE_COLOR_WHITE;

    if (startRow == endRow && startCol == endCol) {
        return true;
    }

    if (startRow < 0 && startCol < 0) {
        return false;
    }

    bool res = false;

    if (type == FIGURE_TYPE_PAWN) {
        if (color == FIGURE_COLOR_WHITE) {
            if (endRow > startRow) {
                res = false;
            } else if (endCol == startCol && isFigureThere(endRow, endCol, enemyColor)) {
                res = false;
            } else if (figure->getIsAtInitial() && endRow < startRow - 2) {
                res = false;
            } else if (figure->getIsAtInitial() && endRow == startRow - 2 &&
                       isEnemyInTheMiddle(figure, endRow, endCol)) {
                res = false;
            } else if ((endCol == startCol + 1 || endCol == startCol - 1) &&
                       endRow == startRow - 1 && isFigureThere(endRow, endCol, enemyColor)) {
                res = true;
            } else if (figure->getIsAtInitial() == false && endRow < startRow - 1) {
                res = false;
            } else if (endCol != startCol) {
                res = false;
            } else if (isFigureThere(endRow, endCol, color)) {
                res = false;
            } else {
                res = true;
            }
        } else {
            if (endRow < startRow) {
                res = false;
            } else if (endCol == startCol && isFigureThere(endRow, endCol, enemyColor)) {
                res = false;
            } else if (figure->getIsAtInitial() && endRow > startRow + 2) {
                res = false;
            } else if (figure->getIsAtInitial() && endRow == startRow + 2 &&
                       isEnemyInTheMiddle(figure, endRow, endCol)) {
                res = false;
            } else if ((endCol == startCol + 1 || endCol == startCol - 1) &&
                       endRow == startRow + 1 && isFigureThere(endRow, endCol, enemyColor)) {
                res = true;
            } else if (figure->getIsAtInitial() == false && endRow > startRow + 1) {
                res = false;
            } else if (endCol != startCol) {
                res = false;
            } else if (isFigureThere(endRow, endCol, color)) {
                res = false;
            } else {
                res = true;
            }
        }
    } else if (type == FIGURE_TYPE_KING) {
        if (abs(endRow - startRow) > 1) {
            res = false;
        } else if (abs(endCol - startCol) > 1) {
            res = false;
        } else if (isFigureThere(endRow, endCol, color)) {
            res = false;
        } else {
            res = true;
        }
    } else if (type == FIGURE_TYPE_QUEEN) {
        if ((abs(endRow - startRow) > 0 && abs(endCol - startCol) == 0) ||
            (abs(endRow - startRow) == 0 && abs(endCol - startCol) > 0) ||
            (abs(endRow - startRow) == abs(endCol - startCol))) {

            if (isEnemyInTheMiddle(figure, endRow, endCol)) {
                res = false;
            } else if (isMyOnThePath(figure, endRow, endCol)) {
                res = false;
            } else {
                res = true;
            }
        } else {
            res = false;
        }
    } else if (type == FIGURE_TYPE_ROOK) {
        if ((abs(endRow - startRow) > 0 && abs(endCol - startCol) == 0) ||
            (abs(endRow - startRow) == 0 && abs(endCol - startCol) > 0)) {
            if (isEnemyInTheMiddle(figure, endRow, endCol)) {
                res = false;
            } else if (isMyOnThePath(figure, endRow, endCol)) {
                res = false;
            } else {
                res = true;
            }
        } else {
            res = false;
        }
    } else if (type == FIGURE_TYPE_BISHOP) {
        if (abs(endRow - startRow) == abs(endCol - startCol)) {
            if (isEnemyInTheMiddle(figure, endRow, endCol)) {
                res = false;
            } else if (isMyOnThePath(figure, endRow, endCol)) {
                res = false;
            } else {
                res = true;
            }
        } else {
            res = false;
        }
    } else if (type == FIGURE_TYPE_KNIGHT) {
        if ((abs(endRow - startRow) == 2 && abs(endCol - startCol) == 1) ||
            (abs(endRow - startRow) == 1 && abs(endCol - startCol) == 2)) {
            if (isFigureThere(endRow, endCol, color)) {
                res = false;
            } else {
                res = true;
            }
        } else {
            res = false;
        }
    }

    return res;
}

void BoardState::moveHistoryFigureTo(Figure *figure, int endIndex) {
    if (figure->boardIndex() == endIndex) {
        return;
    }

    int startRow = figure->getRow();
    int startCol = figure->getColumn();
    int endRow = endIndex / 8;
    int endCol = endIndex % 8;
    FigureColor color = figure->getColor();
    FigureColor enemyColor = color == FIGURE_COLOR_WHITE ? FIGURE_COLOR_BLACK : FIGURE_COLOR_WHITE;

    figure->moveTo(endRow, endCol);

    if (m_Board[endRow][endCol] == enemyColor) {
        QList<Figure*> *targetFigures = enemyColor == FIGURE_COLOR_WHITE ? &m_Whites : &m_Blacks;
        for (int i = 0; i < targetFigures->length(); ++i) {
            if (targetFigures->at(i)->boardIndex() == endIndex) {
                targetFigures->at(i)->moveTo(-targetFigures->at(i)->getRow(), -targetFigures->at(i)->getColumn());
                break;
            }
        }
    }

    m_Board[startRow][startCol] = 0;
    m_Board[endRow][endCol] = color;
}

void BoardState::moveFigureTo(Figure *figure, int endIndex) {
    if (figure->boardIndex() == endIndex) {
        return;
    }

    int startRow = figure->getRow();
    int startCol = figure->getColumn();
    int endRow = endIndex / 8;
    int endCol = endIndex % 8;
    FigureColor color = figure->getColor();
    FigureColor enemyColor = color == FIGURE_COLOR_WHITE ? FIGURE_COLOR_BLACK : FIGURE_COLOR_WHITE;

    bool enemyKilled = (m_Board[endRow][endCol] == enemyColor);
    addToLog(figure, endIndex, enemyKilled);
    figure->moveTo(endRow, endCol);

    if (enemyKilled) {
        QList<Figure*> *targetFigures = enemyColor == FIGURE_COLOR_WHITE ? &m_Whites : &m_Blacks;
        for (int i = 0; i < targetFigures->length(); ++i) {
            if (targetFigures->at(i)->boardIndex() == endIndex) {
                targetFigures->at(i)->moveTo(-targetFigures->at(i)->getRow(), -targetFigures->at(i)->getColumn());
                break;
            }
        }
    }

    m_Board[startRow][startCol] = 0;
    m_Board[endRow][endCol] = color;

    if (color == FIGURE_COLOR_WHITE) {
        m_TurnColor = FIGURE_COLOR_BLACK;
    } else {
        m_TurnColor = FIGURE_COLOR_WHITE;
    }

    emit whitesTurnAvailableChanged(m_TurnColor == FIGURE_COLOR_WHITE);
    emit blacksTurnAvailableChanged(m_TurnColor == FIGURE_COLOR_BLACK);
}


bool BoardState::turnAvailable(QString color) {
    return color == figureColorToString(m_TurnColor);
}

bool BoardState::whitesTurnAvailable() {
    return m_TurnColor == FIGURE_COLOR_WHITE;
}

bool BoardState::blacksTurnAvailable() {
    return m_TurnColor == FIGURE_COLOR_BLACK;
}

int BoardState::state() {
    return m_CurrentState;
}

void BoardState::stopGame() {
    m_CurrentState = STATE_INITIAL;
    emit stateChanged((int)m_CurrentState);

    m_TurnColor = FIGURE_COLOR_NONE;
    emit whitesTurnAvailableChanged(false);
    emit blacksTurnAvailableChanged(false);
}
