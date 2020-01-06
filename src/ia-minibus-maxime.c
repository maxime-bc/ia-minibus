// Maxime BLANCHON
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** ----- Strategy -----
 *
 * All travelers are stored in a list.
 * When a traveler reach his destination, he is deleted from the list.
 *
 * First, the program upgrades buses number, then buses speed (SP) and finally travel fares (CT).
 *
 * If a new bus can be placed (enough money and the maximum number of buses is not reached),
 * it is placed on the station with the most travelers.
 * If all the stations are empty, the round is passed and the bus is not placed.
 *
 * A bus can have 3 states :
 *                  - moving (going from a station A to a station B),
 *                  - stopped (the bus is at a station, waiting for travelers who arrived at
 *                            their destination to got off and for new travelers to got on),
 *                  - available (bus is ready to go to a new destination station).
 *
 * For each bus I own :
 *      - if the bus is available with travelers on board, its destination is set to the station where
 *      most of the travelers wants to go,
 *      - if the bus is available with no travelers on board, its destination is set to
 *      the station with most travelers waiting for a bus. If all the stations are empty, the round is passed.
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
    int travelers_number;
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
    int bus_id;
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
 * @param travelers_list : list of travelers where the traveler should be added.
 * @param new_traveler : traveler to add to @param travelers_list.
 */

void add_traveler(TravelersList *travelers_list, Traveler *new_traveler) {
    new_traveler->next = travelers_list->first_traveler;
    travelers_list->first_traveler = new_traveler;
}

/**
 * delete_traveler_at_dest()
 *
 * Deletes a traveler who have reached his destination from the travelers list.
 *
 * @param travelers_list : list of travelers from which the traveler should be deleted.
 * @param travelers_list_size : size of @param travelers_list.
 * @param id_traveler : id of the traveler to delete because he reached his destination.
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
 * Updates the bus_id of a traveler with the id of the bus he got on.
 *
 * @param travelers_list : list of travelers containing the traveler to update.
 * @param stations_array : array of stations.
 * @param traveler_id : id of the traveler for which the bus_id needs to be updated.
 * @param bus_id : id of the bus in which the traveler got on.
 */

void update_traveler_in_bus(TravelersList *travelers_list, Station stations_array[], int traveler_id, int bus_id) {

    Traveler *actual_traveler = travelers_list->first_traveler;

    while (actual_traveler != NULL) {

        if (actual_traveler->id == traveler_id) {
            actual_traveler->bus_id = bus_id;
            stations_array[actual_traveler->ids1].travelers_number--;
        }

        actual_traveler = actual_traveler->next;
    }
}

/**
 * is_bus_moving()
 *
 * Permits to know if a given bus is in movement or stopped at a station.
 *
 * @param bus : bus for which we want to know if it is moving.
 * @param stations_array : array of stations.
 * @param stations_number : number of stations in @param stations_array.
 * @return Returns 1 if the bus is moving, 0 if the bus is stopped at a station
 * and -1 if the bus destination does not exist in the given stations array.
 *
 */

