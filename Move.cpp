/*
 * Copyright 2023 University of Michigan EECS183
 *
 * Move.cpp
 * Project UID 848fee0125dbb5eb53ed294f20dbef81
 *
 * <#Names#>
 * <#Uniqnames#>
 *
 * Final Project - Elevators
 */
 
#include <cmath>
#include <sstream>
#include <stdio.h>      
#include <stdlib.h>
#include "Move.h"

using namespace std;

Move::Move(string commandString) : Move() {
    // Pass move: empty string
    if (commandString.empty()) {
        isPass = true;
        return;
    }

    // Game moves: save or quit
    if (commandString.size() == 1) {
        char c = commandString[0];
        if (c == 'S' || c == 's') {
            isSave = true;
        }
        else if (c == 'Q' || c == 'q') {
            isQuit = true;
        }
        return;
    }

    // Service or pickup move: "e<id>f<floor>" or "e<id>p"
    stringstream ss(commandString);
    char prefix;
    ss >> prefix;

    if (prefix == 'e' || prefix == 'E') {
        int id;
        char action;
        ss >> id >> action;
        elevatorId = id;

        if (action == 'f' || action == 'F') {
            int floor;
            ss >> floor;
            targetFloor = floor;
        }
        else if (action == 'p' || action == 'P') {
            isPickup = true;
        }
    }
}

bool Move::isValidMove(Elevator elevators[NUM_ELEVATORS]) const {
    // Pass, save, and quit moves are always valid
    if (isPass || isSave || isQuit) {
        return true;
    }

    // For pickup and service moves, elevatorId must be in range
    if (elevatorId < 0 || elevatorId >= NUM_ELEVATORS) {
        return false;
    }

    // Elevator must not already be servicing a request
    if (elevators[elevatorId].isServicing()) {
        return false;
    }

    // Pickup moves are valid if we reach here
    if (isPickup) {
        return true;
    }

    // Service moves: check targetFloor
    if (targetFloor < 0 || targetFloor >= NUM_FLOORS) {
        return false;
    }

    // Service move must go to a different floor
    if (targetFloor == elevators[elevatorId].getCurrentFloor()) {
        return false;
    }

    return true;
}

void Move::setPeopleToPickup(const string& pickupList,
                             const int currentFloor,
                             const Floor& pickupFloor) {
    numPeopleToPickup = 0;
    totalSatisfaction = 0;

    int maxDistance = -1;
    int furthestTarget = currentFloor;

    for (int i = 0; i < static_cast<int>(pickupList.size()); ++i) {
        int index = pickupList[i] - '0';

        if (numPeopleToPickup < MAX_PEOPLE_PER_FLOOR) {
            peopleToPickup[numPeopleToPickup] = index;
            ++numPeopleToPickup;
        }

        const Person& person = pickupFloor.getPersonByIndex(index);

        int anger = person.getAngerLevel();
        totalSatisfaction += (MAX_ANGER - anger);

        int personTarget = person.getTargetFloor();
        int distance = abs(personTarget - currentFloor);

        if (distance > maxDistance) {
            maxDistance = distance;
            furthestTarget = personTarget;
        }
    }

    targetFloor = furthestTarget;
}

//////////////////////////////////////////////////////
////// DO NOT MODIFY ANY CODE BENEATH THIS LINE //////
//////////////////////////////////////////////////////

Move::Move() {
    elevatorId = -1;
    targetFloor = -1;
    numPeopleToPickup = 0;
    totalSatisfaction = 0;
	isPass = false;
    isPickup = false;
    isSave = false;
    isQuit = false;
}

bool Move::isPickupMove() const {
    return isPickup;
}

bool Move::isPassMove() const {
	return isPass;
}

bool Move::isSaveMove() const {
	return isSave;
}

bool Move::isQuitMove() const {
	return isQuit;
}

int Move::getElevatorId() const {
    return elevatorId;
}

int Move::getTargetFloor() const {
    return targetFloor;
}

int Move::getNumPeopleToPickup() const {
    return numPeopleToPickup;
}

int Move::getTotalSatisfaction() const {
    return totalSatisfaction;
}

void Move::setTargetFloor(int inTargetFloor) {
    targetFloor = inTargetFloor;
}

void Move::copyListOfPeopleToPickup(int newList[MAX_PEOPLE_PER_FLOOR]) const {
    for (int i = 0; i < numPeopleToPickup; ++i) {
        newList[i] = peopleToPickup[i];
    }
}
