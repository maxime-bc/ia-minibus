#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_STATIONS_NUMBER 10
#define START_STATIONS_NUMBER 3

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
};

typedef struct TravelersList TravelersList;
struct TravelersList{
    Traveler *first_traveler;
};

void populate_station(int id, int x, int y, int capacity, int travelers_nb, Station *station){

    station->id = id;
    station->x = x;
    station->y = y;
    station->capacity = capacity;
    station->travelers_nb = travelers_nb;

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

void populate_traveler(Traveler *traveler, int id, int ids1, int ids2, int id_bus){

    traveler->id = id;
    traveler->ids1 = ids1;
    traveler->ids2 = ids2;
    traveler->id_bus = id_bus;
}

void print_station(Station station){

    fprintf(stderr, "IDST=%d, ", station.id);
    fprintf(stderr, "X=%d, ", station.x);
    fprintf(stderr, "Y=%d, ", station.y);
    fprintf(stderr, "K=%d\n", station.capacity);
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
    fprintf(stderr, "NC=%d\n", bus.num_cars);

}

void print_all_bus(Bus bus[], int bus_num){

    fprintf(stderr, "ALL BUS : \n");
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

Traveler *sort_travelers_by_station(Traveler *travelers_array, int travelers_num, int station_id, int *travelers_in_station) {

    Traveler* station_travelers_array = malloc(0 * sizeof(Traveler *));
    check_malloc(station_travelers_array);

    for (int i = 0; i < travelers_num; i++) {

        if (station_id == travelers_array[i].ids1 && travelers_array[i].id_bus == -1) {
            //increase_travelers_array(&station_travelers_array, *travelers_in_station, 1);
            populate_traveler(&station_travelers_array[*travelers_in_station],
                    travelers_array[i].id,
                    travelers_array[i].ids1,
                    travelers_array[i].ids2,
                    travelers_array[i].id_bus);
            *travelers_in_station = *travelers_in_station+1;
        }
    }

    return station_travelers_array;
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

/*
void get_most_crowded_stations(Station stations_array[], int stations_num){

    int max_val = -1;
    int res[stations_num][2];
    int index = 1;

    for(int i = 0; i < stations_num; i++){

        if (stations_array[i].travelers_nb > max_val) {
            max_val = stations_array[i].travelers_nb;
            res[0][0] = i;
            res[0][1] = stations_array[i].travelers_nb;
        }else if(stations_array[i].travelers_nb == max_val){
            res[index][0] = i;
            res[index][1] = stations_array[i].travelers_nb;
            index++;
        }
    }

    for(int i = 0; i < index; i++){
        fprintf(stderr, "STATION %d, TRAVELERS %d\n", res[i][0], res[i][1]);
    }
}*/

void get_the_most_popular_station(TravelersList *travelers_list, const int size, int bus_id){

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
        fprintf(stderr, "MOST TRAVELRS WANTS TO GO TO ST %d\n", station_id_array[index]);
    }
}

int main(void){

    int player_num, travelers_list_size = 0, stations_num = START_STATIONS_NUMBER, player_id, round = 0;

    srand(time(NULL));

    scanf("%d", &player_num);
    scanf("%d", &player_id);

    Player players_array[player_num];
    Station stations_array[MAX_STATIONS_NUMBER];

    TravelersList* travelers_list = malloc(sizeof(*travelers_list));
    check_malloc(travelers_list);

    travelers_list->first_traveler = NULL;

    for(int i = 0; i < stations_num; i++){

        int id, x, y, capacity;

        scanf("%d", &id);
        scanf("%d", &x);
        scanf("%d", &y);
        scanf("%d", &capacity);

        populate_station(id, x, y, capacity, 0, &stations_array[i]);

    }

    while(1){

        for(int i = 0; i < player_num; i++){

            int id, money, SB_upgrades, SP_upgrades, CT_upgrades, end;

            scanf("%d", &id);
            scanf("%d", &money);
            scanf("%d", &SB_upgrades);
            scanf("%d", &SP_upgrades);
            scanf("%d", &CT_upgrades);
            scanf("%d", &end);

            populate_player(id, money, SB_upgrades, SP_upgrades, CT_upgrades, end, &players_array[i]);
        }

        int new_station;
        scanf("%d", &new_station);

        if(new_station){

            int id, x, y, capacity;

            scanf("%d", &id);
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &capacity);

            populate_station(id, x, y, capacity, 0, &stations_array[stations_num]);
            stations_num++;

        }

        int bus_num;
        scanf("%d", &bus_num);

        Bus bus_array[bus_num];

        for(int i = 0; i < bus_num; i++){

            int id, owner_player_id, x, y, station_id, num_cars;

            scanf("%d", &id);
            scanf("%d", &owner_player_id);
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &station_id);
            scanf("%d", &num_cars);

            populate_bus(id, owner_player_id, x, y, station_id, num_cars, &bus_array[i]);

        }

        int new_travelers_num, travelers_in_bus_num, travelers_at_dest_num;
        scanf("%d", &new_travelers_num);
        scanf("%d", &travelers_in_bus_num);
        scanf("%d", &travelers_at_dest_num);

        if(new_travelers_num > 0){

            for(int i = 0; i < new_travelers_num; i ++){

                Traveler *new_traveler = malloc(sizeof(*new_traveler));
                check_malloc(new_traveler);

                int IDT, IDS1, IDS2;

                scanf("%d", &IDT);
                scanf("%d", &IDS1);
                scanf("%d", &IDS2);
                
                populate_traveler(new_traveler, IDT, IDS1, IDS2, -1);
                stations_array[IDS1].travelers_nb++; //Update station1 travelers number
                add_traveler(travelers_list, new_traveler);
                travelers_list_size++;
            }
        }

        if(travelers_in_bus_num > 0){

            for (int i = 0; i < travelers_in_bus_num; i++){

                int IDT, IDB;
                scanf("%d", &IDT);
                scanf("%d", &IDB);

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

        print_all_bus(bus_array, bus_num);
        print_all_stations(stations_array, stations_num);
        print_travelers_list(travelers_list);
        fprintf(stderr, "LIST SIZE : %d\n", travelers_list_size);

        get_the_most_popular_station(travelers_list, travelers_list_size, 0);

        printf("PASS\n");

        round++;

        fflush(stdout);
    }
}

