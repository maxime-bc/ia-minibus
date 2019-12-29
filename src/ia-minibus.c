#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_STATIONS_NUMBER 10
#define START_STATIONS_NUMBER 3

const int IN_BUS_ARRAY_SIZE = 2;

typedef struct Station Station;
struct Station{
    int id;
    int x;
    int y;
    int capacity;
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
    int at_dest;
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
};

typedef struct TravelersList TravelersList;
struct TravelersList{
    Traveler *first_traveler;
};

void populate_station(int id, int x, int y, int capacity, Station *station){

    station->id = id;
    station->x = x;
    station->y = y;
    station->capacity = capacity;

}

void populate_player(int id, int money, int SB_upgrades, int SP_upgrades, int CT_upgrades, int end, Player *player){

    player->id = id;
    player->money = money;
    player->SB_upgrades = SB_upgrades;
    player->SP_upgrades = SP_upgrades;
    player->CT_upgrades = CT_upgrades;
    player->end = end;

}

void populate_bus(int id, int owner_player_id, int x, int y, int station_id, int num_cars, Bus *bus){

    bus->id = id;
    bus->owner_player_id = owner_player_id;
    bus->x = x;
    bus->y = y;
    bus->station_id = station_id;
    bus->num_cars = num_cars;

}

void populate_traveler(Traveler *traveler, int id, int ids1, int ids2, int id_bus, int at_dest){

    traveler->id = id;
    traveler->ids1 = ids1;
    traveler->ids2 = ids2;
    traveler->id_bus = id_bus;
    traveler->at_dest = at_dest;

}

void print_station(Station station){

    fprintf(stderr, "IDST=%d, ", station.id);
    fprintf(stderr, "X=%d, ", station.x);
    fprintf(stderr, "Y=%d, ", station.y);
    fprintf(stderr, "K=%d\n", station.capacity);

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
    fprintf(stderr, "NC=%d\n", bus.num_cars);

}

void print_traveler(Traveler traveler){

    fprintf(stderr, "IDT=%d, ", traveler.id);
    fprintf(stderr, "IDS1=%d, ", traveler.ids1);
    fprintf(stderr, "IDS2=%d, ", traveler.ids2);
    fprintf(stderr, "ID_BUS=%d, ", traveler.id_bus);
    fprintf(stderr, "AT_DEST=%d\n", traveler.at_dest);

}


// Add a new traveler at the start of the list
void add_traveler(TravelersList *travelers_list, Traveler *new_traveler){
    new_traveler->next = travelers_list->first_traveler;
    travelers_list->first_traveler = new_traveler;
}

void print_travelers_list(TravelersList *travelers_list){

    Traveler *actual_traveler = travelers_list->first_traveler;

    while (actual_traveler != NULL){
        print_traveler(*actual_traveler);
        actual_traveler = actual_traveler->next;
    }
}


void print_all_travelers(Traveler *travelers_array, int travelers_num){

    for(int i = 0; i < travelers_num; i++){
        print_traveler(travelers_array[i]);
    }
}

int increase_travelers_array(Traveler **travelers_array, int current_size, int num_new_elems){
    
    const int total_size = current_size + num_new_elems;
    Traveler *temp = realloc(*travelers_array, (total_size * sizeof(Traveler)));

    if (temp == NULL){
        printf("Cannot allocate more memory.\n");
        return 0;
    }else{
        *travelers_array = temp;
    }

    return total_size;
}

void update_travelers_in_bus(Traveler *travelers_array, int travelers_num, int travelers_in_bus[][IN_BUS_ARRAY_SIZE], int in_bus_number){

    for(int i = 0; i < travelers_num; i++){

        for(int j = 0; j < in_bus_number; j++){

            Traveler *traveler = &travelers_array[i];

            if(!traveler->at_dest){

                if(traveler->id == travelers_in_bus[j][0]){
                    traveler->id_bus = travelers_in_bus[j][1];
                    //fprintf(stderr, "UPDATED %d IS NOW IN %d\n", traveler.id, traveler.id_bus);
                }
            }
        }
    }  
}

