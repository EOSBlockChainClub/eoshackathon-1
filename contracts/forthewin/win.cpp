#include <stdlib.h>
#include <eosiolib/eosio.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/multi_index.hpp>

struct mytransfer
 {
    eosio::name from;
    eosio::name to;
    eosio::asset        quantity;
    std::string       memo;
    EOSLIB_SERIALIZE( mytransfer, (from)(to)(quantity)(memo) )
 };

using namespace eosio;

class [[eosio::contract]] forthewin : public eosio::contract {

public:
  using contract::contract;

  // EOS6etY7qrqV7ZLcDNJwviWM2zPrqgbEzi2h1CF9s7mXLsFUcGqaN
  uint64_t charity_address = 1;

  [[eosio::action]]
  void receive(uint64_t self, uint64_t code) {
  //void receive(uint64_t self, uint64_t code, mytransfer data) {

    eosio::print("In receive", "\n");
    // unpack the action as a transfer
    auto data = eosio::unpack_action_data<mytransfer>();
    eosio::print("After unpack", "\n");

    if(data.from.value == self)
      eosio::print("Send is from self", "\n");
      eosio::print("from ", data.from.value, "\n");
      return;
    if(data.to.value != self)
      eosio::print("To is not to self", "\n");
      eosio::print("to", data.to.value, "\n");
      return;

    eosio::print("Before quantity", "\n");
    eosio_assert(data.quantity.is_valid(), "Invalid quantity");
    eosio::print("After assert", "\n");

    auto symbol = data.quantity.symbol;

    // Convert memo to integer
    std::string s = data.memo;
    auto score = atoi(s.c_str());
    eosio::print("After atoi", "\n");

    game_index games(_code, _code.value);
    auto iterator = games.find(1);
    eosio::print("After find", "\n");

    // If no game, create a new game
    if( iterator == games.end() )
    {
      eosio::print("no game", "\n");
      games.emplace(data.from, [&]( auto& row ) {
       row.key = 1;
       row.player_1 = data.from.value;
       row.player_1_score = score;
       row.wager = data.quantity.amount;
      });
    }

    else {
      eosio::print("existing game", "\n");
      auto& row = games.get(1);
      uint64_t winner;

      if (row.player_1_score >= score) {
          auto winner = row.player_1;
      }
      else {
          auto winner = data.from.value;
      }

      //cleanup
      games.erase(row);
      eosio::print("game erased", "\n");

      uint64_t charityamount = 9;
      uint64_t winneramount = 1;

      eosio::asset EOStoUser = eosio::asset(winneramount, symbol);
      eosio::asset EOStoCharity = eosio::asset(charityamount, symbol);


      eosio::action(eosio::permission_level{ _self, name("active") }, name("eosio.token"), name("transfer"),
      make_tuple(_self, winner, EOStoUser, std::string("Thank you for playing!"))).send();
      eosio::print("after first send", "\n");

      // Once we have charity address put it below where 'winner' is
      eosio::action(eosio::permission_level{ _self, name("active") }, name("eosio.token"), name("transfer"),
      make_tuple(_self, charity_address, EOStoCharity, std::string("Here is a gift!"))).send();
      eosio::print("after second send", "\n");
    }

  }

private: 
  struct [[eosio::table]] game {
    uint8_t  key;
    uint64_t player_1;
    uint32_t player_1_score;
    uint64_t wager;
    uint64_t primary_key() const { return key; }
  };
  typedef eosio::multi_index<"games"_n, game> game_index;


};

//EOSIO_DISPATCH( forthewin, (receive) )
//
#define EOSIO_ABI_EX( TYPE, MEMBERS ) \
extern "C" { \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      auto self = receiver; \
      if( code == self || code == name("eosio.token").value) { \
      	 if( action == name("transfer").value){ \
      	 	eosio_assert( code == name("eosio.token").value, "Must transfer EOS"); \
      	 } \
         TYPE thiscontract( self); \
         switch( action ) { \
            EOSIO_API( TYPE, MEMBERS ) \
         } \
      } \
   } \
}

EOSIO_ABI_EX(forthewin, (receive))

/*
extern "C" {
  void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    eosio::print("Receiver ", eosio::name(receiver), " code ",eosio::name(code), "\n");
    if(code==name("eosio.token").value && action== name("transfer").value) {
      eosio::print("Received eosio.token transfer. Executing action", "\n");
      auto data = eosio::unpack_action_data<mytransfer>();
      eosio::print(data.to, data.from, data.quantity, "\n");
      execute_action(name(receiver), name(code),  &forthewin::receive(receiver, code, data));
    }
  }
}
*/
