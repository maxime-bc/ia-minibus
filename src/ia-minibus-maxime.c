#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** ----- Strategy -----
 *
 * When a bus can be placed (enough money and not at the maximum buses number), it is placed on the station with the most travelers.
 * If all the stations are empty, the round is passed.
 *
 * The ia upgrades first buses number, then buses speed and finally travel fares.
 *
 * Three states : moving, stopped and available
 *
 * Loop through all our buses :
 *      if the bus is available, the bus dest is set to the station where most of the travelers wants to go.
 *      if their is no travelers inside the bus, the dest of the bus is set to station with most travelers.
 *      if all stations are empty, the tour is passed.
 */

/** ----- Preprocessor constants ----- **/

#define MAX_BUS_NUMBER 16
#define MAX_STATIONS_NUMBER 10
#define START_STATIONS_NUMBER 3

#define MOVING 1
#define STOPPED 2
#define AVAILABLE 3

/** ----- Structures ----- **/

typedef struct Station Station;
struct Station {
    int id;
    int x;
    int y;
    int capacity;
    int travelers_nb;
};

typedef struct Player Player;
struct Player {
    int id;
    int money;
    int SB_upgrades;
    int SP_upgrades;
    int CT_upgrades;
    int end;
};

typedef struct Traveler Traveler;
struct Traveler {
    int id;
    int ids1;
    int ids2;
    int id_bus;
    Traveler *next;
};

typedef struct Bus Bus;
struct Bus {
    int id;
    int owner_player_id;
    int x;
    int y;
    int station_id;
    int num_cars;
    int state;
};

typedef struct TravelersList TravelersList;
struct TravelersList {
    Traveler *first_traveler;
};

/** ----- Functions ----- **/

/**
 * add_traveler()
 *
 * Adds a traveler to the travelers list.
 *
 * @param travelers_list : the list of travelers where the traveler should be added.
 * @param new_traveler : the traveler to add to the travelers list.
 */

void add_traveler(TravelersList *travelers_list, Traveler *new_traveler) {
    new_traveler->next = travelers_list->first_traveler;
    travelers_list->first_traveler = new_traveler;
}

/**
 * delete_traveler_at_dest()
 *
 * Deletes a travelers who have reached his destination from the travelers list.
 *
 * @param travelers_list : the list of travelers from which the traveler should be deleted.
 * @param travelers_list_size : the size of the travelers list.
 * @param id_traveler : the id of the traveler to be deleted because he reached his destination.
 */

void delete_traveler_at_dest(TravelersList *travelers_list, int *travelers_list_size, int id_traveler) {

    Traveler *previous_traveler, *actual_traveler;
    actual_traveler = travelers_list->first_traveler;
    previous_traveler = actual_traveler;

    while (actual_traveler != NULL) {

        if (actual_traveler->id == id_traveler) {

            if (previous_traveler == actual_traveler) {

                travelers_list->first_traveler = actual_traveler->next;
                free(actual_traveler);

            } else {

                previous_traveler->next = actual_traveler->next;
                free(actual_traveler);
            }

            *travelers_list_size = *travelers_list_size - 1;
            break;
        }

        previous_traveler = actual_traveler;
        actual_traveler = actual_traveler->next;
    }
}

/**
 * update_traveler_in_bus()
 *
 * Updates the id_bus of a traveler with the id of the bus he got on.
 *
 * @param travelers_list : the list of travelers from which the traveler should be updated.
 * @param stations_array : array of stations.
 * @param id_traveler : id of the traveler for which the id_bus needs to be updated.
 * @param id_bus : id of the bus in which the traveler got on.
 */

void update_traveler_in_bus(TravelersList *travelers_list, Station stations_array[], int id_traveler, int id_bus) {

    Traveler *actual_traveler = travelers_list->first_traveler;

    while (actual_traveler != NULL) {

        if (actual_traveler->id == id_traveler) {
            actual_traveler->id_bus = id_bus;
            stations_array[actual_traveler->ids1].travelers_nb--;
        }

        actual_traveler = actual_traveler->next;
    }
}

/**
 * get_most_crowded_station()
 *
 * Search the most crowded station.
 *
 * @param stations_array : array of stations.
 * @param stations_num : number of stations in the stations_array.
 * @return id of the most crowded station.
 */

int get_most_crowded_station(Station stations_array[], int stations_num) {

    int max_travelers_nb = 0, index = -1;

    for (int i = 0; i < stations_num; i++) {

        if (stations_array[i].travelers_nb > max_travelers_nb) {
            max_travelers_nb = stations_array[i].travelers_nb;
            index = i;
        }
    }
    return index;
}

