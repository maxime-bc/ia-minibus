#include <stdio.h>
#include <stdlib.h>

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
    fprintf(stderr, "ID_BUS=%d", traveler.id_bus);
    fprintf(stderr, "AT_DEST=%d\n", traveler.at_dest);

}

void print_all_travelers(Traveler* travelers_array, int travelers_num){

    fprintf(stderr, "ALL TRAVELERS : \n");
    for(int i = 0; i < travelers_num; i++){
        print_traveler(travelers_array[i]);
    }
}

int increase_travelers_array(Traveler **travelers_array, int current_size, int numNewElems){
    
    const int totalSize = current_size + numNewElems;
    Traveler *temp = realloc(*travelers_array, (totalSize * sizeof(Traveler)));

    if (temp == NULL){
        printf("Cannot allocate more memory.\n");
        return 0;
    }else{
        *travelers_array = temp;
    }

    return totalSize;
}

int main(void){

    // 3 stations are created at the start of the game
    int stations_number = 3;
    int MAX_STATIONS = 10;
    int NB_PLAYERS = 0;
    int ID_PLAYER = 0;
    int ROUND = 0;

    // get number of players and our player number
    scanf("%d", &NB_PLAYERS);
    scanf("%d", &ID_PLAYER);
    fprintf(stderr, "NBP=%d, ", NB_PLAYERS);
    fprintf(stderr, "IDP=%d\n", ID_PLAYER);

    Station all_stations[MAX_STATIONS];
    Player all_players[NB_PLAYERS];

    int travelers_num = 0;
    Traveler* travelers_array = malloc(0 * sizeof(Traveler *));
    
    if(travelers_array == NULL){
        printf("Cannot allocate initial memory for data\n");
        exit(1);
    }

    // Populate stations informations and print them

    for(int i = 0; i < stations_number; i++){

        int id = 0, x = 0, y = 0, capacity = 0;

        scanf("%d", &id);
        scanf("%d", &x);
        scanf("%d", &y);
        scanf("%d", &capacity);

        populate_station(id, x, y, capacity, &all_stations[i]);
        print_station(all_stations[i]);

    }

    while(1){

        fprintf(stderr, "ROUND %d\n", ROUND);

        for(int i = 0; i < NB_PLAYERS; i++){

            int id = 0, money = 0, SB_upgrades = 0, SP_upgrades = 0, CT_upgrades = 0, end = 0;

            scanf("%d", &id);
            scanf("%d", &money);
            scanf("%d", &SB_upgrades);
            scanf("%d", &SP_upgrades);
            scanf("%d", &CT_upgrades);
            scanf("%d", &end);

            populate_player(id, money, SB_upgrades, SP_upgrades, CT_upgrades, end, &all_players[i]);
            print_player(all_players[i]);

        }

        int is_new_station = 0;
        scanf("%d", &is_new_station);
        fprintf(stderr, "NEWST=%d\n", is_new_station);

        if(is_new_station){

            //Each 25 rounds, a new station is added

            int id = 0, x = 0, y = 0, capacity = 0;

            scanf("%d", &id);
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &capacity);

            populate_station(id, x, y, capacity, &all_stations[stations_number]);
            stations_number++;
            print_all_stations(all_stations, stations_number);
            
        }

        int NB_BUS = 0;
        scanf("%d", &NB_BUS);
        fprintf(stderr, "NBBUS=%d\n", NB_BUS);

        Bus all_bus[NB_BUS];

        for(int i = 0; i < NB_BUS; i++){

            int id = 0, owner_player_id = 0, x = 0, y = 0, station_id = 0, num_cars = 0;

            scanf("%d", &id);
            scanf("%d", &owner_player_id);
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &station_id);
            scanf("%d", &num_cars);

            populate_bus(id, owner_player_id, x, y, station_id, num_cars, &all_bus[i]);
            print_bus(all_bus[i]);

        }

        int new_travelers = 0, TRAVELERS_IN_BUS = 0, TRAVELERS_REACHING_DEST = 0;
        scanf("%d", &new_travelers);
        scanf("%d", &TRAVELERS_IN_BUS);
        scanf("%d", &TRAVELERS_REACHING_DEST);
        fprintf(stderr, "NT=%d, ", new_travelers);
        fprintf(stderr, "TB=%d, ", TRAVELERS_IN_BUS);
        fprintf(stderr, "TD=%d\n", TRAVELERS_REACHING_DEST);

        if(new_travelers != 0){

            int actual_travelers_num = travelers_num;
            travelers_num = increase_travelers_array(&travelers_array, travelers_num, new_travelers);

            fprintf(stderr, "NEW TRAVELERS :  \n");

            for(int i = actual_travelers_num; i < travelers_num; i ++){

                int IDT = 0, IDS1 = 0, IDS2 = 0;

                scanf("%d", &IDT);
                scanf("%d", &IDS1);
                scanf("%d", &IDS2);
                
                populate_traveler(&travelers_array[i], IDT, IDS1, IDS2, -1, 0);
                print_traveler(travelers_array[i]);

            }

            //print_all_travelers(travelers_array, travelers_num);
        }

        int all_travelers_in_bus[TRAVELERS_IN_BUS][2];

        for (int i = 0; i < TRAVELERS_IN_BUS; i++){

            int IDT = 0, IDB = 0;

            scanf("%d", &IDT);
            scanf("%d", &IDB);
            fprintf(stderr, "IDT=%d, ", IDT);
            fprintf(stderr, "IDB=%d\n", IDB);

            all_travelers_in_bus[i][1] = IDB;
            all_travelers_in_bus[i][2] = IDB;
        }

        int all_travelers_reaching_dest[TRAVELERS_REACHING_DEST];

        for(int i = 0; i < TRAVELERS_REACHING_DEST; i++){

            int IDT = 0;

            scanf("%d", &IDT);
            fprintf(stderr, "IDT=%d\n", IDT);

            all_travelers_reaching_dest[i] = IDT;

        }

        // //IA//

        // fprintf(stderr, "PLAYER ID=%d\n", ID_PLAYER);
        // fprintf(stderr, "PLAYER MONEY=%d\n", all_players[ID_PLAYER].money);

        // if(NB_BUS == 0 && all_players[ID_PLAYER].money >= 100){
        //     printf("BUS 2\n");
        // }else{

        //     if(new_travelers != 0){

        //         if(NB_BUS == 1){

        //             int distance = 0;
        //             int bus_dest_station = 0;

        //             for(int i = 0; i < new_travelers; i++){

        //                 int start_station_id = all_new_travellers[i][1];
        //                 int start_st_x = 0, start_st_y = 0;

        //                 //Get the departure station
        //                 for(int i = 0; i < stations_number; i++){
        //                     if(all_stations[i].id == start_station_id){
        //                         start_st_x = all_stations[i].x;
        //                         start_st_y = all_stations[i].y;
        //                     }
        //                 }
        //                 fprintf(stderr, "TRAVELER ST DP=%d\n", start_station_id);

        //                 //Determine which one is the closest to the actual bus
        //                 int new_distance = (start_st_x + all_bus[1].x)*(start_st_x + all_bus[1].x) + (start_st_y + all_bus[1].y)*(start_st_y + all_bus[1].y);

        //                 if(new_distance < distance){
        //                     distance = new_distance;
        //                     bus_dest_station = start_station_id;
        //                     fprintf(stderr, "CLOSEST T=%d\n", all_new_travellers[i][1]);
        //                 }
                    
        //             }

        //             printf("DESTINATION 0 %d\n", bus_dest_station);

        //         }
        //     }else{
        //         printf("PASS\n");
        //     }
        // }

        // //IA//

        printf("PASS\n");

        ROUND++;

        fflush(stdout);

    }
}

