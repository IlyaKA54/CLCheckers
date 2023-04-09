//
// Created by EgrZver on 20.03.2023.
//

#include "clcengine_types.h"

#ifndef CHECKERS_CLCHECKERS_H
#define CHECKERS_CLCHECKERS_H



void appendToATakingSequence(TakingSequence* sequence, TakingMove move) {
    sequence->takingMoves[sequence->tmsCount++] = move;
}

void deleteLastFromATakingSequence(TakingSequence* sequence) {
    sequence->tmsCount--;
}

void copyToAnotherTakingSequence(TakingSequence* source, TakingSequence* destination) {
    for (int i = 0; i < source->tmsCount; i++) {
        destination->takingMoves[i] = source->takingMoves[i];
    }
    destination->tmsCount = source->tmsCount;
}

TakingSequence getNullPath() {
    TakingSequence path;
    path.tmsCount = 0;
    return path;
}


void initiateChecker(Checker* checker, int x, int y, Color color) {
    checker->coordinates.x = x;
    checker->coordinates.y = y;
    checker->color = color;
    checker->type = Regular;
    checker->markedForDeath = false;
}

void updateBoardRender(Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board->boardRender[j][i] = ((i + j) % 2 == 0) ? EMPTY_BLACK : EMPTY_WHITE;
        }
    }
    for (int i = 0; i < board->checkersCount[Black]; i++) {
        int rcx = board->checkers[Black][i].coordinates.x, rcy = board->checkers[Black][i].coordinates.y;
        board->boardRender[rcy][rcx] = (board->checkers[Black][i].type == King) ? KING_BLACK : REG_BLACK;
    }
    for (int i = 0; i < board->checkersCount[White]; i++) {
        int rcx = board->checkers[White][i].coordinates.x, rcy = board->checkers[White][i].coordinates.y;
        board->boardRender[rcy][rcx] = (board->checkers[White][i].type == King) ? KING_WHITE : REG_WHITE;
    }
}

Board createANewBoard(/*Color playerSide*/) {
    Board newBoard;
    newBoard.checkersCount[Black] = 12; newBoard.checkersCount[White] = 12;
    int valueOfInitedPairs = 0;
    for (short j = 0; j < 3; j++) {
        for (short k = 0; k < 8; k += 2) {
            initiateChecker(&newBoard.checkers[White][valueOfInitedPairs], k + (j % 2), j, White);
            initiateChecker(&newBoard.checkers[Black][valueOfInitedPairs++], k + 1 - (j % 2), 7 - j, Black);
        }
    }
    updateBoardRender(&newBoard);
    return newBoard;
}

GameSituation makeNullGameSituation(Color playerSide) {
    GameSituation gameSituation;
    gameSituation.board = createANewBoard();
    gameSituation.playerSide = playerSide;
    gameSituation.rmBufferLen = 0; //gameSituation.krmCount = 0; gameSituation.kbmCount = 0;
    gameSituation.tmBufferLen = 0;
    gameSituation.tmsCount = 0;
    gameSituation.rmsCount = 0;
    gameSituation.mmsCount = 0;
    gameSituation.lastTakingSequence.tmsCount = 0;
    for (int i = 0; i < 16; i++) gameSituation.takingSequences[i].tmsCount = 0;
    Color turnOf;
    //gameSituation.rtmCount = 0; gameSituation.ktmCount = 0;
    return gameSituation;
}

Game createANewGame(Color playerSide, Color firstMove, GameType type) {
    Game newGame;
    newGame.situation = makeNullGameSituation(playerSide);
    newGame.state = (playerSide == firstMove) ? PlayerMove : ((type == RvsR) ? EnemyMoveReal : EnemyMoveComputer);
    return newGame;
}

Checker getCheckerByCoords(GameSituation* situation, int cx, int cy) {
    for (int i = 0; i < situation->board.checkersCount[White]; i++) {
        if (situation->board.checkers[White][i].coordinates.x == cx && situation->board.checkers[White][i].coordinates.y == cy) {
            return situation->board.checkers[White][i];
        }
    }
    for (int i = 0; i < situation->board.checkersCount[Black]; i++) {
        if (situation->board.checkers[Black][i].coordinates.x == cx && situation->board.checkers[Black][i].coordinates.y == cy) {
            return situation->board.checkers[Black][i];
        }
    }
}

int getCheckerIndexByCoordsAndColor(GameSituation* situation, int cx, int cy, Color color) {
    for (int i = 0; i < situation->board.checkersCount[color]; i++) {
        if (situation->board.checkers[color][i].coordinates.x == cx && situation->board.checkers[color][i].coordinates.y == cy) {
            return i;
        }
    }
}

Color sideColor(GameSituation* situation, int cx, int cy) {
    BoardCellState tInfo = situation->board.boardRender[cy][cx];
    if (tInfo == REG_WHITE || tInfo == KING_WHITE) {
        return White;
    }
    else if (tInfo == REG_BLACK || tInfo == KING_BLACK) {
        return Black;
    }
}

Color negateColor(Color color) {
    if (color == White) return Black;
    else if (color == Black) return White;
}


