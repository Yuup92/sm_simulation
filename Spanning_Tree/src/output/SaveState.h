#ifndef SAVESTATE_H_
#define SAVESTATE_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include <chrono>
#include <ctime>

#include "src/connection/Neighbours.h"
#include "src/connection/LinkedNode.h"
#include "src/connection/capacity/LinkCapacity.h"

class SaveState {

    public:
        static constexpr char DIRNAME[] = "savestate";
        static constexpr char NUMNODES[] = "NUMBERNODES:";
        static constexpr char NUMNEIG[] = "NUMBERNEIGHBOURHOODS:";

        static constexpr char NEIGHNUM[] = "NEIGHBOURHOODNUM:";
        static constexpr char LINKEDN[] = "LINKEDNODES:";
        static constexpr char OUTGOINGE[] = "OUTGOINGEDGE:";
        static constexpr char CAPACITY[] = "CAPACITY:";
        static constexpr char NUMOFCHILD[] = "NUMBEROFCHILDREN:";
        static constexpr char NOCHILD[] = "NOCHILDREN";
        static constexpr char EDGEROOT[] = "EDGEROOT";
        static constexpr char NOTROOT[] = "EDGENOTROOT";
        static constexpr char SPACER[] = "####";


        SaveState(void);

        void set_node_id_and_amount_of_nodes(int,int);
        void set_neighbours(Neighbours *, int);
        void save(void);

        std::string loadstate(LinkCapacity *);

    private:
        int nodeId;
        int nodesInSystem;
        std::string fileName;
        bool updatedAmountNeighbourhoods;
        int amountOfNeighbourhoods;

        int linkCapacityIndex;
        LinkCapacity linkCapacity[30];

        std::ofstream saveFile;
        std::ifstream loadFile;

        Neighbours *connectedNeighbours;

        std::string get_time(void);
        std::string get_neighbourhood_string(void);

        int get_line_number(std::string);

        int update_line_pos(int, int);
        void update_link_capacity(LinkCapacity *, int, int);
};

#endif