void update_travelers_reaching_dest(Traveler *travelers_array, int travelers_num, const int travelers_reaching_dest[], int reaching_dest){

    for(int i = 0; i < travelers_num; i++){

        for(int j = 0; j < reaching_dest; j++){

            Traveler *traveler = &travelers_array[i];

            if(!traveler->at_dest){

                if(traveler->id == travelers_reaching_dest[j]){
                    traveler->at_dest = 1;
                    traveler->id_bus = -1;
                    //fprintf(stderr, "UPDATED %d IS AT DEST : %d\n", traveler.id, traveler.at_dest);
                }
            }
        }
    }
}

Traveler *sort_travelers_by_station(Traveler *travelers_array, int travelers_num, int station_id, int *travelers_in_station) {

    Traveler* station_travelers_array = malloc(0 * sizeof(Traveler *));

    for (int i = 0; i < travelers_num; i++) {

        if (station_id == travelers_array[i].ids1 && travelers_array[i].id_bus == -1) {
            increase_travelers_array(&station_travelers_array, *travelers_in_station, 1);
            populate_traveler(&station_travelers_array[*travelers_in_station],
                    travelers_array[i].id,
                    travelers_array[i].ids1,
                    travelers_array[i].ids2,
                    travelers_array[i].id_bus,
                    travelers_array[i].at_dest);
            *travelers_in_station = *travelers_in_station+1;
        }
    }

    return station_travelers_array;
}