CheckerType getCheckerTypeOnBoard(GameSituation* situation, int cx, int cy) {
    BoardCellState tInfo = situation->board.boardRender[cy][cx];
    if (tInfo == REG_WHITE || tInfo == REG_BLACK) {
        return Regular;
    }
    else if (tInfo == KING_BLACK || tInfo == KING_WHITE) {
        return King;
    }
}


bool isMarkedForDeath(GameSituation* situation, Color checkerColor, int mx, int my) {
    if (situation->board.boardRender[my][mx] == EMPTY_BLACK) return false;
    int index = getCheckerIndexByCoordsAndColor(situation, mx, my, checkerColor);
    return situation->board.checkers[checkerColor][index].markedForDeath;
}

bool isAVictim(GameSituation* situation, Color forWhichSide, int tx, int ty) {
    //bool lock = isMarkedForDeath(situation, negateColor(forWhichSide), getCheckerIndexByCoordsAndColor(situation, tx, ty, forWhichSide));
    BoardCellState tInfo = situation->board.boardRender[ty][tx];
    //if (lock) return false;
    //else {
        if (forWhichSide == Black) {
            return (tInfo == REG_WHITE || tInfo == KING_WHITE);
        } else if (forWhichSide == White) {
            return (tInfo == REG_BLACK || tInfo == KING_BLACK);
        } else return false;
    //}
}



bool isAFriend(GameSituation* situation, Color forWhichSide, int tx, int ty) {
    BoardCellState tInfo = situation->board.boardRender[ty][tx];
    if (forWhichSide == White) {
        return tInfo == REG_WHITE || tInfo == KING_WHITE;
    }
    else if (forWhichSide == Black) {
        return tInfo == REG_BLACK || tInfo == KING_BLACK;
    }
}



void findAllKBMovesForOne(GameSituation* situation, Color checkerColor, int checkerIndex) {
    if (situation->board.checkers[checkerColor][checkerIndex].type != King) {
        Move move;
        move.isKingMove = false; move.isKingBecomingMove = true;
        int ex = situation->board.checkers[checkerColor][checkerIndex].coordinates.x;
        int ey = situation->board.checkers[checkerColor][checkerIndex].coordinates.y;
        move.source.x = ex; move.source.y = ey;

        if (checkerColor == Black ? ey == 1 : ey == 6) {
            if (ex < 7) {
                if ((situation->board.boardRender[(checkerColor == Black ? ey - 1 : ey + 1)][ex+1] == EMPTY_BLACK)) {
                    move.destination.y = (checkerColor == Black ? ey - 1 : ey + 1); move.destination.x = ex + 1;
                    situation->regularMovesBuffer[situation->rmBufferLen++] = move;
                }
            }
            if (ex > 0) {
                if ((situation->board.boardRender[(checkerColor == Black ? ey - 1 : ey + 1)][ex-1] == EMPTY_BLACK)) {
                    move.destination.y = (checkerColor == Black ? ey - 1 : ey + 1); move.destination.x = ex - 1;
                    situation->regularMovesBuffer[situation->rmBufferLen++] = move;
                }
            }
        }
    }
}

bool longMoveLookingFlag(int x, int y, Coordinates direction) {
    if (direction.x == NORTH_EAST.x && direction.y == NORTH_EAST.y) {
        return !(x >= 7 || y >= 7);
    }
    else if (direction.x == NORTH_WEST.x && direction.y == NORTH_WEST.y) {
        return !(x <= 0 || y >= 7);
    }
    else if (direction.x == SOUTH_EAST.x && direction.y == SOUTH_EAST.y) {
        return !(x >= 7 || y <= 0);
    }
    else if (direction.x == SOUTH_WEST.x && direction.y == SOUTH_WEST.y) {
        return !(x <= 0 || y <= 0);
    }
}

void findAllKingMovesForOneOnDir(GameSituation* situation, Color checkerColor, int checkerIndex, Coordinates directionShift) {
    Move move;
    move.isKingBecomingMove = false; move.isKingMove = false;
    int ex = situation->board.checkers[checkerColor][checkerIndex].coordinates.x;
    int ey = situation->board.checkers[checkerColor][checkerIndex].coordinates.y;
    move.source = {ex, ey};
    bool flag = longMoveLookingFlag(ex, ey, directionShift);
    int shift = 1;
    //kingMoveLookingFlag(ex + shift*directionShift.x, ey + shift * directionShift.y, directionShift)
    while (flag) {
        if (situation->board.boardRender[ey + shift * directionShift.y][ex + shift*directionShift.x] == EMPTY_BLACK) {
            move.destination.x = ex + shift*directionShift.x;
            move.destination.y = ey + shift * directionShift.y;
            situation->regularMovesBuffer[situation->rmBufferLen++] = move;
            flag = longMoveLookingFlag(ex + shift*directionShift.x, ey + shift * directionShift.y, directionShift);
        } else {
            flag = false;
        }
        shift++;
    }

}

