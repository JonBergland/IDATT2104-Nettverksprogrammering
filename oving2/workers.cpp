#include <condition_variable>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <functional>
#include <atomic>

using namespace std ;

class Workers
{
private:
    int num_workers;
    bool wait = true;
    atomic<bool> stop_flag = false;
    mutex wait_mutex;
    mutex work_list_mutex;
    condition_variable cv;
    vector<thread> threads;
    list<function<void()>> work_list;
public:
    Workers(int num_workers);

    void start() {
         try
        {
            
            for (size_t i = 0; i < this->num_workers; i++)
            {
                cout << "Create thread" << endl;
                this->threads.emplace_back(
                    ([this, i] {
                        try {
                            cout << "Thread " << i << " started" << endl;
                            while (!this->stop_flag) {
                                {
                                    unique_lock<mutex> wait_lock (this->wait_mutex);
                                    if (this->wait)
                                        this->cv.wait(wait_lock);
                                }
                                
                                unique_lock<mutex> list_lock (this->work_list_mutex);

                                if (!work_list.empty()) {
                                    function<void()> func = work_list.front();
                                    work_list.pop_front();
                                    list_lock.unlock();
                                    
                                    func();
                                } else {
                                    this->stop();
                                    list_lock.unlock();
                                }
                            }
                            cout << "Thread " << i << " finished" << endl;
                    } catch(const std::exception& e) {
                        std::cerr << e.what() << '\n';
                    }
                }));
            }
        } catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        } catch(...) {
            cerr << "Unkown exception in thread" << endl;
        }
         this_thread::sleep_for(chrono::milliseconds(1000));
    }

    void post(function<void()> func) {
        this->stop_flag = false;
        unique_lock<mutex> lock (this->work_list_mutex);
        work_list.emplace_back(func);
        cout << "Adding work" << endl;
        cv.notify_one();
    }

    void post_timeout(function<void()> func, int timeout_length_ms) {
        this->post([&func, &timeout_length_ms] {
            this_thread::sleep_for(chrono::milliseconds(timeout_length_ms));
            func();
        });

    }

    void join() {
        {
            cout << "Not Waiting" << endl;
            unique_lock<mutex> lock (this->wait_mutex);
            this->wait = false;
        }

        cout << "Joining threads" << endl;
        cv.notify_all();
        for (auto &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void stop() {
        if (!this->stop_flag.load()) {
            this->stop_flag.store(true);
        }
        // cv.notify_all();
    }

    ~Workers();
};

Workers::Workers(int num_workers) : num_workers(num_workers) {
   
}

Workers::~Workers()
{
}


int main() {
    try
    {
        int number_of_workers = 2;
        Workers workers(number_of_workers);
        Workers event_loop(1);


        workers.start();
        event_loop.start();

        workers.post([] {
            this_thread::sleep_for(1s);
            cout << "First work task finished" << endl;
        });

        workers.post([] {
            this_thread::sleep_for(1s);
            cout << "Second work task finished" << endl;
        });

        workers.post_timeout([] {
            cout << "First task finished" << endl;
        }, 2000);

        workers.post_timeout([] {
            cout << "Second task finished" << endl;
        }, 1000);

        workers.post_timeout([] {
            cout << "Third task finished" << endl;
        }, 4000);

        workers.post_timeout([] {
            cout << "Fourth task finished" << endl;
        }, 2000);

        event_loop.post_timeout([] {
            cout << "Event First task finished" << endl;
        }, 1000);

        event_loop.post_timeout([] {
            cout << "Event Second task finished" << endl;
        }, 1000);

        workers.join();
        event_loop.join();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
