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

Event::Event(EventType eventType, double eventTime, double serviceTime, int location, long id) {
    this->eventTime = eventTime;
    this->serviceTime = serviceTime;
    this->eventType = eventType;
    this->eventLocation = location;
    this->customerID = id;
}

Event::Event(EventType eventType, double eventTime, int location, long id) {
    this->eventTime = eventTime;
    this->serviceTime = 0;
    this->eventType = eventType;
    this->eventLocation = location;
    this->customerID = id;
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

double Event::setServiceTime(double serviceTime) {
    Event::serviceTime = serviceTime;
    return Event::serviceTime;
}
double Event::getServiceTime() const {
    return serviceTime;
}

long Event::getCustomerId() const {
    return customerID;
}

long Event::setCustomerId(long customerID) {
    Event::customerID = customerID;
    return Event::customerID;
}

int Event::getEventLocation() const {
    return eventLocation;
}

int Event::setEventLocation(int eventLocation) {
    Event::eventLocation = eventLocation;
    return Event::eventLocation;
}

