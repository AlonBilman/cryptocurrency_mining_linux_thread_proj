
#include "server.h"
#include <iostream>

void* Server::server_thread_start(void* arg)
{
    auto* server= static_cast<Server*>(arg);
    server->start();
    return nullptr; //only for the void* to work. it won't return null ever.
}


//create one dummy node 
Server::Server(int difficulty_target) : difficulty_target(difficulty_target)
{
    pthread_mutex_lock(&bl_lock);
    next_block.set_difficulty(difficulty_target);
    block_chain.push_front(next_block);
    pthread_mutex_unlock(&bl_lock);
}

void Server::print_last_block(Block& block_added)
{
   std::cout << "Server: New block added by Miner #" << std::dec << block_added.get_relayed_by()
                     << ", height(" <<std::dec << block_added.get_height() << "), timestamp(" << block_added.get_timestamp()
                     << "), hash(0x" << std::hex << block_added.get_hash() << std::dec << "), prev_hash(0x"
                     << std::hex << block_added.get_prev_hash() << std::dec << "), nonce(" 
                     << block_added.get_nonce() << ")" << std::endl;
}

bool Server::verify_proof_of_work_(Block& block_to_check)
{
    if(block_to_check.get_difficulty() != difficulty_target)
    {
        std::cout<<"The Difficulty is wrong, Miner #"<<block_to_check.get_relayed_by()<<std::endl;
        return false;
    }
    else if(block_to_check.get_height() != number_of_blocks+1)
    {
        std::cout<<"Server: The Height is wrong, Miner #"<<block_to_check.get_relayed_by()<<std::endl;
        return false;
    }
    else if(block_to_check.get_prev_hash() != block_chain.front().get_hash())
    {
        std::cout<<"Server: The prev_hash is wrong, Miner #"<<block_to_check.get_relayed_by()<<std::endl;
        return false;
    }

    unsigned int hash_test = //this calculates the hash
        hash(block_to_check.get_height(),block_to_check.get_nonce(),(block_to_check.get_timestamp()),
             block_to_check.get_prev_hash(),block_to_check.get_relayed_by());
    
    if(hash_test != block_to_check.get_hash())
    {
        std::cout<<"Server: The Hash is wrong, Miner #"<<block_to_check.get_relayed_by()<<std::endl;
        return false;
    }
    else if((hash_test >> (32-difficulty_target)) != 0)
    {
        std::cout<<"Server: The Hash is wrong, Miner #"<<block_to_check.get_relayed_by()<<std::endl;
        return false;
    }

    return true; //if we survived all the checks.
}


void Server::add_block_(Block & block_to_add) //adding to block_chain. making sure that its secure
{
    pthread_mutex_lock(&bl_lock);
    block_chain.push_front(block_to_add);
    ++number_of_blocks;
    //calling the print function 
    print_last_block(block_to_add);
    if(block_chain.size() == MAX_CHAIN_SIZE)
    {
        block_chain.pop_back();
    }
    pthread_mutex_unlock(&bl_lock);
}

[[noreturn]] void Server::start()
{    
    while(true)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex); //waiting for a block to check
        Block curr_block_to_check = next_block; //copying the block

        if(verify_proof_of_work_(curr_block_to_check))
            add_block_(curr_block_to_check);

        pthread_mutex_unlock(&mutex);
    }
}

void Server::check_new_block(Block &new_block) {
    pthread_mutex_lock(&bl_lock);
    next_block = new_block; //we can change it to queue
    pthread_cond_signal(&cond); //notify the Server::start function to apply
    pthread_mutex_unlock(&bl_lock);
}

//Block data getters
int Server::get_latest_block_height() {
    pthread_mutex_lock(&bl_lock);
    auto res = block_chain.front().get_height();
    pthread_mutex_unlock(&bl_lock);
    return res;
}

/*int Server::get_latest_block_timestamp() {
    pthread_mutex_lock(&bl_lock);
    auto res = block_chain.front().get_timestamp();
    pthread_mutex_unlock(&bl_lock);
    return res;
}*/

unsigned int Server::get_latest_block_hash() {
    pthread_mutex_lock(&bl_lock);
    auto res = block_chain.front().get_hash();
    pthread_mutex_unlock(&bl_lock);
    return res;
}

/*unsigned int Server::get_latest_block_prev_hash() {
    pthread_mutex_lock(&bl_lock);
    auto res = block_chain.front().get_prev_hash();
    pthread_mutex_unlock(&bl_lock);
    return res;
}*/

int Server::get_latest_block_difficulty() {
    pthread_mutex_lock(&bl_lock);
    auto res = block_chain.front().get_difficulty();
    pthread_mutex_unlock(&bl_lock);
    return res;
}

/*
int Server::get_latest_block_nonce() {
    pthread_mutex_lock(&bl_lock);
    auto res = block_chain.front().get_nonce();
    pthread_mutex_unlock(&bl_lock);
    return res;
}

int Server::get_latest_block_relayed_by() {
    pthread_mutex_lock(&bl_lock);
    auto res = block_chain.front().get_relayed_by();
    pthread_mutex_unlock(&bl_lock);
    return res;
}
*/

