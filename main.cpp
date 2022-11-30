#include <iostream>
#include <cmath>
#include <queue>
#include <random>
#include <iomanip>
#include <fstream>
#include "Event.h"

using namespace std;

void initialization();
void processArrival(Event event);
void scheduleDeparture(Event event, int currentServer);
void processDeparture(Event event);
void processQueueEntry(Event event, int queue);
void reportGeneration();
double normal(normal_distribution<> normal);

const int TotalCustomers = 100;
double Clock, MeanInterArrivalTime, SIGMAInterArrivalTime, TotalBusy[5],
        SIGMA[5], MeanServiceTime[5], TimeInQueue[3], TimeWaiting[3][TotalCustomers],
        TimeBeingServed[5][TotalCustomers], ArrivalTime[TotalCustomers], InterArrivalTime[TotalCustomers];
long QueueLength[3], NumberInService[5], NumberOfDepartures[5];

normal_distribution<> ArrivalDistribution; // Server performance generators
normal_distribution<> ServerDistributions[5]; // Server performance generators
priority_queue<Event, vector<Event>, greater<>> FutureEventList; // Future Events to process.
queue<Event> Queues[3]; // Customer in queues.
ofstream textoutfile;

int main() {
    initialization();
    while (NumberOfDepartures[3] + NumberOfDepartures[4] < TotalCustomers) {
        Event evt = FutureEventList.top();
        FutureEventList.pop();
        Clock = evt.getEventTime();

        if(evt.getEventType() == arrival)
            processArrival(evt);
        else
            processDeparture(evt);
    }

    reportGeneration();
    textoutfile.close();
    return 0;
}

void initialization() {
    textoutfile.open("out.txt");
    textoutfile << fixed;
    textoutfile << setprecision(2);

    MeanInterArrivalTime = 7.523;
    SIGMAInterArrivalTime = 10.244;
    SIGMA[0] = 2.018;
    SIGMA[1] = 2.583;
    SIGMA[2] = (SIGMA[0] + SIGMA[1]) / 2; // New Server
    SIGMA[3] = 3.135;
    SIGMA[4] = 5.192;
    MeanServiceTime[0] = 5.056;
    MeanServiceTime[1] = 4.740;
    MeanServiceTime[2] = (MeanServiceTime[0] + MeanServiceTime[1]) / 2; // New Server
    MeanServiceTime[3] = 8.844;
    MeanServiceTime[4] = 10.204;
    Clock = 0.0;
    ArrivalDistribution = normal_distribution<>(MeanInterArrivalTime, SIGMAInterArrivalTime);

    for (int i = 0; i < 5; i++)
        ServerDistributions[i] = normal_distribution<>(MeanServiceTime[i], SIGMA[i]);

    double lastTime = 0;
    for (int i = 0; i < TotalCustomers; i++) {
        double interArrivalTime = normal(ArrivalDistribution);
        Event event(arrival, lastTime += interArrivalTime, 0, i);
        ArrivalTime[i] = event.getEventTime();
        InterArrivalTime[i] = interArrivalTime;
        FutureEventList.push(event);
    }
}

// Push the departure event into the future
// Depart from 'currentServer'
void scheduleDeparture(Event event, int currentServer) {
    double ServiceTime = normal(ServerDistributions[currentServer]);
    TotalBusy[currentServer] += ServiceTime;
    TimeBeingServed[currentServer][event.getCustomerId()] += ServiceTime;
    Event depart = Event(departure, Clock+ServiceTime, ServiceTime, currentServer, event.getCustomerId());
    FutureEventList.push(depart);
    NumberInService[currentServer] = 1;
}