int is_bus_moving(Bus bus, Station stations_array[], int stations_number) {

    for (int i = 0; i < stations_number; i++) {

        if (bus.station_id == stations_array[i].id) {
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
 * This function is used by the function qsort called in @see get_the_most_popular_station().
 *
 * @param a : int a to compare with b.
 * @param b : int b to compare with a.
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
 * @param travelers_list : list of travelers used to get all the travelers in the bus corresponding to @param bus_id.
 * @param travelers_list_size : size of @param travelers_list.
 * @param bus_id : id of the bus for which we want to know travelers most popular destination.
 * @return -1 if the bus is empty or the id of the most popular station if the bus isn't empty.
 */

int get_the_most_popular_station(TravelersList *travelers_list, int travelers_list_size, int bus_id) {

    int counter, max_freq = 0, index = -1, travelers_in_bus_dest_array[travelers_list_size], frequency[travelers_list_size];
    Traveler *actual_traveler = travelers_list->first_traveler;

    int travelers_in_bus_number = 0;

    while (actual_traveler != NULL) {

        if (actual_traveler->bus_id == bus_id) {
            travelers_in_bus_dest_array[travelers_in_bus_number] = actual_traveler->ids2;
            frequency[travelers_in_bus_number] = -1;
            travelers_in_bus_number++;
        }
        actual_traveler = actual_traveler->next;
    }

    // Return -1 if the bus is empty.
    if (travelers_in_bus_number == 0) {
        return -1;
    }

    qsort(travelers_in_bus_dest_array, (size_t) travelers_in_bus_number, sizeof(int), int_compare);

    for (int i = 0; i < travelers_in_bus_number; i++) {

        counter = 1;
        for (int j = i + 1; j < travelers_in_bus_number; j++) {
            // If a duplicate element is found
            if (travelers_in_bus_dest_array[i] == travelers_in_bus_dest_array[j]) {
                counter++;
                frequency[j] = 0; // Make sure to not count frequency of the same element again
            }
        }
        // If frequency of the current element is not counted
        if (frequency[i] != 0) {
            frequency[i] = counter;
        }
    }

    for (int i = 0; i < travelers_in_bus_number; ++i) {
        if (frequency[i] > max_freq) {
            max_freq = frequency[i];
            index = i;
        }
    }

    return travelers_in_bus_dest_array[index];
}

/**
 * get_most_crowded_station()
 *
 * Search the most crowded station.
 *
 * @param stations_array : array of stations.
 * @param stations_number : number of stations in @param stations_array.
 * @return -1 if all stations are empty or the id of the most crowded station if there is travelers.
 */

int get_most_crowded_station(Station stations_array[], int stations_number) {

    int max_travelers_nb = 0, index = -1;

    for (int i = 0; i < stations_number; i++) {

        if (stations_array[i].travelers_number > max_travelers_nb) {
            max_travelers_nb = stations_array[i].travelers_number;
            index = i;
        }
    }
    return index;
}

/**
 * check_malloc()
 *
 * Check if memory allocation with malloc worked.
 *
 * @param pointer : pointer to check, returned by malloc.
 */

void check_malloc(void *pointer) {
    // Pointer is NULL if the memory allocation with malloc failed.
    if (pointer == NULL) {
        fprintf(stderr, "Cannot allocate initial memory for data.\n");
        exit(EXIT_FAILURE);
    }
}

/** ----- Main ----- **/

int main(void) {

    // Initialised variables.
    int players_number = 0, travelers_list_size = 0, stations_number = START_STATIONS_NUMBER, my_player_id = 0, round = 0, my_buses_number = 0;

    // Variables initialised each round with game data using scanf.
    int buses_number, new_station, new_travelers_number, travelers_in_buses_number, travelers_at_dest_number, id_traveler, id_bus;

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
    scanf("%d", &players_number);
    scanf("%d", &my_player_id);

    // Players array is created after knowing the number of players.
    Player players_array[players_number];

    for (int i = 0; i < stations_number; i++) {
        scanf("%d%d%d%d",
              &stations_array[i].id,
              &stations_array[i].x,
              &stations_array[i].y,
              &stations_array[i].capacity);
        stations_array[i].travelers_number = 0;
    }

    while (1) {

        for (int i = 0; i < players_number; i++) {

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
                  &stations_array[stations_number].id,
                  &stations_array[stations_number].x,
                  &stations_array[stations_number].y,
                  &stations_array[stations_number].capacity);
            stations_array[stations_number].travelers_number = 0;

            stations_number++;
        }

        // my_buses_number is calculated at each round, so it is initialised to 0.
        my_buses_number = 0;

        scanf("%d", &buses_number);

        for (int i = 0; i < buses_number; i++) {

            scanf("%d%d%d%d%d%d",
                  &buses_array[i].id,
                  &buses_array[i].owner_player_id,
                  &buses_array[i].x,
                  &buses_array[i].y,
                  &buses_array[i].station_id,
                  &buses_array[i].num_cars);

            if (buses_array[i].owner_player_id == my_player_id) {
                my_buses_number++;
            }
        }

        scanf("%d%d%d", &new_travelers_number, &travelers_in_buses_number, &travelers_at_dest_number);

        if (new_travelers_number > 0) {

            for (int i = 0; i < new_travelers_number; i++) {

                Traveler *new_traveler = malloc(sizeof(*new_traveler));
                check_malloc(new_traveler);

                scanf("%d%d%d", &new_traveler->id, &new_traveler->ids1, &new_traveler->ids2);
                new_traveler->bus_id = -1;

                stations_array[new_traveler->ids1].travelers_number++; //Update station1 travelers number
                add_traveler(travelers_list, new_traveler);
                travelers_list_size++;
            }
        }

        if (travelers_in_buses_number > 0) {

            for (int i = 0; i < travelers_in_buses_number; i++) {
                scanf("%d%d", &id_traveler, &id_bus);
                update_traveler_in_bus(travelers_list, stations_array, id_traveler, id_bus);
            }
        }

        if (travelers_at_dest_number > 0) {

            for (int i = 0; i < travelers_at_dest_number; i++) {
                scanf("%d", &id_traveler);
                delete_traveler_at_dest(travelers_list, &travelers_list_size, id_traveler);
            }
        }

        /** IA starts here **/

        // Checking for upgrades
        if (my_buses_number < 4 && players_array[my_player_id].money >= 100) {

            int most_crowded_station_for_spawn = get_most_crowded_station(stations_array, stations_number);

            if (most_crowded_station_for_spawn != -1) {
                printf("BUS %d; ", most_crowded_station_for_spawn);
            }
            // If all stations are empty, a round is passed.

        } else if (my_buses_number == 4 && players_array[my_player_id].SP_upgrades < 2 &&
                   players_array[my_player_id].money >= 200) {
            printf("UPDATESP; ");

        } else if (my_buses_number == 4 && players_array[my_player_id].SP_upgrades == 2 &&
                   players_array[my_player_id].CT_upgrades < 5 && players_array[my_player_id].money >= 100) {
            printf("UPDATECT; ");
        }

        // Looping through all buses
        for (int i = 0; i < buses_number; i++) {

            // Filter buses by owner player id to give commands only to my own buses.
            if (buses_array[i].owner_player_id == my_player_id) {

                if (buses_array[i].state == MOVING &&
                    is_bus_moving(buses_array[i], stations_array, stations_number) == 0) {
                    buses_array[i].state = STOPPED;

                } else if (buses_array[i].state == AVAILABLE) {

                    int most_popular_station = get_the_most_popular_station(travelers_list, travelers_list_size,
                                                                            buses_array[i].id);
                    if (most_popular_station != -1) {
                        printf("DESTINATION %d %d; ", buses_array[i].id, most_popular_station);

                    } else {
                        int most_crowded_station = get_most_crowded_station(stations_array, stations_number);
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