void findAllKingMovesForOne(GameSituation* situation, Color checkerColor, int checkerIndex) {
    if (situation->board.checkers[checkerColor][checkerIndex].type == King) {
        findAllKingMovesForOneOnDir(situation, checkerColor, checkerIndex, NORTH_EAST);
        findAllKingMovesForOneOnDir(situation, checkerColor, checkerIndex, NORTH_WEST);
        findAllKingMovesForOneOnDir(situation, checkerColor, checkerIndex, SOUTH_EAST);
        findAllKingMovesForOneOnDir(situation, checkerColor, checkerIndex, SOUTH_WEST);
    }
}


void findAllRegularMovesForOne(GameSituation* situation, Color checkerColor, int checkerIndex) {
    if (situation->board.checkers[checkerColor][checkerIndex].type != King) {
        Move move;
        move.isKingMove = false;
        move.isKingBecomingMove = false;
        int ex = situation->board.checkers[checkerColor][checkerIndex].coordinates.x;
        int ey = situation->board.checkers[checkerColor][checkerIndex].coordinates.y;
        move.source.x = ex;
        move.source.y = ey;
        bool flag = checkerColor == Black ? ey > 1 : ey < 6;
        if (flag) {
            if (ex < 7) {
                if ((situation->board.boardRender[checkerColor == Black ? ey - 1 : ey + 1][ex + 1] == EMPTY_BLACK)) {
                    move.destination.y = checkerColor == Black ? ey - 1 : ey + 1;
                    move.destination.x = ex + 1;
                    situation->regularMovesBuffer[situation->rmBufferLen++] = move;
                    //situation->rmCount++;
                }
            }
            if (ex > 0) {
                if ((situation->board.boardRender[checkerColor == Black ? ey - 1 : ey + 1][ex - 1] == EMPTY_BLACK)) {
                    move.destination.y = checkerColor == Black ? ey - 1 : ey + 1;
                    move.destination.x = ex - 1;
                    situation->regularMovesBuffer[situation->rmBufferLen++] = move;
                    //situation->rmCount++;
                }
            }
        }
    }
}

void findAllRegularMoves(GameSituation* situation, Color forWhichSide) {
    for (int i = 0; i < situation->board.checkersCount[forWhichSide]; i++) {
        findAllRegularMovesForOne(situation, forWhichSide, i);
    }
}

void findAllKingBecomingMoves(GameSituation* situation, Color forWhichSide) {
    for (int i = 0; i < situation->board.checkersCount[forWhichSide]; i++) {
        findAllKBMovesForOne(situation, forWhichSide, i);
    }
}

void findAllKingMoves(GameSituation* situation, Color forWhichSide) {
    for (int i = 0; i < situation->board.checkersCount[forWhichSide]; i++) {
        findAllKingMovesForOne(situation, forWhichSide, i);
    }
}



void findAllTakingMovesForOne(GameSituation* situation, Color checkerColor, int checkerIndex/*, TakingMove previousMove*/) {
    if (situation->board.checkers[checkerColor][checkerIndex].type != King) {
        TakingMove takingMove;
        int ex = situation->board.checkers[checkerColor][checkerIndex].coordinates.x;
        int ey = situation->board.checkers[checkerColor][checkerIndex].coordinates.y;
        takingMove.source.x = ex;
        takingMove.source.y = ey;
        //int vx = previousMove.victim.x, vy = previousMove.victim.y;
        if (ey < 6) {
            if (ex < 6) {
                //bool farIsFree = false;

                if ((situation->board.boardRender[ey + 2][ex + 2] == EMPTY_BLACK) && isAVictim(situation, checkerColor, ex + 1, ey + 1) && (!isMarkedForDeath(situation,negateColor(checkerColor), ex + 1, ey + 1))/* && ((ex + 1 != vx) || (ey + 1 != vy))*/) {
                    takingMove.destination.y = ey + 2;
                    takingMove.destination.x = ex + 2;
                    takingMove.victim.x = ex + 1;
                    takingMove.victim.y = ey + 1;
                    takingMove.takingSide = negateColor(checkerColor);
                    takingMove.victimType = getCheckerTypeOnBoard(situation, ex + 1, ey + 1);
                    takingMove.isASpecialMove = (checkerColor == White && ey == 5);
                    situation->takingMovesBuffer[situation->tmBufferLen++] = takingMove;
                    //situation->rmCount++;
                }
            }
            if (ex > 1) {
                if ((situation->board.boardRender[ey + 2][ex - 2] == EMPTY_BLACK) && isAVictim(situation, checkerColor, ex - 1, ey + 1) && !isMarkedForDeath(situation,
                                                                                                                                                             negateColor(checkerColor), ex - 1, ey + 1)/* && ((ex - 1 != vx) || (ey + 1 != vy))*/) {
                    takingMove.destination.y = ey + 2;
                    takingMove.destination.x = ex - 2;
                    takingMove.victim.x = ex - 1;
                    takingMove.victim.y = ey + 1;
                    takingMove.takingSide = negateColor(checkerColor);
                    takingMove.victimType = getCheckerTypeOnBoard(situation, ex - 1, ey + 1);
                    takingMove.isASpecialMove = (checkerColor == White && ey == 5);
                    situation->takingMovesBuffer[situation->tmBufferLen++] = takingMove;
                    //situation->rmCount++;
                }
            }
        }
        if (ey > 1) {
            if (ex < 6) {
                if ((situation->board.boardRender[ey - 2][ex + 2] == EMPTY_BLACK) && isAVictim(situation, checkerColor, ex + 1, ey - 1) && !isMarkedForDeath(situation,negateColor(checkerColor), ex + 1, ey - 1)/* && ((ex + 1 != vx) || (ey - 1 != vy))*/) {
                    takingMove.destination.y = ey - 2;
                    takingMove.destination.x = ex + 2;
                    takingMove.victim.x = ex + 1;
                    takingMove.victim.y = ey - 1;
                    takingMove.takingSide = negateColor(checkerColor);
                    takingMove.victimType = getCheckerTypeOnBoard(situation, ex + 1, ey - 1);
                    takingMove.isASpecialMove = (checkerColor == Black && ey == 2);
                    situation->takingMovesBuffer[situation->tmBufferLen++] = takingMove;
                    //situation->rmCount++;
                }
            }
            if (ex > 1) {
                if ((situation->board.boardRender[ey - 2][ex - 2] == EMPTY_BLACK) && isAVictim(situation, checkerColor, ex - 1, ey - 1) && !isMarkedForDeath(situation,negateColor(checkerColor), ex - 1, ey - 1)/* && ((ex - 1 != vx) || (ey - 1 != vy))*/) {
                    takingMove.destination.y = ey - 2;
                    takingMove.destination.x = ex - 2;
                    takingMove.victim.x = ex - 1;
                    takingMove.victim.y = ey - 1;
                    takingMove.takingSide = negateColor(checkerColor);
                    takingMove.victimType = getCheckerTypeOnBoard(situation, ex - 1, ey - 1);
                    takingMove.isASpecialMove = (checkerColor == Black && ey == 2);
                    situation->takingMovesBuffer[situation->tmBufferLen++] = takingMove;
                    //situation->rmCount++;
                }
            }
        }
    }
}

