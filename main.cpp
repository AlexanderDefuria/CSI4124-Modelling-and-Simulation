#include <iostream>
#include <cmath>
#include <queue>
#include <random>
#include <iomanip>
#include "Event.h"

using namespace std;

void initialization();
void processArrival(Event event);
void scheduleDeparture(Event event, int currentServer);
void processDeparture(Event event);
void processQueueEntry(Event event, int queue);
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
    cout << setprecision(4);

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

    for (int i = 0; i < 10; i++) {
        Event firstEvent(arrival, normal(ArrivalDistribution), 0, TotalCustomers);
        TotalCustomers++;
        FutureEventList.push(firstEvent);
    }


}

// Push the departure event into the future
void scheduleDeparture(Event event, int currentServer) {
    double ServiceTime = normal(ServerDistributions[currentServer]);
    TotalBusy[currentServer] += ServiceTime;
    Event depart = Event(departure, Clock+ServiceTime, ServiceTime, currentServer, event.getCustomerId());
    FutureEventList.push(depart);
    NumberInService[currentServer] = 1;
}

void processArrival(Event event) {
    bool queue = false;

    // Determine where the customer is coming from and going to
    int location = event.getEventLocation();

    switch (location) {
        case 0:
            if (NumberInService[0] == 0)
                scheduleDeparture(event, 0);
            else if (NumberInService[1] == 0) {
                event.setEventLocation(1);
                scheduleDeparture(event, 1);
            }
            else {
                queue = true;
                processQueueEntry(event, location);
            }
            break;
        case 1:
        case 2:
            if (NumberInService[location + 1] == 0) {
                scheduleDeparture(event, location + 1);
            }
            else {
                queue = true;
                processQueueEntry(event, location);
            }
            break;
        default:
            // At start of system
            event.setEventLocation(0);
            Queues[0].push(event);
            QueueLength[0]++;
            scheduleDeparture(event, 0);
    }

    LastEventTime = Clock;


    if (!queue)
        cout << "Time: " << Clock <<" :\t customer " << event.getCustomerId() << " arrived at server " << event.getEventLocation() << endl;
}

void processQueueEntry(Event event, int queue) {
    event.setEventLocation(queue);
    Queues[queue].push(event);
    cout << "Time: " << Clock <<" :\t customer " << event.getCustomerId() << " arrived at queue " << event.getEventLocation() << endl;
}

void processDeparture(Event event) {
    NumberInService[event.getEventLocation()]--;

    NumberOfDepartures[event.getEventLocation()]++;

    int location = event.getEventLocation();

    switch (location) {
        case 0:
            FutureEventList.push(Event(arrival, Clock, 1, event.getCustomerId()));
            if (!Queues[0].empty()) {
                Event evt = Queues[0].front();
                Queues[0].pop();
                processArrival(evt);
            }
            break;
        case 1:
            FutureEventList.push(Event(arrival, Clock, 2, event.getCustomerId()));
            if (!Queues[0].empty()) {
                Event evt = Queues[0].front();
                Queues[0].pop();
                processArrival(evt);
            }
            break;
        case 2:
        case 3:
            if (!Queues[location - 1].empty()) {
                Event evt = Queues[location - 1].front();
                Queues[location - 1].pop();
                processArrival(evt);
            }
        default:
            break;
    }

    cout << "Time: " << Clock <<" :\t customer " << event.getCustomerId() << " finished from server " << event.getEventLocation() << " after " << event.getServiceTime()  << endl;
}

double normal(normal_distribution<> normal) {
    random_device rd{};
    mt19937 gen{rd()};
    return abs(normal(gen));
}

void reportGeneration() {
    for (int i = 0; i < 4; i++)
        cout << "Server " << i << " Utilization: " << TotalBusy[i] << ", " << 100 * (TotalBusy[i] / Clock) << "%" << endl;
}

