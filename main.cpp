#include <iostream>
#include <cmath>
#include <queue>
#include <random>
#include "Event.h"

#define SERVER_COUNT 4;

using namespace std;

void initialization();
void processArrival(Event event);
void scheduleDeparture(Event event, int currentServer);
void processDeparture(Event event);
void reportGeneration();
double normal(normal_distribution<> normal);

double Clock, MeanInterArrivalTime, SIGMAInterArrivalTime, SumResponseTime, TotalBusy[4],
    SIGMA[4], MeanServiceTime[4], LastEventTime[4];
long NumberOfCustomers, QueueLength[3], NumberInService[4],
    TotalCustomers, NumberOfDepartures[4], LongService;

normal_distribution<> NormalDistributions[4]; // Server performance generators
priority_queue<Event> FutureEventList; // Future Events to process.
queue<Event> Queues[3]; // Customer in queues.

int main() {
    initialization();

    while (NumberOfDepartures[2] + NumberOfDepartures[3] < TotalCustomers) {
        Event evt = FutureEventList.top();
        FutureEventList.top();
        Clock = evt.getEventTime();

        if(evt.getEventType() == arrival)
            processArrival(evt);
        else
            processDeparture(evt);
    }

    reportGeneration();
    return 0;
}

void initialization() {
    MeanInterArrivalTime = 7.523;
    SIGMAInterArrivalTime = 10.244;
    SIGMA[0] = 2.018;
    SIGMA[1] = 2.583;
    SIGMA[2] = 3.135;
    SIGMA[3] = 5.192;
    MeanServiceTime[0] = 5.056;
    MeanServiceTime[1] = 4.740;
    MeanServiceTime[2] = 8.844;
    MeanServiceTime[3] = 10.204;
    Clock = 0.0;
    SumResponseTime = 0.0;

    for (int i = 0; i < 4; i++)
        NormalDistributions[i] = normal_distribution<>(MeanServiceTime[i], SIGMA[i]);

}

// Push the departure event into the future
void scheduleDeparture(Event event, int currentLocation) {
    double ServiceTime = normal(NormalDistributions[event.getEventLocation()]);
    Event depart = Event(departure, Clock+ServiceTime, currentLocation);
    FutureEventList.push(depart);
    NumberInService[currentLocation] = 1;
}

void processArrival(Event event) {
    int selectedQueue;

    for (int i = 0; i < 4; i++) {
        if (NumberInService[i] == 1) {
            TotalBusy[i] += Clock - LastEventTime[i];
        }
    }

    switch (event.getEventLocation()) {
        case 1:
            if (NumberInService[0] == 0)
                scheduleDeparture(event, 0);
            else if (NumberInService[1] == 0)
                scheduleDeparture(event, 1);
            break;
        case 2:
            if (NumberInService[2] == 0)
                scheduleDeparture(event, 2);
            break;
        case 3:
            if (NumberInService[3] == 0)
                scheduleDeparture(event, 3);
            break;

        default:
            event.setEventLocation(0);
            Queues[0].push(event);
            QueueLength[0]++;
            selectedQueue = 0;
            scheduleDeparture(event, 0);
    }

    cout << "Time: " << Clock <<" :\t customer " << event.getEventId() << " arrived! " << endl;
}

void processDeparture(Event event) {
    NumberInService[event.getEventLocation()]--;

    if (event.getEventLocation() == 0) {
         // create new arrival in appropriate queue
//         event.set;
         FutureEventList.push(event);
    }



    cout << "Time: " << Clock <<" :\t customer " << event.getEventId() << " finished from server 1" << endl;

}

double normal(normal_distribution<> normal) {
    random_device rd{};
    mt19937 gen{rd()};
    return normal(gen);
}

void reportGeneration() {

}