int findDistanceToVictim(GameSituation* situation, int x, int y, Color forWhichSide, Coordinates direction) {
    // -1 if no victim
    int shift = 1;
    //bool flag = true;
    while (longMoveLookingFlag(x + shift*direction.x, y + shift * direction.y, direction)) {
        if (isMarkedForDeath(situation, negateColor(forWhichSide), x + shift*direction.x, y + shift * direction.y)) return -1;

        //else if (isMarkedForDeath(situation, negateColor(forWhichSide), x + shift*direction.x, y + shift * direction.y)) return -1;
        else if (isAFriend(situation, forWhichSide, x + shift*direction.x, y + shift * direction.y)) return -1;
        else if (isAVictim(situation, forWhichSide, x + shift * direction.x, y + shift * direction.y)) {
            return shift;
        }
        else if (situation->board.boardRender[y + shift * direction.y][x + shift*direction.x] == EMPTY_BLACK) shift++;
        //else return -1;

    }
    return -1;
}

void findAllKingTakingMovesForOneOnDir(GameSituation* situation, Color checkerColor, int checkerIndex, Coordinates directionShift) {
    TakingMove takingMove;
    takingMove.isASpecialMove = false;
    takingMove.takingSide = negateColor(checkerColor);
    int ex = situation->board.checkers[checkerColor][checkerIndex].coordinates.x;
    int ey = situation->board.checkers[checkerColor][checkerIndex].coordinates.y;
    takingMove.source = {ex, ey};
    int victimDistance = findDistanceToVictim(situation, ex, ey, checkerColor, directionShift);
    if (victimDistance != -1) {
        takingMove.victim = {ex + victimDistance * directionShift.x, ey + victimDistance * directionShift.y};
        takingMove.victimType = getCheckerTypeOnBoard(situation, ex + victimDistance * directionShift.x, ey + victimDistance * directionShift.y);
        bool flag = longMoveLookingFlag(ex + victimDistance * directionShift.x, ey + victimDistance * directionShift.y, directionShift);
        int additionalShift = 1;
        //kingMoveLookingFlag(ex + shift*directionShift.x, ey + shift * directionShift.y, directionShift)
        while (flag) {
            if (situation->board.boardRender[ey + directionShift.y * (victimDistance + additionalShift)][ex + directionShift.x * (victimDistance + additionalShift)] == EMPTY_BLACK) {
                takingMove.destination = {ex + directionShift.x * (victimDistance + additionalShift), ey + directionShift.y * (victimDistance + additionalShift)};

                situation->takingMovesBuffer[situation->tmBufferLen++] = takingMove;
                flag = longMoveLookingFlag(ex + directionShift.x * (victimDistance + additionalShift), ey + directionShift.y * (victimDistance + additionalShift), directionShift);
            } else {
                flag = false;
            }
            additionalShift++;
        }
    }

}

