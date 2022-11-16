//
// Created by defur on 2022-11-14.
//

#include "Event.h"

bool operator<(const Event &e1, const Event &e2) {
    return e1.eventTime < e2.eventTime;
}

bool operator==(const Event &e1, const Event &e2) {
    return e1.eventTime == e2.eventTime;
}

bool operator>(const Event &e1, const Event &e2) {
    return e1.eventTime > e2.eventTime;
}

Event::Event(EventType eventType, double eventTime, int location) {
    this->eventTime = eventTime;
    this->eventType = eventType;
    this->eventLocation = eventLocation;
}

EventType Event::getEventType() const {
    return eventType;
}

EventType Event::setEventType(EventType eventType) {
    Event::eventType = eventType;
    return Event::eventType;
}

double Event::getEventTime() const {
    return eventTime;
}

double Event::setEventTime(double eventTime) {
    Event::eventTime = eventTime;
    return Event::eventTime;
}

long Event::getEventId() const {
    return eventID;
}

long Event::setEventId(long eventId) {
    eventID = eventId;
    return Event::eventID;
}

int Event::getEventLocation() const {
    return eventLocation;
}

int Event::setEventLocation(int eventLocation) {
    Event::eventLocation = eventLocation;
    return Event::eventLocation;
}

