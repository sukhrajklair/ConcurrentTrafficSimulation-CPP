#include <iostream>
#include <random>
#include <chrono>

#include "TrafficLight.h"
/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // The method receive use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
  	std::unique_lock<std::mutex> uLock(_mutex);
  	_cond.wait(uLock, [this]{return !(_messages.empty());});
  
  	T msg = std::move(_messages.back());
  	_messages.pop_back();
  
  	return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // The method send use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
  	std::lock_guard<std::mutex> lock(_mutex);
  	_messages.push_back(std::move(msg));
  	_cond.notify_one();
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{

    // infinite while-loop runs and repeatedly calls the receive function on the message queue. 
  	//receive is a blocking function and returns data only when it's available
    // Once it receives TrafficLightPhase::green, the method returns.
  	while(true){
      if(_trafficSignals.receive() == TrafficLightPhase::green)
        
        return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. 
  //the thread is then pushed into the threads vector in the base class. The base class joins all of the threads in its destructor
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // the function has an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update signal 
    // to the message queue using move semantics. The cycle duration is a random value between 4 and 6 seconds. 
    // Also, the while-loop uses std::this_thread::sleep_for to wait 1ms between two cycles. 
  	std::default_random_engine generator;
  	std::uniform_real_distribution<double> dist(4.0,6.0);
  	//initial start time and duration
  	auto start = std::chrono::high_resolution_clock::now();
  	double randomDuration = dist(generator);
  	while(true)
    {
      std::chrono::duration<double> timeElapsed = (std::chrono::high_resolution_clock::now() - start);
      double timeElapsedSeconds = timeElapsed.count();
      if(  timeElapsedSeconds > randomDuration ) {
        
      	//generate signal based on current signal
      	TrafficLightPhase signal;
        if (_currentPhase == TrafficLightPhase::green) 
          signal = TrafficLightPhase::red;
        else
          signal = TrafficLightPhase::green;
        
        //send signal
        _trafficSignals.send(std::move(signal));
        
        //reset start and duration
        start = std::chrono::high_resolution_clock::now();
        randomDuration = dist(generator);
      }
      
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
      
}