void findAllKingTakingMovesForOne(GameSituation* situation, Color checkerColor, int checkerIndex) {
    if (situation->board.checkers[checkerColor][checkerIndex].type == King) {
        findAllKingTakingMovesForOneOnDir(situation, checkerColor, checkerIndex, NORTH_EAST);
        findAllKingTakingMovesForOneOnDir(situation, checkerColor, checkerIndex, NORTH_WEST);
        findAllKingTakingMovesForOneOnDir(situation, checkerColor, checkerIndex, SOUTH_EAST);
        findAllKingTakingMovesForOneOnDir(situation, checkerColor, checkerIndex, SOUTH_WEST);
    }


}

void findAllRegularTakingMoves(GameSituation* situation, Color forWhichSide) {
    for (int i = 0; i < situation->board.checkersCount[forWhichSide]; i++) {
        findAllTakingMovesForOne(situation, forWhichSide, i);
    }
}

void findAllKingTakingMoves(GameSituation* situation, Color forWhichSide, bool afterMove) {
    for (int i = 0; i < situation->board.checkersCount[forWhichSide]; i++) {
        findAllKingTakingMovesForOne(situation, forWhichSide, i);
    }
}

inline void findAllTakingSequencesForOne(GameSituation* situation, Checker* checker, int found);

inline void clearLastTakingSequence(GameSituation* situation) {
    situation->lastTakingSequence.tmsCount = 0;
}

/*inline void clearRegularMoveList(GameSituation* situation) {
    situation->rmCount = 0;
}

inline void clearTakingMoveList(GameSituation* situation) {
    situation->tmCount = 0;
}*/

inline void flushMoveBuffers(GameSituation* situation) {
    situation->tmBufferLen = 0;
    situation->rmBufferLen = 0;
   //clearRegularMoveList(situation);
    //clearTakingMoveList(situation);
    //situation->tmCount = 0;
    //situation->krmCount = 0;
    //situation->kbmCount = 0;
    //situation->rtmCount = 0;
    //situation->ktmCount = 0;
}




void removeChecker(Board* board, int index, Color color) {
    if (index < board->checkersCount[color]) {
        for (int i = index + 1; i < board->checkersCount[color];i++) {
            board->checkers[color][i - 1] = board->checkers[color][i];
        }
        board->checkersCount[color]--;
    }
}

void descendChecker(Checker* checker) {
    checker->type = Regular;
}

void ascendChecker(Checker* checker) {
    checker->type = King;
}

int makeAMove(GameSituation* situation, Move move) {
    Color movedColor = sideColor(situation, move.source.x, move.source.y);
    int movedIndex = getCheckerIndexByCoordsAndColor(situation, move.source.x, move.source.y, movedColor);
    situation->board.checkers[movedColor][movedIndex].coordinates.x = move.destination.x;
    situation->board.checkers[movedColor][movedIndex].coordinates.y = move.destination.y;
    if (move.isKingBecomingMove) ascendChecker(&(situation->board.checkers[movedColor][movedIndex]));
    //updateBoardRender(&(situation->board));
    updateBoardRender(&(situation->board));
    flushMoveBuffers(situation);
    if (move.isKingBecomingMove) {
        situation->lastKingBecomingMove = move;
        findAllKingTakingMovesForOne(situation, movedColor, movedIndex);
        if (situation->tmBufferLen == 0) {
            findAllKingMovesForOne(situation, movedColor, movedIndex);
            return 2;
        }
        return 1;
    } else return 0;
    //situation->board.boardRender[move.destination.y][move.destination.x] = situation->board.boardRender[move.source.y][move.source.x];
    //situation->board.boardRender[move.source.y][move.source.x] = EMPTY_BLACK;
}

void addToLastTakingSequence(GameSituation* situation, TakingMove move) {
    situation->lastTakingSequence.takingMoves[situation->lastTakingSequence.tmsCount++] = move;
}

int makeATakingMove(GameSituation* situation, TakingMove move) {

    Color movedColor = negateColor(move.takingSide);
    int movedIndex = getCheckerIndexByCoordsAndColor(situation, move.source.x, move.source.y, movedColor);
    Color victimColor = move.takingSide;
    int victimIndex = getCheckerIndexByCoordsAndColor(situation, move.victim.x, move.victim.y, victimColor);
    situation->board.checkers[movedColor][movedIndex].coordinates.x = move.destination.x;
    situation->board.checkers[movedColor][movedIndex].coordinates.y = move.destination.y;
    if (move.isASpecialMove) ascendChecker(&(situation->board.checkers[movedColor][movedIndex]));

    situation->board.checkers[victimColor][victimIndex].markedForDeath = true;
    flushMoveBuffers(situation);
    updateBoardRender(&(situation->board));
    if (situation->board.checkers[movedColor][movedIndex].type == King) {
        findAllKingTakingMovesForOne(situation, movedColor, movedIndex);
    }
    else {
        findAllTakingMovesForOne(situation, movedColor, movedIndex);
    }
    return situation->tmBufferLen;
    //if (situation->tmCount != 0) {
        //situation->lastTakingSequence.takingMoves[situation->lastTakingSequence.tmsCount++] = move;
    //    return -1;
    //}
    //removeChecker(&(situation->board), victimIndex, victimColor);
    //updateBoardRender(&(situation->board));

    //clearMoveLists(situation);
    //if (move.isASpecialMove) return 1; else return 0;
}