/**
 * is_bus_moving()
 *
 * Permits to know if a given bus is in movement or stopped at a station.
 *
 * @param bus : bus for which we want to know if it is moving.
 * @param stations_array : array of stations.
 * @param stations_num : number of stations in the stations_array.
 * @return Returns 1 if the bus is moving, 0 if the bus is stopped at a station
 * and -1 if the bus destination does not exist in the given stations array.
 *
 */

int is_bus_moving(Bus bus, Station stations_array[], int stations_num){

    for (int i = 0; i < stations_num; i++){

        if(bus.station_id == stations_array[i].id){
            // If the coordinates of the bus matches those of a station, it means that the bus is stopped.
            return !(bus.x == stations_array[i].x && bus.y == stations_array[i].y);
        }
    }
    return -1;
}

/**
 * int_compare()
 *
 * Calculates the difference between two integers.
 * This function is used by the function qsort called in get_the_most_popular_station().
 *
 * @param a : int a to compare with b.
 * @param b : in b to compare with a.
 * @return the difference between a and b.
 */
int int_compare(const void *a, const void *b) {

    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia - *ib;
}

/**
 * get_the_most_popular_station()
 *
 * Returns the id of the destination station for which most travelers in the bus wants to go.
 *
 * @param travelers_list : the list of travelers, used to get all the
 * @param size
 * @param bus_id
 * @return
 */

int get_the_most_popular_station(TravelersList *travelers_list, const int size, int bus_id) {

    int count, max_val = 0, index = -1, station_id_array[size], freq[size];
    Traveler *actual_traveler = travelers_list->first_traveler;
    int counter = 0;

    while (actual_traveler != NULL) {

        if (actual_traveler->id_bus == bus_id) {
            station_id_array[counter] = actual_traveler->ids2;
            freq[counter] = -1;
            counter++;
        }

        actual_traveler = actual_traveler->next;
    }

    qsort(station_id_array, counter, sizeof(int), int_compare);

    for (int i = 0; i < counter; i++) {

        count = 1;
        for (int j = i + 1; j < counter; j++) {
            // If duplicate element is found
            if (station_id_array[i] == station_id_array[j]) {
                count++;
                freq[j] = 0; // Make sure not to count frequency of same element again
            }
        }
        // If frequency of current element is not counted
        if (freq[i] != 0) {
            freq[i] = count;
        }
    }

    for (int i = 0; i < counter; ++i) {
        if (freq[i] > max_val) {
            max_val = freq[i];
            index = i;
        }
    }
    if (index == -1) {
        return index;
    } else {
        return station_id_array[index];
    }
}

/**
 * check_malloc()
 *
 * Check if memory allocation with malloc has worked.
 *
 * @param pointer : the pointer, NULL if the memory allocation failed or
 * the address of the first byte of the allocated memory area.
 */

void check_malloc(void *pointer) {
    if (pointer == NULL) {
        fprintf(stderr, "Cannot allocate initial memory for data\n");
        exit(EXIT_FAILURE);
    }
}

/** ----- Main ----- **/

