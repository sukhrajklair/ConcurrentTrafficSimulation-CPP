#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase
{
  red,
  green
};

//the following tempelate class implements a Concurrent Message Queue
template <class T>
class MessageQueue
{
public:
	T receive();
    void send( T && msg);
private:
	std::mutex _mutex;
    std::condition_variable _cond;
	std::deque<T> _messages;
};



class TrafficLight: public TrafficObject
{
public:
    // constructor / desctructor
	TrafficLight();
    // getters / setters
	TrafficLightPhase getCurrentPhase();
    // typical behaviour methods
	void waitForGreen();
    void simulate();
private:
    // typical behaviour methods
	void cycleThroughPhases();
    //MessageQueue stores the traffic signals
    //each siganl is pushed into the queue and retreived in another thread
	MessageQueue<TrafficLightPhase> _trafficSignals;
    std::condition_variable _condition;
    std::mutex _mutex;
   	TrafficLightPhase _currentPhase;
};

#endif