int cancelAMove(GameSituation* situation, Move move) {
    Color movedColor = sideColor(situation, move.destination.x, move.destination.y);
    int movedIndex = getCheckerIndexByCoordsAndColor(situation, move.destination.x, move.destination.y, movedColor);
    situation->board.checkers[movedColor][movedIndex].coordinates.x = move.source.x;
    situation->board.checkers[movedColor][movedIndex].coordinates.y = move.source.y;
    if (move.isKingBecomingMove) descendChecker(&(situation->board.checkers[movedColor][movedIndex]));
    updateBoardRender(&(situation->board));
    if (move.isKingBecomingMove) return -1; else return 0;
    //situation->board.boardRender[move.destination.y][move.destination.x] = situation->board.boardRender[move.source.y][move.source.x];
    //situation->board.boardRender[move.source.y][move.source.x] = EMPTY_BLACK;
}

int cancelATakingMove(GameSituation* situation, TakingMove move, bool emulated) {
    Color movedColor = negateColor(move.takingSide);
    int movedIndex = getCheckerIndexByCoordsAndColor(situation, move.destination.x, move.destination.y, movedColor);
    //Color victimColor = move.takingSide;
    //int victimIndex = getCheckerIndexByCoordsAndColor(situation, move.victim.x, move.victim.y, victimColor);
    situation->board.checkers[movedColor][movedIndex].coordinates.x = move.source.x;
    situation->board.checkers[movedColor][movedIndex].coordinates.y = move.source.y;
    if (move.isASpecialMove) descendChecker(&(situation->board.checkers[movedColor][movedIndex]));
    if (!emulated) {
        Checker regeneratedVictim;
        regeneratedVictim.coordinates.x = move.victim.x;
        regeneratedVictim.coordinates.y = move.victim.y;
        regeneratedVictim.type = move.victimType;
        regeneratedVictim.color = move.takingSide;
        regeneratedVictim.markedForDeath = false;
        situation->board.checkers[move.takingSide][situation->board.checkersCount[move.takingSide]++] = regeneratedVictim;
    }
    else {
        int falseVictimIndex = getCheckerIndexByCoordsAndColor(situation, move.victim.x, move.victim.y, move.takingSide);
        situation->board.checkers[move.takingSide][falseVictimIndex].markedForDeath = false;
    }
    //removeChecker(&(situation->board), victimIndex, victimColor);
    //updateBoardRender(&(situation->board));
    updateBoardRender(&(situation->board));
    //clearMoveLists(situation);
    if (move.isASpecialMove) return -1; else return 0;
}


void removeMarkedForDeath(GameSituation* situation, Color inWhere) {
    for (int i = 0; i < situation->board.checkersCount[inWhere]; i++) {
        if (situation->board.checkers[inWhere][i].markedForDeath) {
            removeChecker(&(situation->board), i, inWhere);
            i--;
        }
    }
    updateBoardRender(&(situation->board));
}

void findRegularMoveSequenceForOne(GameSituation* situation, Color checkerColor, int checkerIndex) {
    findAllKBMovesForOne(situation, checkerColor, checkerIndex);
    findAllRegularMovesForOne(situation, checkerColor, checkerIndex);
    findAllKingMovesForOne(situation, checkerColor, checkerIndex);

    if (situation->rmBufferLen != 0) {
        Move* buffer = new Move[situation->rmBufferLen];
        for (int i = 0; i < situation->rmBufferLen; i++) buffer[i] = situation->regularMovesBuffer[i];
        int savedRMs = situation->rmBufferLen;
        for (int i = 0; i < savedRMs; i++) {
            Move extracted = buffer[i];
            if (extracted.isKingBecomingMove) {
                //int insertIndex = situation->rmsCount;
                flushMoveBuffers(situation);
                int insertIndex;
                makeAMove(situation, extracted);
                if (situation->tmBufferLen == 0 && situation->rmBufferLen != 0) {
                    for (int j = 0; j < situation->rmBufferLen; j++) {
                        insertIndex = situation->rmsCount;
                        situation->regMoveSequences[insertIndex].regularMoves[0] = extracted;
                        situation->regMoveSequences[insertIndex].regularMoves[1] = situation->regularMovesBuffer[j];
                        situation->regMoveSequences[insertIndex].rmsCount = 2;
                        situation->rmsCount++;
                    }
                }
                cancelAMove(situation, extracted);
                //situation->regMoveSequences[insertIndex].regularMoves[situation->regMoveSequences[insertIndex].rmsCount++] = extracted;
                //situation->rmsCount++;
            }
            else {
                int insertIndex = situation->rmsCount;
                situation->regMoveSequences[insertIndex].regularMoves[0] = extracted; // maybe 0
                situation->regMoveSequences[insertIndex].rmsCount = 1;
                situation->rmsCount++;
            }
        }
        delete [] buffer;
    }
}