int main(void) {

    // Initialised variables.
    int players_num = 0, travelers_list_size = 0, stations_num = START_STATIONS_NUMBER, my_player_id = 0, round = 0, my_buses_num = 0;

    // Variables initialised each round with game data using scanf.
    int buses_num, new_station, new_travelers_num, travelers_in_buses_num, travelers_at_dest_num, id_traveler, id_bus;

    Bus buses_array[MAX_BUS_NUMBER];
    Station stations_array[MAX_STATIONS_NUMBER];
    TravelersList *travelers_list = malloc(sizeof(*travelers_list));
    check_malloc(travelers_list);

    // Initialise the travelers list.
    travelers_list->first_traveler = NULL;

    // Initialise the state of all the buses.
    for (int i = 0; i < MAX_BUS_NUMBER; i++) {
        buses_array[i].state = AVAILABLE;
    }

    // Get the number of players and our player id.
    scanf("%d", &players_num);
    scanf("%d", &my_player_id);

    // Players array is created after knowing the number of players.
    Player players_array[players_num];

    for (int i = 0; i < stations_num; i++) {
        scanf("%d%d%d%d",
              &stations_array[i].id,
              &stations_array[i].x,
              &stations_array[i].y,
              &stations_array[i].capacity);
        stations_array[i].travelers_nb = 0;
    }

    while (1) {

        for (int i = 0; i < players_num; i++) {

            scanf("%d%d%d%d%d%d",
                  &players_array[i].id,
                  &players_array[i].money,
                  &players_array[i].SB_upgrades,
                  &players_array[i].SP_upgrades,
                  &players_array[i].CT_upgrades,
                  &players_array[i].end);
        }

        scanf("%d", &new_station);

        if (new_station) {

            scanf("%d%d%d%d",
                  &stations_array[stations_num].id,
                  &stations_array[stations_num].x,
                  &stations_array[stations_num].y,
                  &stations_array[stations_num].capacity);
            stations_array[stations_num].travelers_nb = 0;

            stations_num++;
        }

        // my_buses_num is calculated at each round, so it is initialised to 0.
        my_buses_num = 0;

        scanf("%d", &buses_num);

        for (int i = 0; i < buses_num; i++) {

            scanf("%d%d%d%d%d%d",
                  &buses_array[i].id,
                  &buses_array[i].owner_player_id,
                  &buses_array[i].x,
                  &buses_array[i].y,
                  &buses_array[i].station_id,
                  &buses_array[i].num_cars);

            if (buses_array[i].owner_player_id == my_player_id) {
                my_buses_num++;
            }
        }

        scanf("%d%d%d", &new_travelers_num, &travelers_in_buses_num, &travelers_at_dest_num);

        if (new_travelers_num > 0) {

            for (int i = 0; i < new_travelers_num; i++) {

                Traveler *new_traveler = malloc(sizeof(*new_traveler));
                check_malloc(new_traveler);

                scanf("%d%d%d", &new_traveler->id, &new_traveler->ids1, &new_traveler->ids2);
                new_traveler->id_bus = -1;

                stations_array[new_traveler->ids1].travelers_nb++; //Update station1 travelers number
                add_traveler(travelers_list, new_traveler);
                travelers_list_size++;
            }
        }

        if (travelers_in_buses_num > 0) {

            for (int i = 0; i < travelers_in_buses_num; i++) {
                scanf("%d%d", &id_traveler, &id_bus);
                update_traveler_in_bus(travelers_list, stations_array, id_traveler, id_bus);
            }
        }

        if (travelers_at_dest_num > 0) {

            for (int i = 0; i < travelers_at_dest_num; i++) {
                scanf("%d", &id_traveler);
                delete_traveler_at_dest(travelers_list, &travelers_list_size, id_traveler);
            }
        }

        /** IA starts here **/

        // Checking for upgrades
        if (my_buses_num < 4 && players_array[my_player_id].money >= 100) {

            int most_crowded_station_for_spawn = get_most_crowded_station(stations_array, stations_num);

            if (most_crowded_station_for_spawn != -1) {
                printf("BUS %d; ", most_crowded_station_for_spawn);
            }

        } else if (my_buses_num == 4 && players_array[my_player_id].SP_upgrades < 2 &&
                   players_array[my_player_id].money >= 200) {
            printf("UPDATESP; ");

        } else if (my_buses_num == 4 && players_array[my_player_id].SP_upgrades == 2 &&
                   players_array[my_player_id].CT_upgrades < 5 && players_array[my_player_id].money >= 100) {
            printf("UPDATECT; ");
        }

        //Looping through my buses
        for (int i = 0; i < buses_num; i++) {

            if (buses_array[i].owner_player_id == my_player_id) {

                if (buses_array[i].state == MOVING &&
                    is_bus_moving(buses_array[i], stations_array, stations_num) == 0) {
                    buses_array[i].state = STOPPED;

                } else if (buses_array[i].state == AVAILABLE) {

                    int most_popular_station = get_the_most_popular_station(travelers_list, travelers_list_size,
                                                                            buses_array[i].id);
                    if (most_popular_station != -1) {
                        printf("DESTINATION %d %d; ", buses_array[i].id, most_popular_station);

                    } else {
                        int most_crowded_station = get_most_crowded_station(stations_array, stations_num);
                        if (most_crowded_station != -1) {
                            printf("DESTINATION %d %d; ", buses_array[i].id, most_crowded_station);
                        }
                    }
                    buses_array[i].state = MOVING;

                } else if (buses_array[i].state != MOVING && buses_array[i].state != AVAILABLE) {
                    buses_array[i].state = AVAILABLE;
                }
            }
        }

        printf("PASS\n");

        /** IA ends here **/

        round++;

        fflush(stdout);
        fflush(stderr);
    }
}