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
    SIGMA[4], MeanServiceTime[4], LastEventTime;
long NumberOfCustomers, QueueLength[3], NumberInService[4],
    TotalCustomers, NumberOfDepartures[4], LongService;

normal_distribution<> ArrivalDistribution; // Server performance generators
normal_distribution<> ServerDistributions[4]; // Server performance generators
priority_queue<Event, vector<Event>, greater<Event>> FutureEventList; // Future Events to process.
queue<Event> Queues[3]; // Customer in queues.

int main() {
    initialization();

    while (NumberOfDepartures[2] + NumberOfDepartures[3] < TotalCustomers) {
        Event evt = FutureEventList.top();
        FutureEventList.pop();
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
    ArrivalDistribution = normal_distribution<>(MeanInterArrivalTime, SIGMAInterArrivalTime);

    for (int i = 0; i < 4; i++)
        ServerDistributions[i] = normal_distribution<>(MeanServiceTime[i], SIGMA[i]);

    for (int i = 0; i < 5; i++) {
        Event firstEvent(arrival, normal(ArrivalDistribution), 0, TotalCustomers);
        TotalCustomers++;
        FutureEventList.push(firstEvent);
    }


}

// Push the departure event into the future
void scheduleDeparture(Event event, int currentLocation) {
    double ServiceTime = normal(ServerDistributions[event.getEventLocation()]);
    Event depart = Event(departure, Clock+ServiceTime, currentLocation, event.getCustomerId());
    FutureEventList.push(depart);
    NumberInService[currentLocation] = 1;
}

void processArrival(Event event) {
    int selectedQueue;

    // Update the total busy time for each server.
    for (int i = 0; i < 4; i++) {
        if (NumberInService[i] == 1) {
            TotalBusy[i] += Clock - LastEventTime;
        }
    }

    // Determine where the customer is coming from and going to
    switch (event.getEventLocation()) {
        case 0:
            if (NumberInService[0] == 0)
                scheduleDeparture(event, 0);
            else if (NumberInService[1] == 0)
                scheduleDeparture(event, 1);
            break;
        case 1:
            if (NumberInService[2] == 0)
                scheduleDeparture(event, 2);
            break;
        case 2:
            if (NumberInService[3] == 0)
                scheduleDeparture(event, 3);
            break;
        default:
            // At start of system
            event.setEventLocation(0);
            Queues[0].push(event);
            QueueLength[0]++;
            selectedQueue = 0;
            scheduleDeparture(event, 0);
    }

    LastEventTime = Clock;

    cout << "Time: " << Clock <<" :\t customer " << event.getCustomerId() << " arrived in queue " << event.getEventLocation() << endl;
}

void processDeparture(Event event) {
    NumberInService[event.getEventLocation()]--;

    NumberOfDepartures[event.getEventLocation()] ++ ;

    switch (event.getEventLocation()) {
        case 0:
            FutureEventList.push(Event(arrival, Clock, 1, event.getCustomerId()));
        case 1:
            FutureEventList.push(Event(arrival, Clock, 2, event.getCustomerId()));
        default:
            break;
    }



    cout << "Time: " << Clock <<" :\t customer " << event.getCustomerId() << " finished from server " << event.getEventLocation() << endl;

}

double normal(normal_distribution<> normal) {
    random_device rd{};
    mt19937 gen{rd()};
    return abs(normal(gen));
}

void reportGeneration() {

}

