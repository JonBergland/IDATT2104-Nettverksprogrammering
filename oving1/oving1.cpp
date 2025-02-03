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
        list<int> prime_list;
        mutex prime_list_mutex;

        list<int> found_list;
        mutex found_list_mutex;

        vector<thread> threads;

        int interval_length = higher - lower;
        int const range_length = interval_length / number_of_threads;
        cout << range_length << endl;
        
        int start = this->lower; 
        int end = start;
        for (size_t i = 1; i < this->number_of_threads + 1; i++)
        {
            start = end;
            end = (i == number_of_threads) ? this->higher : start + range_length;
            cout << "start og slutt for thread " << i << ": " << start << " " << end << endl;
            
            threads.emplace_back([&prime_list, &prime_list_mutex, 
            &found_list, &found_list_mutex, 
            i, this, start, end] {
                try 
                {
                    cout << "Thread " << i << " started " << endl;
                    for (size_t j = start; j < end; j++)
                    {
                        cout << "Thread " << i << " checking : " << j << endl;
                        bool inPrime = false;

                        for (auto a : prime_list) 
                        {
                            if (j % a == 0) 
                            {
                                inPrime = true;
                            }
                        };
                        
                        if (!inPrime) 
                        {
                            bool isPrime = true;
                            for (size_t i = 2; i < j; i++)
                            {
                                if (j % i == 0) {
                                    isPrime = false;
                                    unique_lock<mutex> lock(prime_list_mutex);
                                    prime_list.push_back(i);
                                    lock.unlock();
                                    
                                    if ((this->lower <= i) & ( i < this->higher)) {
                                        unique_lock<mutex> lock(found_list_mutex);
                                        bool found = (find(found_list.begin(), found_list.end(), i) != found_list.end());
                                        if (!found) {
                                             found_list.push_front(i);
                                        }
                                        lock.unlock();
                                    }
                                    break;
                                }
                            }
                            
                            if (isPrime) {
                                unique_lock<mutex> lock(found_list_mutex);
                                found_list.push_back(j);
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
}

oving1::~oving1()
{
}

int main() {
    int lower = 2;
    int upper = 100;
    int number_of_threads = 4;

    oving1 oving(lower, upper, number_of_threads);

    oving.generate_prime();
    
    return 0;
}