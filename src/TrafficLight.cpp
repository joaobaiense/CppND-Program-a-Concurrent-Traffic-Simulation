#include <iostream>
#include "TrafficLight.h"

// Initialize Static Members
std::random_device rd;
std::mt19937 TrafficLight::eng(rd());
std::uniform_int_distribution<uint32_t> TrafficLight::randDist(TrafficLight::cycleMin, TrafficLight::cycleMax);


/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

    std::unique_lock<std::mutex> mut(m_mut);

    // wait for a new message
    m_cond.wait(mut);

    // move the message
    TrafficLightPhase tmp = std::move(m_queue.front());
    m_queue.pop_front();

    // pop the message 
    m_queue.pop_front();

    // unlock 
    mut.unlock();

    return tmp;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    std::unique_lock<std::mutex> mut(m_mut);

    // push to the queue 
    m_queue.emplace_back(std::move(msg));

    // unlock 
    mut.unlock();

    // notify 
    m_cond.notify_one();

}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    m_currentPhase = TrafficLightPhase::LIGHT_RED;

}

TrafficLight::~TrafficLight(){
    // destructor
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.

    TrafficLightPhase tmp;

    while(true){

        tmp =  m_queue.receive();

        if (tmp == TrafficLightPhase::LIGHT_GREEN){
            break;
        }
    }

    // std::cout << "GREEEN!!! returning now" << std::endl; 
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return m_currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 


    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    uint32_t timeSinceLastUpdate;
    uint32_t cycleDelay = 0; 


    // init stop watch
    lastUpdate = std::chrono::system_clock::now();

    std::string msg;

    // initialize cycleDelay
    cycleDelay = randDist(eng);

    while(true){

        timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDelay) {

            // cycle the light 
            if (m_currentPhase == TrafficLightPhase::LIGHT_GREEN) {
                m_currentPhase = TrafficLightPhase::LIGHT_RED;

                msg = "Current light phase is: RED";

            } else { 
                m_currentPhase = TrafficLightPhase::LIGHT_GREEN;

                msg = "Current light phase is: GREEN";
            }

            // std::cout << msg;

            TrafficLightPhase next = m_currentPhase;
            m_queue.send(std::move(next));

            // Set the next cycle delay
            cycleDelay = randDist(eng);

            // std::cout << " Light with delay: " << std::to_string(cycleDelay) << std::endl;

            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }

        // finally, sleep for 1 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

