#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_BUS_NUMBER 4
#define MAX_STATIONS_NUMBER 10
#define START_STATIONS_NUMBER 3
#define BUFFER_SIZE 256

typedef struct Station Station;
struct Station{
    int id;
    int x;
    int y;
    int capacity;
    int travelers_nb;
};

typedef struct Player Player;
struct Player{
    int id;
    int money;
    int SB_upgrades;
    int SP_upgrades;
    int CT_upgrades;
    int end;
};

typedef struct Traveler Traveler;
struct Traveler{
    int id;
    int ids1;
    int ids2;
    int id_bus;
    Traveler *next;
};

typedef struct Bus Bus;
struct Bus{
    int id;
    int owner_player_id;
    int x;
    int y;
    int station_id;
    int num_cars;
    int availability;
};

typedef struct TravelersList TravelersList;
struct TravelersList{
    Traveler *first_traveler;
};

void print_station(Station station){

    fprintf(stderr, "IDST=%d, ", station.id);
    fprintf(stderr, "X=%d, ", station.x);
    fprintf(stderr, "Y=%d, ", station.y);
    fprintf(stderr, "K=%d, ", station.capacity);
    fprintf(stderr, "NBT=%d\n", station.travelers_nb);

}

void print_all_stations(Station station[], int travelers_num){

    fprintf(stderr, "ALL STATIONS : \n");
    for(int i = 0; i < travelers_num; i++){
        print_station(station[i]);
    }
}

void print_player(Player player){

    fprintf(stderr, "IDP=%d, ", player.id);
    fprintf(stderr, "M=%d, ", player.money);
    fprintf(stderr, "SB=%d, ", player.SB_upgrades);
    fprintf(stderr, "SP=%d, ", player.SP_upgrades);
    fprintf(stderr, "CT=%d, ", player.CT_upgrades);
    fprintf(stderr, "END=%d\n", player.end);

}

void print_bus(Bus bus){

    fprintf(stderr, "IDB=%d, ", bus.id);
    fprintf(stderr, "IDP=%d, ", bus.owner_player_id);
    fprintf(stderr, "X=%d, ", bus.x);
    fprintf(stderr, "Y=%d, ", bus.y);
    fprintf(stderr, "IDST=%d, ", bus.station_id);
    fprintf(stderr, "NC=%d, ", bus.num_cars);
    fprintf(stderr, "AV=%d\n", bus.availability);

}

void print_all_bus(Bus bus[], int bus_num){

    fprintf(stderr, "MY BUS : \n");
    for(int i = 0; i < bus_num; i++){
        print_bus(bus[i]);
    }
}

void print_traveler(Traveler traveler){

    fprintf(stderr, "IDT=%d, ", traveler.id);
    fprintf(stderr, "IDS1=%d, ", traveler.ids1);
    fprintf(stderr, "IDS2=%d, ", traveler.ids2);
    fprintf(stderr, "ID_BUS=%d\n", traveler.id_bus);

}

void check_malloc(void *pointer){
    if (pointer == NULL){
        fprintf(stderr, "Cannot allocate initial memory for data\n");
        exit(EXIT_FAILURE);
    }
}

void add_traveler(TravelersList *travelers_list, Traveler *new_traveler){
    new_traveler->next = travelers_list->first_traveler;
    travelers_list->first_traveler = new_traveler;
}

void print_travelers_list(TravelersList *travelers_list){

    fprintf(stderr, "ALL TRAVELERS : \n");
    Traveler *actual_traveler = travelers_list->first_traveler;

    while (actual_traveler != NULL){
        print_traveler(*actual_traveler);
        actual_traveler = actual_traveler->next;
    }
}

void delete_travelers_at_dest(TravelersList *travelers_list, int id_traveler){

    Traveler *previous_traveler,*actual_traveler;
    actual_traveler=travelers_list->first_traveler;
    previous_traveler = actual_traveler;

    while(actual_traveler != NULL){

        if(actual_traveler->id == id_traveler){

            if(previous_traveler == actual_traveler){

                travelers_list->first_traveler = actual_traveler->next;
                free(actual_traveler);

            }else{

                previous_traveler->next=actual_traveler->next;
                free(actual_traveler);
            }
            break;
        }

        previous_traveler = actual_traveler;
        actual_traveler = actual_traveler->next;
    }
}

