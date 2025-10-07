#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <iterator>

#include "LevelInfo.h"
#include "Usings.h"
#include "Order.h"
#include "OrderModify.h"
#include "OrderbookLevelInfos.h"
#include "Trade.h"

class OrderBook
{
private:
    struct OrderEntry
    {
        OrderPointer order_{ nullptr };
        OrderPointers::iterator location_;
    };

		struct LevelData
		{
			Quantity quantity_{ }; // Total quantity at that price
			Quantity count_{ }; // Number of orders at that price

			enum class Action
			{
				Add,
				Remove,
				Match,
			};
		};

		std::unordered_map<Price, LevelData> data_;
    std::map<Price, OrderPointers, std::greater<Price>> bids_;
    std::map<Price, OrderPointers, std::less<Price>> asks_;
    std::unordered_map<OrderId, OrderEntry> orders_;
		mutable std::mutex ordersMutex_;
		std::thread ordersPruneThread_;
		std::condition_variable shutdownConditionVariable_;
		std::atomic<bool> shutdown_{ false };

		void PruneGoodForDayOrders();

		void CancelOrders(OrderIds orderIds);
		void CancelOrderInternal(OrderId orderId);

		void OnOrderCancelled(OrderPointer order);
		void OnOrderAdded(OrderPointer order);
		void OnOrderMatched(Price price, Quantity quantity, bool isFullyFilled);
		void UpdateLevelData(Price price, Quantity quantity, LevelData::Action action);

		bool CanFullyFill(Side side, Price price, Quantity quantity) const;
    bool CanMatch(Side side, Price price) const;
    Trades MatchOrders();

public:
		OrderBook();
		~OrderBook();
		
    Trades AddOrder(OrderPointer order);
    void CancelOrder(OrderId orderId);
    Trades MatchOrder(OrderModify order);

    std::size_t Size() const;
    OrderBookLevelInfos GetOrderInfos() const;
};

