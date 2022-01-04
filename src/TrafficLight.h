#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"
#include <random>

// forward declarations to avoid include cycle
class Vehicle;


// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:

    std::deque<T> m_queue;
    std::condition_variable m_cond;
    std::mutex m_mut;
    
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

enum TrafficLightPhase {
    LIGHT_UNKNOWN,
    LIGHT_RED,
    LIGHT_GREEN
};


class TrafficLight : TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate() override;
    

private:
    // typical behaviour methods
    void cycleThroughPhases();

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.
    MessageQueue<TrafficLightPhase> m_queue; 

    std::condition_variable m_cond;
    std::mutex m_mutex;

    TrafficLightPhase m_currentPhase = TrafficLightPhase::LIGHT_UNKNOWN; 

    // Setup the random distribution
    static std::mt19937 eng; //(rd());

    const static uint32_t cycleMin = 4000; // 4 seconds 
    const static uint32_t cycleMax = 6000; // 6 seconds

    static std::uniform_int_distribution<uint32_t> randDist; //(cycleMin, cycleMax);

};





#endif