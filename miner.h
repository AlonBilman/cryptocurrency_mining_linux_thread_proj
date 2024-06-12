#pragma once
#include <iostream>
#include "block.h"
#include "server.h"
#include <zlib.h>
#include <ctime>
#include "global.h"

class Miner
{
protected:
    //Data members
    int id;
    int difficulty_target=0;
    unsigned int last_hash=0;
    int height_target=1;
    int nonce=0;
    unsigned int mask=1;
    time_t timestamp;
    Server *my_server;

    //pthread objects
    pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

public:
    static void* miner_thread_start(void* arg);
    //constructor
    Miner(int id_, Server *my_server_);
    //functions
    void update_target_parameters();
    virtual void start_mining(); //to be overridden by fakeMiner
    bool mined_success(unsigned int crc_res) const;
    unsigned int calculate_hash_code();
};


