#include <iostream>
#include <mutex>
#include <list>
#include <thread>
#include <algorithm>
#include <vector>
#include <bits/stdc++.h>


using namespace std;

// Task 1
// Find all prime numbers between two given numbers and with help of a given threads
// Output should be sorted list of all the prime numbers found

class oving1
{
private:
    int lower;
    int higher;
    int number_of_threads;
    list<int> prime_list;
    mutex prime_list_mutex;
    
public:
    oving1(int lower, int higher, int number_of_threads);

    void generate_prime() {
        list<int> interval_list = list<int>();
        mutex interval_list_mutex;

        list<int> prime_list;
        mutex prime_list_mutex;

        list<int> found_list;
        mutex found_list_mutex;

        vector<thread> threads;
        
        for (size_t i = lower; i < higher; i++)
        {
            interval_list.push_back(i);
        }

        int interval_length = interval_list.size();
        int const range_length = interval_length / number_of_threads;
        
        int start = 0; 
        int end = 0;
        for (size_t i = 0; i < this->number_of_threads; i++)
        {
            start = end;
            int end = (i == number_of_threads - 1) ? interval_length : start + range_length;
            
            threads.emplace_back([&interval_list, &interval_list_mutex, 
            &prime_list, &prime_list_mutex, 
            &found_list, &found_list_mutex, 
            i, this, start, end] {
                try 
                {
                    bool even = (i % 2 == 0);
                    while (interval_list.size() > 0)
                    {
                        cout << "Thread " << i << " started, size: " << interval_list.size() << endl;
                        unique_lock<mutex> lock(interval_list_mutex);
                        int element;
                        if (interval_list.size() > 0) {
                            if (even) {
                                element = interval_list.front();
                                interval_list.pop_front();
                            } else {
                                element = interval_list.back();
                                interval_list.pop_back();
                            }
                            lock.unlock();
                        } else {
                            break;
                        }

                        bool inPrime = false;

                        for (auto a : prime_list) 
                        {
                            if (element % a == 0) 
                            {
                                inPrime = true;
                            }
                        };
                        
                        if (!inPrime) 
                        {
                            bool isPrime = true;
                            for (size_t i = 2; i < element; i++)
                            {
                                if (element % i == 0) {
                                    isPrime = false;
                                    unique_lock<mutex> lock(prime_list_mutex);
                                    prime_list.push_back(i);
                                    lock.unlock();
                                    
                                    if ((this->lower <= i) & ( i < this->higher)) {
                                        unique_lock<mutex> lock(found_list_mutex);
                                        found_list.push_front(i);
                                        lock.unlock();
                                    }

                                    break;
                                }
                            }
                            
                            if (isPrime) {
                                unique_lock<mutex> lock(found_list_mutex);
                                found_list.push_back(element);
                                lock.unlock();

                            }
                        }
                    }

                    cout << "Thread " << i << " finished" << endl;
               
                } catch (const exception& e) {
                    cerr << "Exception in thread: " << e.what() << endl;
                } catch (...) {
                    cerr << "Unknown exception in thread" << endl;
                }
                 
            });

        }

        for (auto &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        found_list.sort();

        for (auto value : found_list)
        {
            cout << value << endl;
        };
        
        
        
    }

    ~oving1();
};

oving1::oving1(int lower, int higher, int number_of_threads) 
{
    this->lower = lower;
    this->higher = higher;
    this->number_of_threads = number_of_threads;
    this->prime_list = list<int>();
    cout << this->lower << endl;
}

oving1::~oving1()
{
}





int main() {
    int lower = 2;
    int upper = 100;
    int number_of_threads = 2;
    list<int> prime_list;


    oving1 oving(lower, upper, number_of_threads);

    oving.generate_prime();
    
    return 0;
}