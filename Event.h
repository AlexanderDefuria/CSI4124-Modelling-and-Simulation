//
// Created by defur on 2022-11-14.
//

#ifndef PROJECTMS2_EVENT_H
#define PROJECTMS2_EVENT_H

enum EventType {arrival, departure};

class Event {
    friend bool operator<(const Event& e1, const Event& e2);
    friend bool operator>(const Event& e1, const Event& e2);
    friend bool operator==(const Event& e1, const Event& e2);

protected:
    EventType eventType;
    double eventTime;
    long customerID;
    long eventLocation;

public:
    Event()= default;
    Event(EventType eventType, double eventTime, int eventQueue, long id);

    double getEventTime() const;
    double setEventTime(double eventTime);
    EventType getEventType() const;
    EventType setEventType(EventType eventType);
    long getCustomerId() const;
    long setCustomerId(long customerID);
    int getEventLocation() const;
    int setEventLocation(int eventLocation);
};


#endif //PROJECTMS2_EVENT_H