void update_travelers_in_bus(TravelersList *travelers_list, Station stations_array[], int id_traveler, int id_bus){

    Traveler *actual_traveler = travelers_list->first_traveler;

    while (actual_traveler != NULL){

        if(actual_traveler->id == id_traveler){
            actual_traveler->id_bus = id_bus;
            stations_array[actual_traveler->ids1].travelers_nb--;
        }

        actual_traveler = actual_traveler->next;
    }
}

// get bus with bus_array[bus_id] and station with stations_array[station_id]
int bus_arrived(Bus bus, Station station){

    if(bus.x == station.x && bus.y == station.y){
        return 1;
    }else{
        return 0;
    }
}

/*
int is_bus_full(Bus bus, Traveler* travelers, int travelers_nb){
    int travelers_in_bus_number = 0;
    for(int i = 0; i < travelers_nb; i++){
        if(travelers[i].id_bus == bus.id){
            travelers_in_bus_number++;
        }
    }
    if(travelers_in_bus_number == )
}
 */


Traveler *get_all_travelers_in_bus(Bus bus, Traveler *travelers, int travelers_num, int *travelers_in_bus_num){

    *travelers_in_bus_num = 0;
    Traveler* travelers_in_bus_array = malloc(0 * sizeof(Traveler *));
    check_malloc(travelers_in_bus_array);

    for(int i = 0; i < travelers_num; i++){

        if(travelers[i].id_bus == bus.id){

            //increase_travelers_array(&travelers_in_bus_array, *travelers_in_bus_num, 1);
            travelers_in_bus_array[*travelers_in_bus_num] = travelers[i];
            *travelers_in_bus_num = *travelers_in_bus_num + 1;
        }
    }

    return travelers_in_bus_array;
}

/* comparison function for qsort */
int int_compare(const void *a, const void *b){

    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return *ia  - *ib;
}

int get_the_most_popular_station(TravelersList *travelers_list, const int size, int bus_id){

    int count, max_val = 0, index = -1, station_id_array[size], freq[size];
    Traveler *actual_traveler = travelers_list->first_traveler;
    int counter = 0;

    while (actual_traveler != NULL){

        if(actual_traveler->id_bus == bus_id){
            station_id_array[counter] = actual_traveler->ids2;
            freq[counter] = -1;
            counter++;
        }

        actual_traveler = actual_traveler->next;
    }

    qsort(station_id_array, counter, sizeof(int), int_compare);


    for(int i=0; i < counter; i++){

        count = 1;
        for(int j=i+1; j < counter; j++){
            // If duplicate element is found
            if(station_id_array[i] == station_id_array[j]){
                count++;
                freq[j] = 0; // Make sure not to count frequency of same element again
            }
        }
        // If frequency of current element is not counted
        if(freq[i] != 0){
            freq[i] = count;
        }
    }

    for (int i = 0; i < counter; ++i) {
        if (freq[i] > max_val ) {
            max_val = freq[i];
            index = i;
        }
    }
    if(index != -1){
        return index;
    }else{
        return station_id_array[index];
    }
}

