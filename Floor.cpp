/*
 * Copyright 2023 University of Michigan EECS183
 *
 * Floor.cpp
 * Project UID 848fee0125dbb5eb53ed294f20dbef81
 *
 * Yibei Zhai/Yancheng Xiang/Haohong Zheng/Lue Xu
 * yibei/ericxyc/haohongz/kkykky
 *
 * Final Project - Elevators
 */


#include "Floor.h"
#include <algorithm>

using namespace std;

int Floor::tick(int currentTime) {
    int explodeCount = 0;
    int explodeIndices[MAX_PEOPLE_PER_FLOOR];
    for (int i = 0; i < numPeople; i++) {
        if (people[i].tick(currentTime)) {
            explodeIndices[explodeCount] = i;
            explodeCount++;
        }
    }
    if (explodeCount > 0) {
        removePeople(explodeIndices, explodeCount);
    }
    return explodeCount;
}

void Floor::addPerson(Person newPerson, int request) {
    if (numPeople < MAX_PEOPLE_PER_FLOOR) {
        people[numPeople] = newPerson;
        numPeople++;
        if (request > 0) {
            hasUpRequest = true;
        }
        else if (request < 0) {
            hasDownRequest = true;
        }
    }
}

void Floor::removePeople(const int indicesToRemove[MAX_PEOPLE_PER_FLOOR],
                         int numPeopleToRemove) {
    //make a copy of the parameter to enable sorting
    int targetsToRemove[MAX_PEOPLE_PER_FLOOR] = {};
    
    //copy values from indicesToRemove to enable sorting
    for (int i = 0; i < numPeopleToRemove; i++){
        targetsToRemove[i] = indicesToRemove[i];
    }
    
    //sort the values in targetsToRemove from least to greatest
    sort(targetsToRemove, targetsToRemove + numPeopleToRemove);
    
    //now use targetsToRemove instead of indicesToRemove
    int removeCount = 0;
    for (int i = 0; i < numPeopleToRemove; i++) {
        for (int j = targetsToRemove[i] - removeCount; j < numPeople - 1; j++) {
            people[j] = people[j + 1];
        }
        removeCount++;
        numPeople--;
    }
    resetRequests();
}

void Floor::resetRequests() {
    hasUpRequest = false;
    hasDownRequest = false;
    for (int i = 0; i < numPeople; i++) {
        if (people[i].getTargetFloor() > people[i].getCurrentFloor()) {
            hasUpRequest = true;
        }
        else if (people[i].getTargetFloor() < people[i].getCurrentFloor()) {
            hasDownRequest = true;
        }
    }
}

//////////////////////////////////////////////////////
////// DO NOT MODIFY ANY CODE BENEATH THIS LINE //////
//////////////////////////////////////////////////////

Floor::Floor() {
    hasDownRequest = false;
    hasUpRequest = false;
    numPeople = 0;
}

void Floor::prettyPrintFloorLine1(ostream& outs) const {
	string up = "U";
	outs << (hasUpRequest ? up : " ") << " ";
	for (int i = 0; i < numPeople; ++i){
		outs << people[i].getAngerLevel();
		outs << ((people[i].getAngerLevel() < MAX_ANGER) ? "   " : "  ");
	}
	outs << endl;
}

void Floor::prettyPrintFloorLine2(ostream& outs) const {
	string down = "D";
	outs << (hasDownRequest ? down : " ") << " ";
	for (int i = 0; i < numPeople; ++i) {
		outs << "o   ";
	}
	outs << endl;
}

void Floor::printFloorPickupMenu(ostream& outs) const {
	cout << endl;
	outs << "Select People to Load by Index" << endl;
	outs << "All people must be going in same direction!";
	/*  O   O
	// -|- -|-
	//  |   |
	// / \ / \  */
	outs << endl << "              ";
	for (int i = 0; i < numPeople; ++i) {
		outs << " O   ";
	}
	outs << endl << "              ";
	for (int i = 0; i < numPeople; ++i) {
		outs << "-|-  ";
	}
	outs << endl << "              ";
	for (int i = 0; i < numPeople; ++i) {
		outs << " |   ";
	}
	outs << endl << "              ";
	for (int i = 0; i < numPeople; ++i) {
		outs << "/ \\  ";
	}
	outs << endl << "INDEX:        ";
	for (int i = 0; i < numPeople; ++i) {
		outs << " " << i << "   ";
	}
	outs << endl << "ANGER:        ";
	for (int i = 0; i < numPeople; ++i) {
		outs << " " << people[i].getAngerLevel() << "   ";
	}
	outs << endl << "TARGET FLOOR: ";
	for (int i = 0; i < numPeople; ++i) {
		outs << " " << people[i].getTargetFloor() << "   ";
	}
}

void Floor::setHasUpRequest(bool hasRequest) {
    hasUpRequest = hasRequest;
}

bool Floor::getHasUpRequest() const {
	return hasUpRequest;
}

void Floor::setHasDownRequest(bool hasRequest) {
    hasDownRequest = hasRequest;
}

bool Floor::getHasDownRequest() const {
	return hasDownRequest;
}

int Floor::getNumPeople() const {
    return numPeople;
}

Person Floor::getPersonByIndex(int index) const {
    return people[index];
}
