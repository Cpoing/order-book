#include "Orderbook.h"

bool OrderBook::CanMatch(Side side, Price price) const
{
    if (side == Side::Buy)
    {
        if (asks_.empty())
            return false;

        const auto& [bestAsk, _] = *asks_.begin();
        return price >= bestAsk;
    }
    else
    {
        if (bids_.empty())
            return false;

        const auto& [bestBid, _] = *bids_.begin();
        return price <= bestBid;
    }
}

Trades OrderBook::MatchOrders()
{
    Trades trades;
    trades.reserve(orders_.size());

    while (true)
    {
        if (bids_.empty() || asks_.empty())
            break;

        auto& [bidPrice, bids] = *bids_.begin();
        auto& [askPrice, asks] = *asks_.begin();

        if (bidPrice < askPrice)
            break;

        while (bids.size() && asks.size())
        {
            // time-price priority
            auto bid = bids.front();
            auto ask = asks.front();

            Quantity quantity = std::min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity());

            bid->Fill(quantity);
            ask->Fill(quantity);

            if (bid->IsFilled())
            {
                bids.pop_front();
                orders_.erase(bid->GetOrderId());
            }

            if (ask->IsFilled())
            {
                asks.pop_front();
                orders_.erase(ask->GetOrderId());
            }

            if (bids.empty())
                bids_.erase(bidPrice);

            if (asks.empty())
                asks_.erase(askPrice);

            trades.push_back(Trade{
                TradeInfo{ bid->GetOrderId(), bid->GetPrice(), quantity },
                TradeInfo{ ask->GetOrderId(), ask->GetPrice(), quantity }
            });
        }
    }

    if (!bids_.empty())
    {
        auto& [_, bids] = *bids_.begin();
        auto& order = bids.front();
        if (order->GetOrderType() == OrderType::FillAndKill)
            CancelOrder(order->GetOrderId());
    }

    if (!asks_.empty())
    {
        auto& [_, asks] = *asks_.begin();
        auto& order = asks.front();
        if (order->GetOrderType() == OrderType::FillAndKill)
            CancelOrder(order->GetOrderId());
    }

    return trades;
}

Trades OrderBook::AddOrder(OrderPointer order)
{
    if (orders_.contains(order->GetOrderId())) // Order is unique
        return { };

    if (order->GetOrderType() == OrderType::FillAndKill &&
        !CanMatch(order->GetSide(), order->GetPrice())) // Can't match a fill-and-kill order
        return { };

		if (order->GetOrderType() == OrderType::Market)
		{
			if (order->GetSide() == Side::Buy && !asks_.empty())
			{
				const auto& [worstAsk, _] = *asks_.rbegin();
				order->ToGoodTillCancel(worstAsk); // turns Market order to GoodTillCancel order with the price as the worst ask
			}
			else if (order->GetSide() == Side::Sell && !bids_.empty())
			{
				const auto& [worstBid, _] = *bids_.rbegin();
				order->ToGoodTillCancel(worstBid);
			}
			else
				return { };
		}

    OrderPointers::iterator iterator;

    if (order->GetSide() == Side::Buy)
    {
        auto& orders = bids_[order->GetPrice()];
        orders.push_back(order);
        iterator = std::next(orders.begin(), orders.size() - 1);
    }
    else
    {
        auto& orders = asks_[order->GetPrice()];
        orders.push_back(order);
        iterator = std::next(orders.begin(), orders.size() - 1);
    }

    orders_.insert({ order->GetOrderId(), OrderEntry{ order, iterator } });
    return MatchOrders(); // Vector of trades after matching all possible orders
}

void OrderBook::CancelOrder(OrderId orderId)
{
    if (!orders_.contains(orderId))
        return;

    const auto [order, iterator] = orders_.at(orderId);
    orders_.erase(orderId);

    if (order->GetSide() == Side::Sell)
    {
        auto price = order->GetPrice();
        auto& orders = asks_.at(price);
        orders.erase(iterator);
        if (orders.empty()) // if orders are empty, delete that price
            asks_.erase(price);
    }
    else
    {
        auto price = order->GetPrice();
        auto& orders = bids_.at(price);
        orders.erase(iterator);
        if (orders.empty())
            bids_.erase(price);
    }
}

Trades OrderBook::MatchOrder(OrderModify order)
{
    if (!orders_.contains(order.GetOrderId()))
        return { };

    const auto& [existingOrder, _] = orders_.at(order.GetOrderId());
    CancelOrder(order.GetOrderId());
    return AddOrder(order.ToOrderPointer(existingOrder->GetOrderType()));
}

std::size_t OrderBook::Size() const
{
    return orders_.size();
}

OrderBookLevelInfos OrderBook::GetOrderInfos() const
{
    LevelInfos bidInfos, askInfos;
    bidInfos.reserve(orders_.size());
    askInfos.reserve(orders_.size());

    auto CreateLevelInfos = [](Price price, const OrderPointers& orders) -> LevelInfo
    {
        return LevelInfo{
            price,
            std::accumulate(
                orders.begin(), orders.end(), static_cast<Quantity>(0),
                [](Quantity runningSum, const OrderPointer& order)
                { return runningSum + order->GetRemainingQuantity(); })
        };
    };

    for (const auto& [price, orders] : bids_)
        bidInfos.push_back(CreateLevelInfos(price, orders));

    for (const auto& [price, orders] : asks_)
        askInfos.push_back(CreateLevelInfos(price, orders));

    return OrderBookLevelInfos{ bidInfos, askInfos };
}

