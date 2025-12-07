/*
 * Copyright 2023 University of Michigan EECS183
 *
 * AI.cpp
 * Project UID 848fee0125dbb5eb53ed294f20dbef81
 *
 * Yibei Zhai/Yancheng Xiang/Haohong Zheng/Lue Xu
 * yibei/ericxyc/haohongz/kkykky
 *
 * Final Project - Elevators
 */

#include "AI.h"
#include <cassert>

// This file is used only in the Reach, not the Core.
// You do not need to make any changes to this file for the Core

string getAIMoveString(const BuildingState& buildingState) {
    // check if there are person in the building
    int totalNumPeople = 0;
    for (int f = 0; f < NUM_FLOORS; ++f) {
        totalNumPeople += buildingState.floors[f].numPeople;
    }
    if (totalNumPeople == 0) {
        // no person in elevator pass
        return "";
    }
    const int ANGER_WEIGHT = 2.5;
    const int DISTANCE_WEIGHT = 7.5;
    int bestElevator = -1;
    int bestFloor = -1;
    int bestScore = -1000000000;

    // loop every vacant elevator
    for (int e = 0; e < NUM_ELEVATORS; ++e) {

        _Elevator elevator = buildingState.elevators[e];

        // skip the servicing elevator
        if (elevator.isServicing) {
            continue;
        }

        // loop every floor for a non-servicing elevator
        for (int f = 0; f < NUM_FLOORS; ++f) {
            _Floor floor = buildingState.floors[f];

            // skip when no person in a floor
            if (floor.numPeople == 0) {
                continue;
            }
            
            // calc distance from the elevator to that floor
            int distance = elevator.currentFloor - floor.floorNum;
            if (distance < 0) {
                distance = -distance;
            }
            
            // sum of anger of that floor (excluding the person who is going to explode before elevator arrive
            int sumAnger = 0;

            for (int i = 0; i < floor.numPeople; ++i) {
                int angerNow = floor.people[i].angerLevel;

                // calc the increase of anger of that person  before e arrive
                int travelTicks = distance;

                int extraIncrements = 0;
                if (travelTicks > 0) {
                    extraIncrements =
                        ceil(travelTicks + TICKS_PER_ANGER_INCREASE - 1)
                        / TICKS_PER_ANGER_INCREASE;
                }

                int angerAtArrival = angerNow + extraIncrements;

                // skip if explode before arrive
                if (angerAtArrival >= MAX_ANGER) {
                    continue;
                }

                // else add the anger to sumanger
                sumAnger += angerNow;
            }

            // skip if sumanger is 0
            if (sumAnger == 0) {
                continue;
            }

            // a score for each floor relative for that elevator
            int score = sumAnger * ANGER_WEIGHT - distance * DISTANCE_WEIGHT;

            // assign the bestFloor with the floor with highest score for that elevator
            if (score > bestScore) {
                bestScore = score;
                bestElevator = e;
                bestFloor = floor.floorNum;
            }
        }
    }

    if (bestElevator == -1 || bestFloor == -1) {
        return "";
    }

    _Elevator chosen = buildingState.elevators[bestElevator];

    //if elevator at current floor -> pickup
    if (chosen.currentFloor == bestFloor) {
        string move = "e";
        move += char('0' + bestElevator);
        move += 'p';
        return move;
    }

    // else service move
    string move = "e";
    move += char('0' + bestElevator);
    move += 'f';
    move += char('0' + bestFloor);
    return move;
}

string getAIPickupList(const Move& move, const BuildingState& buildingState,
    const Floor& floorToPickup) {
    
    int numPeople = floorToPickup.getNumPeople();
    if (numPeople == 0) {
        return "";
    }
    
    // separate people into up and down groups
    string upIndices = "";
    string downIndices = "";
    int upAngers[MAX_PEOPLE_PER_FLOOR];
    int downAngers[MAX_PEOPLE_PER_FLOOR];
    int upCnt = 0;
    int downCnt = 0;
    
    int floorNum = floorToPickup.getPersonByIndex(0).getCurrentFloor();
    
    // group by direction
    for (int i = 0; i < numPeople; i++) {
        Person p = floorToPickup.getPersonByIndex(i);
        int target = p.getTargetFloor();
        int anger = p.getAngerLevel();
        
        if (target > floorNum) {
            upIndices += char('0' + i);
            upAngers[upCnt++] = anger;
        } else if (target < floorNum) {
            downIndices += char('0' + i);
            downAngers[downCnt++] = anger;
        }
    }
    
    // decide which group to pick from
    string chosenGroup = "";
    int chosenAngers[MAX_PEOPLE_PER_FLOOR];
    int chosenCnt = 0;
    
    if (upCnt == 0 && downCnt > 0) {
        // only down group exists
        chosenGroup = downIndices;
        for (int i = 0; i < downCnt; i++) {
            chosenAngers[i] = downAngers[i];
        }
        chosenCnt = downCnt;
    } else if (downCnt == 0 && upCnt > 0) {
        // only up group exists
        chosenGroup = upIndices;
        for (int i = 0; i < upCnt; i++) {
            chosenAngers[i] = upAngers[i];
        }
        chosenCnt = upCnt;
    } else if (upCnt > 0 && downCnt > 0) {
        // both groups exist - pick based on max anger
        int maxUp = 0;
        int maxDown = 0;
        
        for (int i = 0; i < upCnt; i++) {
            if (upAngers[i] > maxUp) maxUp = upAngers[i];
        }
        for (int i = 0; i < downCnt; i++) {
            if (downAngers[i] > maxDown) maxDown = downAngers[i];
        }
        
        if (maxUp > maxDown || (maxUp == maxDown && upCnt >= downCnt)) {
            chosenGroup = upIndices;
            for (int i = 0; i < upCnt; i++) {
                chosenAngers[i] = upAngers[i];
            }
            chosenCnt = upCnt;
        } else {
            chosenGroup = downIndices;
            for (int i = 0; i < downCnt; i++) {
                chosenAngers[i] = downAngers[i];
            }
            chosenCnt = downCnt;
        }
    } else {
        return "0";
    }
    
    // if group fits in elevator, take all
    if (chosenCnt <= ELEVATOR_CAPACITY) {
        return chosenGroup;
    }
    
    // otherwise pick top ELEVATOR_CAPACITY by anger
    string result = "";
    bool picked[MAX_PEOPLE_PER_FLOOR] = {false};
    
    for (int round = 0; round < ELEVATOR_CAPACITY; round++) {
        int bestAnger = -1;
        int bestIdx = -1;
        
        for (int i = 0; i < chosenCnt; i++) {
            if (!picked[i] && chosenAngers[i] > bestAnger) {
                bestAnger = chosenAngers[i];
                bestIdx = i;
            }
        }
        
        if (bestIdx != -1) {
            picked[bestIdx] = true;
            result += chosenGroup[bestIdx];
        }
    }
    
    return result;
}
