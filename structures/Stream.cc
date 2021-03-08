//
// Created by Parti on 2021/2/23.
//

#include "structures/Stream.h"

using namespace drogon;
using namespace tech::structures;
using namespace std;

Stream::Stream(const int &uid) : BasePlayer(true), _uid(uid) {}

int Stream::getUid() const { return _uid; }

uint64_t Stream::getScore() const { return _score; }

void Stream::setScore(const uint64_t &score) { _score = score; }

uint64_t Stream::getSurvivalTime() const { return _survivalTime; }

void Stream::setSurvivalTime(const uint64_t &survivalTime) { _survivalTime = survivalTime; }

uint64_t Stream::getPlace() const { return _place; }

void Stream::setPlace(const uint64_t &place) { _place = place; }

bool Stream::getDead() const { return _dead; }

void Stream::setDead(const bool &dead) { _dead = dead; }
