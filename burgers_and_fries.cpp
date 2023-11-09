#include <iostream>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <unistd.h>
using namespace std;
#define MAX_THREADS 100

#define BURGER 0
#define FRIES 1
const char* type_names[] = {"BURGER", "FRIES"};
#define pii pair<int, int>

int k;
sem_t burger_sem;
sem_t fries_sem;
mutex mtx;
int last_order_type = -1;  // -1 indicates no last order

// Do not change
void process_order() {
    sleep(2);
}

void place_order(int type) {
    mtx.lock();
    if (type == BURGER) {
        sem_wait(&burger_sem);  // P() Decrement burger semaphore
        mtx.unlock();
        cout << "Order: BURGER" << endl;
    } else {
        sem_wait(&fries_sem);  //P()  Decrement fries semaphore
        mtx.unlock();
        cout << "Order: FRIES" << endl;
    }

    process_order();  // Process the order for 2 seconds

    mtx.lock();
    // Post-processing, increment the semaphore for the next order.
    // You will need to include logic to determine which type should be incremented based on your waiting logic.
    if (type == BURGER) {
        sem_post(&burger_sem); // V()
    } else {
        sem_post(&fries_sem); // V()
    }
    mtx.unlock();
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