int main(void){

    int players_num, travelers_list_size = 0, stations_num = START_STATIONS_NUMBER, my_player_id, round = 0;

    srand(time(NULL));

    scanf("%d", &players_num);
    scanf("%d", &my_player_id);

    Player players_array[players_num];
    Station stations_array[MAX_STATIONS_NUMBER];

    TravelersList* travelers_list = malloc(sizeof(*travelers_list));
    check_malloc(travelers_list);

    travelers_list->first_traveler = NULL;

    for(int i = 0; i < stations_num; i++){
        scanf("%d%d%d%d",
                &stations_array[i].id,
                &stations_array[i].x,
                &stations_array[i].y,
                &stations_array[i].capacity);
        stations_array[i].travelers_nb = 0;
    }

    while(1){

        for(int i = 0; i < players_num; i++){

            scanf("%d%d%d%d%d%d",
                    &players_array[i].id,
                    &players_array[i].money,
                    &players_array[i].SB_upgrades,
                    &players_array[i].SP_upgrades,
                    &players_array[i].CT_upgrades,
                    &players_array[i].end);
        }

        int new_station;
        scanf("%d", &new_station);

        if(new_station){

            scanf("%d%d%d%d",
                    &stations_array[stations_num].id,
                    &stations_array[stations_num].x,
                    &stations_array[stations_num].y,
                    &stations_array[stations_num].capacity);
            stations_array[stations_num].travelers_nb = 0;

            stations_num++;
        }

        int bus_num, my_bus_num = 0;
        Bus my_bus_array[MAX_BUS_NUMBER];
        scanf("%d", &bus_num);

        for(int i = 0; i < bus_num; i++){

            scanf("%d%d%d%d%d%d",
                    &my_bus_array[my_bus_num].id,
                    &my_bus_array[my_bus_num].owner_player_id,
                    &my_bus_array[my_bus_num].x,
                    &my_bus_array[my_bus_num].y,
                    &my_bus_array[my_bus_num].station_id,
                    &my_bus_array[my_bus_num].num_cars);
        }

        int new_travelers_num, travelers_in_bus_num, travelers_at_dest_num;
        scanf("%d%d%d", &new_travelers_num, &travelers_in_bus_num, &travelers_at_dest_num);

        if(new_travelers_num > 0){

            for(int i = 0; i < new_travelers_num; i ++){

                Traveler *new_traveler = malloc(sizeof(*new_traveler));
                check_malloc(new_traveler);

                scanf("%d%d%d", &new_traveler->id, &new_traveler->ids1, &new_traveler->ids2);
                new_traveler->id_bus = -1;

                stations_array[new_traveler->ids1].travelers_nb++; //Update station1 travelers number
                add_traveler(travelers_list, new_traveler);
                travelers_list_size++;
            }
        }

        if(travelers_in_bus_num > 0){

            for (int i = 0; i < travelers_in_bus_num; i++){

                int IDT, IDB;
                scanf("%d%d", &IDT, &IDB);

                update_travelers_in_bus(travelers_list, stations_array, IDT, IDB);
                //fprintf(stderr, "TRAVELER %d IS NOW IN THE BUS %d\n", IDT, IDB);

            }
        }

        if(travelers_at_dest_num > 0){

            for(int i = 0; i < travelers_at_dest_num; i++){
                int IDT;
                scanf("%d", &IDT);
                delete_travelers_at_dest(travelers_list, IDT);
                fprintf(stderr, "TRAVELER %d DELETED FROM LIST\n", IDT);
            }
        }

        print_all_bus(my_bus_array, my_bus_num);
        print_all_stations(stations_array, stations_num);
        print_travelers_list(travelers_list);
        fprintf(stderr, "LIST SIZE : %d\n", travelers_list_size);

        //get_the_most_popular_station(travelers_list, travelers_list_size, 0);

        // ----- IA starts here -----

        char command_buffer[BUFFER_SIZE];

        /*
        if(my_bus_num == 0){
            fprintf(stderr, "NO BUS, SO I ADD ONE\n");
            snprintf(command_buffer, BUFFER_SIZE, "BUS %d", (rand() % stations_num));

        }else{

            if(my_bus_num < MAX_BUS_NUMBER && players_array[my_player_id].money >= 100){
                fprintf(stderr, "NEW BUS BUY\n");
                snprintf(command_buffer, BUFFER_SIZE,"BUS %d", (rand() % stations_num));
            }else{

                for(int i = 0; i < my_bus_num; i++){

                    if(my_bus_array[i].availability == 1){
                        snprintf(command_buffer, BUFFER_SIZE, "DESTINATION %d %d", my_bus_array[i].id, (rand() % stations_num));
                        my_bus_array[i].availability = 0;
                        fprintf(stderr, "BUS %d GOES TO A NEW RANDOM ST AND IS NOW UNAVAILABLE : %d\n", my_bus_array[i].id, my_bus_array[i].availability);
                    }else{
                        fprintf(stderr, "BUS %d IS UNAVAILABLE (GOING TO ST %d)\n", my_bus_array[i].id ,my_bus_array[i].station_id);
                        snprintf(command_buffer, BUFFER_SIZE,"PASS");
                    }
                }
            }
        }
         */

        snprintf(command_buffer, BUFFER_SIZE,"PASS");

        printf("%s\n", command_buffer);

        // ----- IA ends here -----

        round++;

        fflush(stdout);
    }
}