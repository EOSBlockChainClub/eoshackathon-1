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
  forthewin(eosio::name receiver, eosio::name code,  eosio::datastream<const char*> ds): contract(receiver, code, ds) {}

  uint64_t charity_address = 1;

  [[eosio::action]]
  void receive(uint64_t self, uint64_t code) {
    // unpack the action as a transfer
    auto data = eosio::unpack_action_data<mytransfer>();

    if(data.from.value == self || data.to.value != self)
      return;
    eosio_assert(data.quantity.is_valid(), "Invalid quantity");

    auto symbol = data.quantity.symbol;

    // Convert memo to integer
    std::string s = data.memo;
    auto score = atoi(s.c_str());

    game_index games(_code, _code.value);
    auto iterator = games.find(1);

    // If no game, create a new game
    if( iterator == games.end() )
    {
      games.emplace(data.from, [&]( auto& row ) {
       row.key = 1;
       row.player_1 = data.from.value;
       row.player_1_score = score;
       row.wager = data.quantity.amount;
      });
    }

    else {
      auto& row = games.get(1);
      uint64_t winner;
      if (row.player_1_score >= score) {
          auto winner = row.player_1;
      }
      else {
          auto winner = data.from;
      }

      //cleanup
      games.erase(row);

      uint64_t charityamount = 9;
      uint64_t winneramount = 1;

      eosio::asset EOStoUser = eosio::asset(winneramount, symbol);
      eosio::asset EOStoCharity = eosio::asset(charityamount, symbol);


      eosio::action(eosio::permission_level{ _self, name("active") }, name("eosio.token"), name("transfer"),
      make_tuple(_self, winner, EOStoUser, std::string("Thank you for playing!"))).send();

      // Once we have charity address put it below where 'winner' is
      eosio::action(eosio::permission_level{ _self, name("active") }, name("eosio.token"), name("transfer"),
      make_tuple(_self, charity_address, EOStoCharity, std::string("Here is a gift!"))).send();
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

EOSIO_DISPATCH( forthewin, (receive))