void processArrival(Event event) {
    // Process arrival in a queue

    bool queue = false;
    // Determine where the customer is coming from and going to
    int location = event.getEventLocation();

    // Which Queue?
    switch (location) {
        case 0:
            if (NumberInService[0] == 0) {
                event.setEventLocation(0);
                scheduleDeparture(event, 0);
            } else if (NumberInService[1] == 0) {
                event.setEventLocation(1);
                scheduleDeparture(event, 1);
            } else if (NumberInService[2] == 0) {
                event.setEventLocation(2);
                scheduleDeparture(event, 2);
            }
            else {
                queue = true;
                processQueueEntry(event, location);
            }
            break;
        case 1:
        case 2:
            if (NumberInService[location + 2] == 0) {
                event.setEventLocation(location+2);
                scheduleDeparture(event, location + 2);
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

    if (!queue)
        textoutfile << "Time: " << Clock <<" :\t customer " << event.getCustomerId() << " arrived at server " << event.getEventLocation() << endl;
}

void processQueueEntry(Event event, int queue) {
    // Put customer into queue
    event.setEventLocation(queue);
    Queues[queue].push(event);
    textoutfile << "Time: " << Clock <<" :\t customer " << event.getCustomerId() << " arrived at queue " << event.getEventLocation() << endl;
}

void processDeparture(Event event) {
    // Process departure from a server

    NumberInService[event.getEventLocation()]--;
    NumberOfDepartures[event.getEventLocation()]++;
    int location = event.getEventLocation();
    int queueChoice = 0;

    // Which Server?
    switch (location) {
        case 0:
            FutureEventList.push(Event(arrival, Clock, 1, event.getCustomerId()));
            if (!Queues[0].empty()) {
                Event evt = Queues[0].front();
                Queues[0].pop();
                TimeInQueue[0] += Clock - evt.getEventTime();
                TimeWaiting[0][evt.getCustomerId()] += Clock - evt.getEventTime();
                processArrival(evt);
            }
            break;
        case 1:
            FutureEventList.push(Event(arrival, Clock, 2, event.getCustomerId()));
            if (!Queues[0].empty()) {
                Event evt = Queues[0].front();
                Queues[0].pop();
                TimeInQueue[0] += Clock - evt.getEventTime();
                TimeWaiting[0][evt.getCustomerId()] += Clock - evt.getEventTime();
                processArrival(evt);
            }
            break;
        case 2:
            // Put the event in the shorter queue
            queueChoice = (Queues[1].size() > Queues[2].size()) ? 2 : 1;
            if (NumberInService[3] ^ NumberInService[4]) // XOR
                queueChoice = (NumberInService[3] > NumberInService[4]) ? 2 : 1; // Choose queue for empty server
            FutureEventList.push(Event(arrival, Clock, queueChoice, event.getCustomerId()));

            // Get Next Customer For This Server from appropriate queue and process arrival
            if (!Queues[0].empty()) {
                Event evt = Queues[0].front();
                Queues[0].pop();
                TimeInQueue[0] += Clock - evt.getEventTime();
                TimeWaiting[0][evt.getCustomerId()] += Clock - evt.getEventTime();
                processArrival(evt);
            }
            break;
        case 3:
        case 4:
            if (!Queues[location - 2].empty()) {
                Event evt = Queues[location - 2].front();
                Queues[location - 2].pop();
                TimeInQueue[location - 2] += Clock - evt.getEventTime();
                TimeWaiting[location - 2][evt.getCustomerId()] += Clock - evt.getEventTime();
                processArrival(evt);
            }
        default:
            break;
    }

    textoutfile << "Time: " << Clock <<" :\t customer " << event.getCustomerId() << " finished from server " << event.getEventLocation() << " after " << event.getServiceTime()  << endl;
}

double normal(normal_distribution<> normal) {
    random_device rd{};
    mt19937 gen{rd()};
    return abs(normal(gen));
}

template<class T>
T sum_array(T* array, int j) {
    T sum;
    while(j > 0) {
        sum += array[j];
        j--;
    }
    return sum;
}

void reportGeneration() {
    for (int i = 0; i < 5; i++)
        textoutfile << "Server " << i << " Utilization: " << TotalBusy[i] << ", " << 100 * (TotalBusy[i] / Clock) << "%"
                    << endl;
    for (int i = 0; i < 5; i++)
        textoutfile << "Server " << i << " Customers Served: " << NumberOfDepartures[i] << ", " << 100 * ((float)NumberOfDepartures[i] / TotalCustomers) << "%"
                    << endl;

    double totalWaitingTime = 0;
    for (int i = 0; i < 3; i++) {
        textoutfile << "Total Time In Queue " << i << ": \t\t" << TimeInQueue[i] << endl;
        totalWaitingTime += TimeInQueue[i];
    }
    textoutfile << "Average Time Spent Waiting: " << totalWaitingTime / TotalCustomers << endl;
    textoutfile << "Average Time Being Served:  " << sum_array(TotalBusy, 4) / TotalCustomers << endl;
    textoutfile << "Average Time In System:     " << (totalWaitingTime + sum_array(TotalBusy, 4)) / TotalCustomers << endl;

    ofstream outfile;
    outfile.open("out.csv");
    outfile << "Customer, Arrival Time, InterArrival Time, Queue 0 Waiting Time, Queue 1 Waiting Time, Queue 2 Waiting Time, ";
    outfile << "Server 0 Time, Server 1 Time, Server 2 Time, Server 3 Time, Server 4 Time";
    outfile << endl;
    for (int i = 0; i < TotalCustomers; i++) {
        outfile << i << "," << ArrivalTime[i] << "," << InterArrivalTime[i] << "," << TimeWaiting[0][i] << "," << TimeWaiting[1][i] << "," << TimeWaiting[2][i] << ",";
        outfile << TimeBeingServed[0][i] << "," << TimeBeingServed[1][i] << "," << TimeBeingServed[2][i] << "," << TimeBeingServed[3][i] << "," << TimeBeingServed[4][i] << ",";
        outfile << endl;
    }
    outfile.close();

}
