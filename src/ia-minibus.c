#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_BUS_NUMBER 4
#define MAX_STATIONS_NUMBER 10
#define START_STATIONS_NUMBER 3

#define BUFFER_SIZE 256

#define MOVING 1
#define STOPPED 2
#define AVAILABLE 3

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
    int state;
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
    fprintf(stderr, "AV=%d\n", bus.state);
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

void delete_travelers_at_dest(TravelersList *travelers_list, int *travelers_list_size, int id_traveler){

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

            *travelers_list_size = *travelers_list_size - 1;
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

int get_most_crowded_station(Station stations_array[], int stations_num){

    int max_travelers_nb = -1, index = -1;

    for(int i = 0; i < stations_num; i++){

        if (stations_array[i].travelers_nb > max_travelers_nb) {
            max_travelers_nb = stations_array[i].travelers_nb;
            index = i;
        }
    }

    if(index == -1 || max_travelers_nb == 0){
        return -1;
    }else{
        return stations_array[index].id;
    }
}

/* comparison function for qsort */
int int_compare(const void *a, const void *b){

    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return *ia  - *ib;
}

int is_bus_moving(Bus bus_array[], Station stations_array[], int bus_num, int stations_num) {

    int dest_station_id;
    for (int i = 0; i < bus_num; i++) {

        dest_station_id = bus_array[i].station_id;
        for (int j = 0; j < stations_num; j++) {

            if (dest_station_id == stations_array[j].id && bus_array[j].state != STOPPED) {
                return (bus_array[j].x == stations_array[j].x && bus_array[j].y == stations_array[j].y);
            }
        }
    }
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
    if(index == -1){
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

        int my_bus_num = 0;

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

        int bus_num;
        Bus bus_array[MAX_BUS_NUMBER];
        scanf("%d", &bus_num);

        for(int i = 0; i < bus_num; i++){

            scanf("%d%d%d%d%d%d",
                    &bus_array[i].id,
                    &bus_array[i].owner_player_id,
                    &bus_array[i].x,
                    &bus_array[i].y,
                    &bus_array[i].station_id,
                    &bus_array[i].num_cars);

            if(bus_array[i].owner_player_id == my_player_id){
                my_bus_num++;
            }
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
            }
        }

        if(travelers_at_dest_num > 0){

            for(int i = 0; i < travelers_at_dest_num; i++){
                int IDT;
                scanf("%d", &IDT);
                delete_travelers_at_dest(travelers_list, &travelers_list_size, IDT);
            }
        }

        print_all_bus(bus_array, bus_num);
        print_all_stations(stations_array, stations_num);
        print_travelers_list(travelers_list);
        fprintf(stderr, "LIST SIZE : %d\n", travelers_list_size);

        // ----- IA starts here -----

        //Upgrades
        if (my_bus_num < 4 && players_array[my_player_id].money >= 100){

            int most_crowded_station_for_spawn = get_most_crowded_station(stations_array, stations_num);
            fprintf(stderr, "MOST CROWDED STATION 4 SPAWN : %d\n", most_crowded_station_for_spawn);

            if(most_crowded_station_for_spawn != -1){
                printf("BUS %d; ", most_crowded_station_for_spawn);
            }

        }else if (my_bus_num == 4 && players_array[my_player_id].SP_upgrades < 2 && players_array[my_player_id].money >= 200){
            printf("UPDATESP; ");

        }else if (my_bus_num == 4 && players_array[my_player_id].SP_upgrades == 2 && players_array[my_player_id].CT_upgrades < 5 && players_array[my_player_id].money >= 100){
            printf("UPDATECT; ");
        }

        for (int i = 0; i < bus_num; i++) {

            if (bus_array[i].owner_player_id == my_player_id) {

                if (bus_array[i].state == MOVING && is_bus_moving(bus_array, stations_array, bus_num, stations_num) == 0) {
                    bus_array[i].state = STOPPED;

                } else if (bus_array[i].state == AVAILABLE) {

                    int most_popular_station = get_the_most_popular_station(travelers_list, travelers_list_size, bus_array[i].id);

                    if (most_popular_station == -1) {

                        int most_crowded_station = get_most_crowded_station(stations_array, stations_num);
                        fprintf(stderr, "MOST CROWDED STATION : %d\n", most_crowded_station);
                        printf("DESTINATION %d %d; ", bus_array[i].id, most_crowded_station);

                    } else {
                        fprintf(stderr, "MOST POPULAR STATION : %d\n", most_popular_station);
                        printf("DESTINATION %d %d; ", bus_array[i].id, most_popular_station);
                    }

                    bus_array[i].state = MOVING;

                } else if (bus_array[i].state != MOVING && bus_array[i].state != AVAILABLE) {
                    bus_array[i].state = AVAILABLE;
                }
            }
        }

        printf("PASS\n");

        // ----- IA ends here -----

        round++;

        fflush(stdout);
        fflush(stderr);
    }
}