int get_station_with_most_travelers(Traveler *travelers_array, int travelers_num, int stations_num) {

    int max = 0;
    int station = -1;
    int travelers_num_per_station[stations_num];

    for (int i = 0; i < stations_num; i++) {

        int travelers_in_station = 0;
        Traveler *all_travelers_in_station = sort_travelers_by_station(travelers_array, travelers_num, i,
                                                                       &travelers_in_station);

        travelers_num_per_station[i] = travelers_in_station;

        if(travelers_in_station > 0) {
            fprintf(stderr, "In ST %d : \n", i);
            print_all_travelers(all_travelers_in_station, travelers_in_station);
        }

    }

    for (int i = 0; i < stations_num; i++){
        if (travelers_num_per_station[i] > max){
            max = travelers_num_per_station[i];
            station = i;
        }
    }

    fprintf(stderr, "Max : %d travelers in ST %d\n", max, station);

    return station;
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
int is_bus_full(Bus bus, Traveler* travelers, int travelers_num){

    int travelers_in_bus_number = 0;

    for(int i = 0; i < travelers_num; i++){

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

    for(int i = 0; i < travelers_num; i++){

        if(travelers[i].id_bus == bus.id){

            increase_travelers_array(&travelers_in_bus_array, *travelers_in_bus_num, 1);
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

int get_the_most_popular_station(Traveler *travelers_in_bus, const int *size){

    int count, max_val = 0, index = -1, station_id_array[*size], freq[*size];

    for(int i = 0; i < *size; i++){
        station_id_array[i] = travelers_in_bus[i].ids2;
        // Initialize frequency array
        freq[i] = -1;
    }

    qsort(station_id_array, *size, sizeof(int), int_compare);

    for(int i=0; i < *size; i++){

        count = 1;
        for(int j=i+1; j < *size; j++){
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

    for (int i = 0; i < *size; ++i) {
        if (freq[i] > max_val ) {
            max_val = freq[i];
            index = i;
        }
    }

    if(index == -1){ //Bus is empty
        return index;
    }else{
        return station_id_array[index];
    }
}


int main(void){

    int player_num = 0, travelers_num = 0, stations_num = START_STATIONS_NUMBER;
    int player_id = 0;
    int round = 0;

    srand(time(NULL));

    // get number of players and our player id
    scanf("%d", &player_num);
    scanf("%d", &player_id);

    Player players_array[player_num];
    Station stations_array[MAX_STATIONS_NUMBER];

    TravelersList* travelers_list = malloc(sizeof(*travelers_list));
    if (travelers_list == NULL){
        fprintf(stderr, "Cannot allocate initial memory for data\n");
        exit(EXIT_FAILURE);
    }

    travelers_list->first_traveler = NULL;

    for(int i = 0; i < stations_num; i++){

        int id = 0, x = 0, y = 0, capacity = 0;

        scanf("%d", &id);
        scanf("%d", &x);
        scanf("%d", &y);
        scanf("%d", &capacity);

        populate_station(id, x, y, capacity, &stations_array[i]);

    }

    while(1){

        for(int i = 0; i < player_num; i++){

            int id = 0, money = 0, SB_upgrades = 0, SP_upgrades = 0, CT_upgrades = 0, end = 0;

            scanf("%d", &id);
            scanf("%d", &money);
            scanf("%d", &SB_upgrades);
            scanf("%d", &SP_upgrades);
            scanf("%d", &CT_upgrades);
            scanf("%d", &end);

            populate_player(id, money, SB_upgrades, SP_upgrades, CT_upgrades, end, &players_array[i]);
            //print_player(players_array[i]);

        }

        int station_added = 0;
        scanf("%d", &station_added);
        fprintf(stderr, "NEWST=%d\n", station_added);

        if(station_added){

            int id = 0, x = 0, y = 0, capacity = 0;

            scanf("%d", &id);
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &capacity);

            populate_station(id, x, y, capacity, &stations_array[stations_num]);
            stations_num++;
            //print_all_stations(stations_array, stations_num);
            
        }

        int bus_nb = 0;
        scanf("%d", &bus_nb);
        fprintf(stderr, "NBBUS=%d\n", bus_nb);

        Bus bus_array[bus_nb];

        for(int i = 0; i < bus_nb; i++){

            int id = 0, owner_player_id = 0, x = 0, y = 0, station_id = 0, num_cars = 0;

            scanf("%d", &id);
            scanf("%d", &owner_player_id);
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &station_id);
            scanf("%d", &num_cars);

            populate_bus(id, owner_player_id, x, y, station_id, num_cars, &bus_array[i]);
            print_bus(bus_array[i]);

        }

        int new_travelers_num = 0, travelers_in_bus = 0, travelers_reaching_dest = 0;
        scanf("%d", &new_travelers_num);
        scanf("%d", &travelers_in_bus);
        scanf("%d", &travelers_reaching_dest);
        fprintf(stderr, "NT=%d, ", new_travelers_num);
        fprintf(stderr, "TB=%d, ", travelers_in_bus);
        fprintf(stderr, "TD=%d\n", travelers_reaching_dest);

        if(new_travelers_num > 0){

            for(int i = 0; i < new_travelers_num; i ++){

                Traveler *new_traveler = malloc(sizeof(*new_traveler));

                int IDT = 0, IDS1 = 0, IDS2 = 0;

                scanf("%d", &IDT);
                scanf("%d", &IDS1);
                scanf("%d", &IDS2);
                
                populate_traveler(new_traveler, IDT, IDS1, IDS2, -1, 0);
                add_traveler(travelers_list, new_traveler);
            }
        }

        if(travelers_in_bus > 0){

            int all_travelers_in_bus[travelers_in_bus][IN_BUS_ARRAY_SIZE];
            
            for (int i = 0; i < travelers_in_bus; i++){

                int IDT = 0, IDB = 0;
                scanf("%d", &IDT);
                scanf("%d", &IDB);
                all_travelers_in_bus[i][0] = IDT;
                all_travelers_in_bus[i][1] = IDB;

            }

            //update_travelers_in_bus(travelers_array, travelers_num, all_travelers_in_bus, travelers_in_bus);
        }

        if(travelers_reaching_dest > 0){

            int all_travelers_reaching_dest[travelers_reaching_dest];

            for(int i = 0; i < travelers_reaching_dest; i++){

                int IDT = 0;
                scanf("%d", &IDT);
                all_travelers_reaching_dest[i] = IDT;

            }

            //update_travelers_reaching_dest(travelers_array, travelers_num, all_travelers_reaching_dest, travelers_reaching_dest);
        }

        print_all_stations(stations_array, stations_num);
        print_travelers_list(travelers_list);

        printf("PASS\n");

        round++;

        fflush(stdout);

    }
}

