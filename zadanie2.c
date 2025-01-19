#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int account_balance = 0; // rachunek na koncie

pthread_mutex_t mutex; //mutex (marker)
pthread_cond_t cond; //zmienna warunkowa
pthread_cond_t balance; //zmienna warunkowa balance

const int NUM_OPERATIONS = 10; //liczba operacji
const int TRANSACTION_AMOUNT = 100; //kwota transakcji
int section_free = 1; // 1 - wolna, 0 - zajęta

int count = 0; // numeracja
void sig_function(int thread_id){
    section_free=1; //zwolnienie sekcji krytycznej
    if (thread_id <= 5){
        pthread_cond_broadcast(&balance);//sygnalizowanie innych watkow w kolejce 
    }
    pthread_cond_signal(&cond);
    
    pthread_mutex_unlock(&mutex);
    
}
int balance_checker_function(){
    if(account_balance<TRANSACTION_AMOUNT){
        return 0;
    }
    return 1;
}
void lock_function(int thread_id){
    pthread_mutex_lock(&mutex);
    while (!section_free){
        printf("%d Klient %d: Sekcja krytyczna jest zajeta, czekam...\n", count++, thread_id);
        pthread_cond_wait(&cond, &mutex); // zawisamy na zmiennej warunkowej (czekamy na zwolnienie sekcji krytycznej)
    }
    if( thread_id > 5) { //sprawdzamy konta, ktore dokonuja wyplaty 6-10
    while(!balance_checker_function()){
        printf("%d Klient %d: Brak pieniedzy na koncie, czekam...\n",  count++, thread_id);
        pthread_cond_wait(&balance, &mutex);//zawisamy na drugiek zmiennej warunkowej, az balans bedzie dodatni.
        }
    }
    section_free=0; // zajmowanie sekcji krytycznej
}

// Funkcja wplacajaca pieniadze na konto
void payin(void *arg) {
    int thread_id = *(int *)arg;
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        lock_function(thread_id);
        account_balance += TRANSACTION_AMOUNT;// Wplata na konto
        printf("%d Klient %d: Wplata %d jednostek. Stan konta: %d\n", count++, thread_id, TRANSACTION_AMOUNT, account_balance);
        sig_function(thread_id);//sygnalizowanie watkow
        usleep(rand() % 1000); // opoznienie
    }

}

// Funkcja wyplacajaca pieniadze z konta
void payout(void *arg) {
    int thread_id = *(int *)arg;

    for (int i = 0; i < NUM_OPERATIONS; i++) {
        lock_function(thread_id);
        account_balance -= TRANSACTION_AMOUNT;
        printf("%d Klient %d: Wyplata %d jednostek. Stan konta: %d\n", count++, thread_id, TRANSACTION_AMOUNT, account_balance);
        sig_function(thread_id);

        usleep(rand() % 1000); // Symulacja opoznienia
    }
}

int main() {
    int num_klients = 10; // ogolnie mamy 10 klientow: klient == watek
    pthread_t threads[num_klients];
    int thread_ids[num_klients];

    pthread_mutex_init(&mutex, NULL); 
    pthread_cond_init(&cond, NULL); 
    pthread_cond_init(&balance, NULL); 
    for (int i = 0; i < num_klients; i++) {
        thread_ids[i] = i + 1;
        if (i < num_klients / 2) {
             
            if (pthread_create(&threads[i], NULL, (void*) payin, &thread_ids[i])) {//0-4 - wplacaja
                perror("pthread_create() error");
                exit(1);
            }
        } else {
            if (pthread_create(&threads[i], NULL,  (void*) payout, &thread_ids[i])) {//5-9 - wyplacajaca
                perror("pthread_create() error");
                exit(1);
            }
            
        }
    }


    for (int i = 0; i < num_klients; i++) {
        if (pthread_join(threads[i], NULL) != 0){
        perror("pthread_join() error");
        exit(3);
        }
    }

    printf("Koniec programu. Ostateczny stan konta: %d\n", account_balance);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&balance);
    return 0;
}