void findAllRegularMoveSequences(GameSituation* situation, Color forWhichSide) {
    for (int i = 0; i < situation->board.checkersCount[forWhichSide]; i++) {
        findRegularMoveSequenceForOne(situation, forWhichSide, i);
        flushMoveBuffers(situation);
    }
}

void makeARegMoveSequence(GameSituation* situation, RegMoveSequence regMoveSequence) {
    for (int i = 0; i < regMoveSequence.rmsCount; i++) makeAMove(situation, regMoveSequence.regularMoves[i]);
}
//over loadede for mized sequence
void findAllTakingSequencesForOne(GameSituation* situation, Color checkerColor, int checkerIndex, TakingSequence* currentPath, bool afterMove = false) {
    if (!afterMove) {
        findAllKingTakingMovesForOne(situation, checkerColor, checkerIndex);
        findAllTakingMovesForOne(situation, checkerColor, checkerIndex);
    }

    TakingMove* buffer = new TakingMove[situation->tmBufferLen];
    for (int i = 0; i < situation->tmBufferLen; i++) buffer[i] = situation->takingMovesBuffer[i];
    int savedTMs = situation->tmBufferLen;
    for (int i = 0; i < savedTMs; i++) {
        TakingMove extracted = buffer[i];
        appendToATakingSequence(currentPath, extracted);
        int stat = makeATakingMove(situation, extracted);
        if (stat == 0) {

                copyToAnotherTakingSequence(currentPath, &(situation->takingSequences[situation->tmsCount++]));

            //deleteLastFromATakingSequence(currentPath);
        }
        else {
            findAllTakingSequencesForOne(situation, checkerColor, checkerIndex, currentPath, true);
        }

        cancelATakingMove(situation, extracted, ENABLED_EMULATION);
        deleteLastFromATakingSequence(currentPath);
    }
    delete [] buffer;
}



void makeATakingMoveSequence(GameSituation* situation, TakingSequence takingSequence) {
    for (int i = 0; i < takingSequence.tmsCount; i++) makeATakingMove(situation, takingSequence.takingMoves[i]);

}

void findAllTakingMoveSequences(GameSituation* situation, Color forWhichSide) {
    for (int i = 0; i < situation->board.checkersCount[forWhichSide]; i++) {
        TakingSequence path = getNullPath();
        findAllTakingSequencesForOne(situation, forWhichSide, i, &path);
        flushMoveBuffers(situation);
    }
}

void findAllTakingSequencesForOne(GameSituation* situation, Color checkerColor, int checkerIndex, TakingSequence* currentPath, Move kingBecomingMoveToInsert, bool afterMove = false) {
    if (!afterMove) {
        findAllKingTakingMovesForOne(situation, checkerColor, checkerIndex);
        findAllTakingMovesForOne(situation, checkerColor, checkerIndex);
    }

    TakingMove* buffer = new TakingMove[situation->tmBufferLen];
    for (int i = 0; i < situation->tmBufferLen; i++) buffer[i] = situation->takingMovesBuffer[i];
    int savedTMs = situation->tmBufferLen;
    for (int i = 0; i < savedTMs; i++) {
        TakingMove extracted = buffer[i];
        appendToATakingSequence(currentPath, extracted);
        int stat = makeATakingMove(situation, extracted);
        if (stat == 0) {
            situation->mixedSequences[situation->mmsCount].kingBecomingMove = kingBecomingMoveToInsert;
            copyToAnotherTakingSequence(currentPath, &(situation->mixedSequences[situation->mmsCount++].takingSequence));
            //copyToAnotherTakingSequence(currentPath, &(situation->takingSequences[situation->tmsCount++]));

            //deleteLastFromATakingSequence(currentPath);
        }
        else {
            findAllTakingSequencesForOne(situation, checkerColor, checkerIndex, currentPath, true);
        }

        cancelATakingMove(situation, extracted, ENABLED_EMULATION);
        deleteLastFromATakingSequence(currentPath);
    }
    delete [] buffer;
}

void findAllMixedSequencesForOne(GameSituation* situation, Color checkerColor, int checkerIndex) {
    findAllKBMovesForOne(situation, checkerColor, checkerIndex);
    int savedTMs = 0;
    if (situation->rmBufferLen != 0) {
        Move* buffer = new Move[situation->rmBufferLen];
        for (int i = 0; i < situation->rmBufferLen; i++) buffer[i] = situation->regularMovesBuffer[i];
        int savedRMs = situation->rmBufferLen;
        for (int i = 0; i < savedRMs; i++) {
            Move extracted = buffer[i];
            situation->mixedSequences[situation->mmsCount].kingBecomingMove = extracted;
            if (extracted.isKingBecomingMove) {
                //situation->mixedSequences[situation->mmsCount].kingBecomingMove = extracted;
                //int insertIndex = situation->rmsCount;
                flushMoveBuffers(situation);
                //int insertIndex;
                makeAMove(situation, extracted);
                if (situation->tmBufferLen != 0) {
                    savedTMs = situation->tmBufferLen;
                    flushMoveBuffers(situation);
                    //insertIndex = situation->mmsCount;

                    TakingSequence null = getNullPath();
                    findAllTakingSequencesForOne(situation, checkerColor, checkerIndex, &null, extracted);
                    //situation->mmsCount++;
                }
                cancelAMove(situation, extracted);
                //situation->regMoveSequences[insertIndex].regularMoves[situation->regMoveSequences[insertIndex].rmsCount++] = extracted;
                //situation->rmsCount++;
            }
        }
        delete [] buffer;
    }
    //for (int i = savedTMs)
}

