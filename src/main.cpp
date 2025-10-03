#include <functional>
#include <iostream>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

enum class OrderType {
    GoodTilCancel,
    FillAndKill
};

enum class Side {
    Buy,
    Sell
};

using Price   = std::int32_t;
using Quantity= std::uint32_t;
using OrderId = std::uint64_t;

class OrderBook {
public:
    class Order {
    public:
        Order(OrderId id, OrderType type, Side side, Price price, Quantity quantity)
            : id{id}, type{type}, side{side}, price{price}, quantity{quantity} {}

        OrderId   id;
        OrderType type;
        Side      side;
        Price     price;
        Quantity  quantity;
    };

		void addOrder(const Order& order)
		{

		}
		bool cancelOrder(OrderId id);

		Price bestBid() const;
		Price bestAsk() const;
private:
	std::map< Price, std::vector<Order>, std::greater<Price> > bids;
	std::map< Price, std::vector<Order>, std::less<Price> > asks;

	std::unordered_map<OrderId, Order> allOrders;
};

int main() {
    OrderBook::Order o{1, OrderType::GoodTilCancel, Side::Buy, 100, 10};
    std::cout << "OK\n";
    return 0;
}
