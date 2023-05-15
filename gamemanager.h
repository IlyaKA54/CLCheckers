//
// Created by Егор on 04.05.2023.
//
#include "clcengine/clcengine.h"


#ifndef CHECKERS_GAMEMANAGER_H
#define CHECKERS_GAMEMANAGER_H

Game currentGame; Color player; bool movesHaveBeenFound = false, moveHasBeenMade = false;
int numberOfCurrentTSMove = 0;
Coordinates lastChosen;

RegMoveSequence rmsForChosen[20]; TakingSequence tsForChosen[20]; MixedSequence msForChosen[20];
int sizeOfRMSFC = 0, sizeOfTSFC = 0, sizeOfMSFC = 0;

Coordinates coordinatesOfAvailableMoves[20]; int sizeOfCOAM = 0;



void addRMSToBuffer(RegMoveSequence* buffer, RegMoveSequence sequence) {
    buffer[sizeOfRMSFC++] = sequence;
}

void addTSToBuffer(TakingSequence* buffer, TakingSequence sequence) {
    buffer[sizeOfTSFC++] = sequence;
}

void addMSToBuffer(MixedSequence* buffer, MixedSequence sequence) {
    buffer[sizeOfMSFC++] = sequence;
}
//добавь карту путей как рендер доски но только с доступнымы для хода полями и жертвы
void flushBuffers() {
    sizeOfRMSFC = 0;
    sizeOfTSFC = 0;
    sizeOfMSFC = 0;
}

void addAnAvailableCoordinate(Coordinates* buffer, Coordinates what) {
    buffer[sizeOfCOAM++] = what;
}

void flushCOAMBuffer() {
    sizeOfCOAM = 0;
}

bool isAvailable(Coordinates* buffer, Coordinates what) {
    for (int i = 0; i < sizeOfCOAM; i++) {
        if (buffer[i].x == what.x && buffer[i].y == what.y) return true;
    }
    return false;
}

bool destinationInRMSBuffer(RegMoveSequence* buffer, Coordinates destination) {
    for (int i = 0; i < sizeOfRMSFC; i++) {
        if (isCoordinatesEqual(buffer[i].regularMoves[0].destination, destination)) return true;
    }
    return false;
}

bool destinationInTSBuffer(TakingSequence* buffer, Coordinates destination, int seqMove) {
    for (int i = 0; i < sizeOfTSFC; i++) {
        if (isCoordinatesEqual(buffer[i].takingMoves[seqMove].destination, destination)) return true;
    }
    return false;
}

bool destinationKBMInMSBuffer(MixedSequence* buffer, Coordinates destination) {
    for (int i = 0; i < sizeOfMSFC; i++) {
        if (isCoordinatesEqual(buffer[i].kingBecomingMove.destination, destination)) return true;
    }
    return false;
}

bool destinationTSInMSBuffer(MixedSequence* buffer, Coordinates destination, int seqMove) {
    for (int i = 0; i < sizeOfRMSFC; i++) {
        if (isCoordinatesEqual(buffer[i].takingSequence.takingMoves[seqMove].destination, destination)) return true;
    }
    return false;
}

int inWhatBufferIsDestination(Coordinates destination, int tsSeqMove) {
    if (destinationInRMSBuffer(rmsForChosen, destination)) return 1;
    else if (destinationInTSBuffer(tsForChosen, destination, tsSeqMove)) return 2;
    else if (destinationKBMInMSBuffer(msForChosen, destination)) return 3;
    //else if (destinationTSInMSBuffer())
    else return 0;
}


int getAvailableIndex(Coordinates* buffer, Coordinates what) {
    for (int i = 0; i < sizeOfCOAM; i++) {
        if (isCoordinatesEqual(buffer[i], what)) return i;
    }
    return -1;
}

Move extractMoveFromRSBuffer(RegMoveSequence* buffer, Coordinates destination) {
    for (int i = 0; i < sizeOfRMSFC; i++) {
        if (isCoordinatesEqual(buffer[i].regularMoves[0].destination, destination)) return buffer[i].regularMoves[0];
    }
}

Move extractMoveFromMSBuffer(MixedSequence* buffer, Coordinates destination) {
    for (int i = 0; i < sizeOfMSFC; i++) {
        if (isCoordinatesEqual(buffer[i].kingBecomingMove.destination, destination)) return buffer[i].kingBecomingMove;
    }
}

TakingMove extractTakingMoveFromTSBuffer(TakingSequence* buffer, Coordinates destination, int seqMove) {
    for (int i = 0; i < sizeOfTSFC; i++) {
        if (isCoordinatesEqual(buffer[i].takingMoves[seqMove].destination, destination)) return buffer[i].takingMoves[seqMove];
    }
}

bool isThereAnyOtherTakingMoves(TakingSequence* buffer, Coordinates fromDest, int seqMove) {
    for (int i = 0; i < sizeOfTSFC; i++) {
        if (buffer[i].tmsCount < seqMove) {
            if (isCoordinatesEqual(buffer[i].takingMoves[seqMove].destination, fromDest)) return true;
        }
    }
    return false;
}

SeqContainer getMove(GameSituation* situation, Coordinates source, Coordinates destination);
#endif //CHECKERS_GAMEMANAGER_H