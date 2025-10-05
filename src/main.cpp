#include <iostream>

#include "Orderbook.h"

int main() {
	OrderBook orderbook;
	const OrderId orderId = 1;
	orderbook.AddOrder(std::make_shared<Order>(OrderType::GoodTilCancel, orderId, Side::Buy, 100, 10));
	std::cout << orderbook.Size() << std::endl;
	orderbook.CancelOrder(orderId);
	std::cout << orderbook.Size() << std::endl;
	return 0;
}
