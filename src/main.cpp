#include <iostream>
#include "Orderbook.h"

int main() {
	OrderBook orderbook;
	printf("test 1\n");
	const OrderId orderId = 1;
	printf("test 2\n");
	orderbook.AddOrder(std::make_shared<Order>(OrderType::GoodTilCancel, orderId, Side::Buy, 100, 10));
	printf("test 3\n");
	std::cout << orderbook.Size() << std::endl;
	printf("test 4\n");
	orderbook.CancelOrder(orderId);
	printf("test 5\n");
	std::cout << orderbook.Size() << std::endl;
	return 0;
}
