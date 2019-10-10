#include <stdio.h>
#include <stdlib.h>

typedef struct Station Station;
struct Station
{
    int id;
    int x;
    int y;
    int capacity;
};

void populate_station(int id, int x, int y, int capacity, Station *station){

    station->id = id;
    station->x = x;
    station->y = y;
    station->capacity = capacity;

}

void print_station(Station station){

    fprintf(stderr, "%d", station.id);
    fprintf(stderr, "%d", station.x);
    fprintf(stderr, "%d", station.y);
    fprintf(stderr, "%d\n", station.capacity);

}

int main(void){

    // 3 stations are created at the start of the game
    int NB_STATIONS = 3;
    Station *all_stations = malloc(NB_STATIONS *sizeof(Station));

    int arrayIndex = 0;

    // get number of players and our player number
    int NJ = 0;
    int J = 0;

    scanf("%d", &NJ);
    scanf("%d", &J);
    fprintf(stderr, "%d", NJ);
    fprintf(stderr, "%d\n", J);

    // Populate stations informations and print them

    for(int i = 0; i < NB_STATIONS; i++){

        int id = 0, x = 0, y = 0, capacity = 0;

        scanf("%d", &id);
        scanf("%d", &x);
        scanf("%d", &y);
        scanf("%d", &capacity);

        populate_station(id, x, y, capacity, all_stations);
        print_station(*all_stations);

    }

    printf("PASS\n");
    
    while(1){

    }
}

