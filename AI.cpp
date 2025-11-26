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
    // 先检查整栋楼是否有人在等电梯
    int totalNumPeople = 0;
    for (int f = 0; f < NUM_FLOORS; ++f) {
        totalNumPeople += buildingState.floors[f].numPeople;
    }
    if (totalNumPeople == 0) {
        // 楼里没人等电梯，直接 pass
        return "";
    }

    int bestElevator = -1;
    int bestFloor = -1;
    int bestScore = -1000000000;  // 很小的初始值

    // 枚举所有“空闲电梯”
    for (int e = 0; e < NUM_ELEVATORS; ++e) {

        _Elevator elevator = buildingState.elevators[e];

        // 正在 servicing 的电梯先不管
        if (elevator.isServicing) {
            continue;
        }

        // 对这个电梯，枚举所有楼层
        for (int f = 0; f < NUM_FLOORS; ++f) {
            _Floor floor = buildingState.floors[f];

            // 没有人在这一层，跳过
            if (floor.numPeople == 0) {
                continue;
            }

            // 找出这一层“最生气”的人
            int maxAngerOnFloor = 0;
            for (int i = 0; i < floor.numPeople; ++i) {
                int anger = floor.people[i].angerLevel;
                if (anger > maxAngerOnFloor) {
                    maxAngerOnFloor = anger;
                }
            }

            // 计算距离（手写绝对值，避免额外头文件）
            int distance = elevator.currentFloor - floor.floorNum;
            if (distance < 0) {
                distance = -distance;
            }

            // 综合分数：愤怒越高越好，距离越近越好
            int score = maxAngerOnFloor * 10 - distance;

            // 维护全局最优
            if (score > bestScore) {
                bestScore = score;
                bestElevator = e;
                bestFloor = floor.floorNum;
            }
        }
    }

    // 如果没有找到可用的 (电梯, 楼层) 组合：
    // 可能是所有电梯都在忙，或者虽然楼里有人但都已被安排目标。
    if (bestElevator == -1 || bestFloor == -1) {
        return "";
    }

    const _Elevator& chosen = buildingState.elevators[bestElevator];

    // 如果电梯已经在目标楼层 -> pickup
    if (chosen.currentFloor == bestFloor) {
        string move = "e";
        move += char('0' + bestElevator);  // 电梯编号（0,1,2）
        move += 'p';
        return move;
    }

    // 否则发一个 service move 让电梯去该楼层
    string move = "e";
    move += char('0' + bestElevator);
    move += 'f';
    move += char('0' + bestFloor);         // 楼层编号（0~9）
    return move;
}

string getAIPickupList(const Move& move, const BuildingState& buildingState, 
                       const Floor& floorToPickup) {
    int numPeople = floorToPickup.getNumPeople();
        if (numPeople == 0) {
            return "";
        }

        string upList;
        string downList;
        int maxAngerUp = -1;
        int maxAngerDown = -1;

        // 1. 按方向分组，顺便记录每组的最大愤怒值
        for (int i = 0; i < numPeople; ++i) {
            const Person& p = floorToPickup.getPersonByIndex(i);
            int current = p.getCurrentFloor();
            int target = p.getTargetFloor();
            int anger = p.getAngerLevel();
            int diff = target - current;

            if (diff > 0) {
                // 想上楼
                upList += char('0' + i);
                if (anger > maxAngerUp) {
                    maxAngerUp = anger;
                }
            }
            else if (diff < 0) {
                // 想下楼
                downList += char('0' + i);
                if (anger > maxAngerDown) {
                    maxAngerDown = anger;
                }
            }
            // diff == 0 的人基本不会出现，忽略
        }

        string result;

        // 所有人 diff 都是 0 或其他奇怪情况 -> 随便接一个 0 号兜底
        if (upList.empty() && downList.empty()) {
            result += '0';
            return result;
        }

        // 2. 决定选上行组还是下行组
        if (upList.empty()) {
            // 只有下行
            result = downList;
        }
        else if (downList.empty()) {
            // 只有上行
            result = upList;
        }
        else {
            // 两组都有：先比最大愤怒
            if (maxAngerUp > maxAngerDown) {
                result = upList;
            }
            else if (maxAngerDown > maxAngerUp) {
                result = downList;
            }
            else {
                // 最大愤怒相同，选人数更多的那一边
                if (upList.size() >= downList.size()) {
                    result = upList;
                }
                else {
                    result = downList;
                }
            }
        }

        // 再兜一层底：保证至少接一个人
        if (result.empty()) {
            result += '0';
        }

        return result;
}