void findAllMixedSequences(GameSituation* situation, Color forWhichSide) {
    for (int i = 0; i < situation->board.checkersCount[forWhichSide]; i++) {
        //TakingSequence path = getNullPath();
        findAllMixedSequencesForOne(situation, forWhichSide, i);
        flushMoveBuffers(situation);
    }
}

void makeAMixedSequence(GameSituation* situation, MixedSequence mixedSequence) {
    makeAMove(situation, mixedSequence.kingBecomingMove);
    makeATakingMoveSequence(situation, mixedSequence.takingSequence);
}

inline void findAllMoves(GameSituation* situation, Color forWhichSide) {
    findAllTakingMoveSequences(situation, forWhichSide);
    findAllMixedSequences(situation, forWhichSide);
    //findAllKingTakingMoves(situation, forWhichSide);
    //findAllRegularTakingMoves(situation, forWhichSide);
    if (situation->tmsCount + situation->tmsCount == 0) {
        findAllRegularMoveSequences(situation, forWhichSide);
        //findAllKingBecomingMoves(situation, forWhichSide);
        //findAllKingMoves(situation, forWhichSide);
        //findAllRegularMoves(situation, forWhichSide);
    }
    //else {
    //    situation->board.checkersCount[negateColor(forWhichSide)]--;
    //}
    //findAllKingTakingMoves(situation, forWhichSide);
}

void clearAllSequencesLists(GameSituation* situation) {
    situation->rmsCount = 0;
    situation->tmsCount = 0;
}



/*void cancelLastTakingSequence(GameSituation* situation) {
    for (int i = situation->lastTakingSequence.tmsCount - 1; i > -1; i--) {
        cancelATakingMove(situation, situation->lastTakingSequence.takingMoves[i]);
    }
    clearLastTakingSequence(situation);
}*/

bool won(GameSituation* situation, Color who) {
    return situation->board.checkersCount[negateColor(who)] == 0;
}

int evaluateQuality(GameSituation* situation, Color forWhichSide) {
    int whiteEval = 0, blackEval = 0;
    if (won(situation, forWhichSide)) {
        return 10000000;
    }
    for (int i = 0; i < situation->board.checkersCount[White]; i++) {
        if (situation->board.checkers[White][i].type == King)  {
            whiteEval += kingCheckerCost * qualityFactor[situation->board.checkers[White][i].coordinates.y][situation->board.checkers[White][i].coordinates.x];
        }
        else {
            whiteEval += regularCheckerCost * qualityFactor[situation->board.checkers[White][i].coordinates.y][situation->board.checkers[White][i].coordinates.x];
        }
    }
    for (int i = 0; i < situation->board.checkersCount[Black]; i++) {
        if (situation->board.checkers[Black][i].type == King)  {
            blackEval += kingCheckerCost * qualityFactor[situation->board.checkers[Black][i].coordinates.y][situation->board.checkers[Black][i].coordinates.x];
        }
        else {
            blackEval += regularCheckerCost * qualityFactor[situation->board.checkers[Black][i].coordinates.y][situation->board.checkers[Black][i].coordinates.x];
        }
    }
    return (forWhichSide == White) ? whiteEval - blackEval : blackEval - whiteEval;
}



int analyysi(GameSituation* situation, Color side, int currentDepth, Difficulty maxDepth, SeqContainer* container) {
    //int eval = evaluateQuality(situation, side);
    // inf if blackss wind -inf else
    findAllMoves(situation, side);

    int bestMoveIndex = -1, bestMoveType = 0;
    if (situation->tmsCount + situation->mmsCount == 0) {
        RegMoveSequence* backup = new RegMoveSequence[situation->rmsCount];
        for (int i = 0; i < situation->rmsCount; i++) backup[i] = situation->regMoveSequences[i];
        int savedRMSS = situation->rmsCount;
        delete [] backup;
        // todo loop for regular
    }
    else {
        // todo lop for tak and mix
    }


    flushMoveBuffers(situation);
    findAllMoves(situation, negateColor(side));
}

SeqContainer getBestMove(GameSituation situation, Color side, Difficulty depth) {
    GameSituation copyOfGS = situation;
    SeqContainer bestMoveContainer; bestMoveContainer.eval = -10000000; bestMoveContainer.seqNumberToDo = -1;
    analyysi(&copyOfGS, side, 0, depth, &bestMoveContainer);
    return bestMoveContainer;

}

#endif CHECKERS_CLCHECKERS_H
