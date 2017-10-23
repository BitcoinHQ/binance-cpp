#include <binance/binance.hpp>
using namespace binance::endpoint;
using namespace binance::websocket_client;
using namespace binance::extra;

#include <memory>
#include <utility>
#include <iostream>
using namespace std;

const char* api_key = getenv("BINANCE_API_KEY");
const char* api_secret = getenv("BINANCE_API_SECRET");

auto pre_check() {
  if (!(api_key && api_secret))
    throw runtime_error("Please setup binance APIKEY and APISECRET!");
}

int main(int argc, char** argv) {
  pre_check();
  auto endpoint = make_shared<Endpoint>(api_key, api_secret);

  function<Maybe<AggTrade>(vector<AggTrade>)> get_first_agg_trade = [](const auto &ats) {
    if (ats.size() == 0) {
      return Nothing<AggTrade>;
    } else {
      return Maybe<AggTrade>(ats[0]);
    }
  };
  (endpoint->agg_trades("LTCBTC") >>= get_first_agg_trade) >>= print_result<AggTrade>;

  function<Maybe<OrderBookEntry>(OrderBook)> get_first_bid = [](const auto &ob) {
    if (ob.bids.size() == 0) {
      return Nothing<OrderBookEntry>;
    } else {
      return Maybe<OrderBookEntry>(ob.bids[0]);
    }
  };
  (endpoint->orderBook("LTCBTC", 5) >>= get_first_bid) >>= print_result<OrderBookEntry>;

  endpoint->ping() >>= print_result<json>;
  endpoint->time() >>= print_result<long>;
  endpoint->buy_limit("ETHBTC", 1.0, 0.069) >>= print_result<json>;
  endpoint->buy_market("ETHBTC", 1.0) >>= print_result<json>;
  endpoint->order_status("ETHBTC", "1") >>= print_result<json>;
  endpoint->cancle_order("ETHBTC", "1") >>= print_result<json>;

  endpoint->depth_websocket("ethbtc",  [](json data) {
      cout << data.dump(2) << endl;
    });
  endpoint->kline_websocket("ethbtc", "1m", [](json data) {
      cout << data.dump(2) << endl;
    });
  endpoint->trades_websocket("ethbtc", [](json data) {
      cout << data.dump(2) << endl;
    });
  auto jr = endpoint->start_user_data_stream();
  if (jr.isJust()) {
    auto listen_key = jr.fromJust()["listenKey"];
    endpoint->user_data_websockets(listen_key, [](json data) {
        cout << data.dump(2) << endl;
      });
  }
}
