#include <iostream>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <algorithm>
#include <unistd.h>
using namespace std;
#define MAX_THREADS 100

#define BURGER 0
#define FRIES 1
const char* type_names[] = {"BURGER", "FRIES"};
#define pii pair<int, int>

int k;
sem_t burger_sem, fries_sem;
mutex mtx;

int waiting_burgers = 0;
int waiting_fries = 0;
int last_order_type = -1;  // -1 indicates no last order

// Do not change
void process_order() {
    sleep(2);
}

void place_order(int type) {
    bool must_wait = (type == BURGER ? (sem_trywait(&burger_sem) != 0) : (sem_trywait(&fries_sem) != 0)); //(sem_trywait(&burger_sem) != 0) || (sem_trywait(&fries_sem) != 0);
    
    if (must_wait) {
        // Display waiting status
        mtx.lock();
        cout << "Waiting: " << type_names[type] << endl;
        mtx.unlock();

        // Now wait to acquire the semaphore
        if (type == BURGER) {
            ++waiting_burgers;
            sem_wait(&burger_sem); // P() Decrement burger semaphore
        } else {
            ++waiting_fries;
            sem_wait(&fries_sem);
        }
        last_order_type = type;
    }

    // Print the order placed message
    mtx.lock();
    cout << "Order: " << type_names[type] << endl;
    mtx.unlock();

    process_order(); // Process the order

    // After processing, release the semaphore
    if (type == BURGER) {
        while(waiting_burgers > 0) {
            sem_post(&burger_sem); // V() Signal burger semaphore
            --waiting_burgers;
        }
        sem_post(&burger_sem); // V() Signal burger semaphore
    } else {
        while (waiting_fries > 0) {
            sem_post(&fries_sem);
            --waiting_fries;
        }
        sem_post(&fries_sem);
    }
}

int main() {
    // Initialize necessary variables, semaphores etc.
    int t;
    cin >> k >> t;
    sem_init(&burger_sem, 0, k);  // Initialize burger semaphore with k
    sem_init(&fries_sem, 0, k);   // Initialize fries semaphore with k

    // Read data: done for you, do not change
    pii incoming[MAX_THREADS];
    int _type, _arrival;
    for (int i = 0; i < t; ++i) {
        cin >> _type >> _arrival;
        incoming[i].first = _type;
        incoming[i].second = _arrival;
    }

    // Create threads: done for you, do not change
    thread* threads[MAX_THREADS];
    for (int i = 0; i < t; ++i) {
        _type = incoming[i].first;
        threads[i] = new thread(place_order, _type);
        if (i < t - 1) {
            int _sleep = incoming[i + 1].second - incoming[i].second;
            sleep(_sleep);
        }
    }

    // Join threads: done for you, do not change
    for (int i = 0; i < t; ++i) {
        threads[i]->join();
        delete threads[i];
    }

    // Destroy the semaphores
    sem_destroy(&burger_sem);
    sem_destroy(&fries_sem);
    return 0;
}