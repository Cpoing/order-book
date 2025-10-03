#include <cstdint>
#include <list>
#include <memory>
#include <vector>
#include <format>

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

struct LevelInfo
{
	Price price_;
	Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderBookLevelInfos {
public:
	OrderBookLevelInfos(const LevelInfos& bids, const LevelInfos& asks) : bids_(bids), asks_(asks) { }

	const LevelInfos& GetBids() const { return bids_; }
	const LevelInfos& GetAsks() const { return asks_; }

private:
	LevelInfos bids_;
	LevelInfos asks_;
};

class Order
{
public:
	Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
		: orderType_(orderType)
		, orderId_(orderId)
		, side_(side)
		, price_(price)
		, intitialQuantity_(quantity)
		, remainingQuantity_(quantity)
	{ }

	OrderId GetOrderId() const { return orderId_; }
	Side GetSide() const { return side_; }
	Price GetPrice() const { return price_; }
	OrderType GetOrderType() const { return orderType_; }
	Quantity GetInitialQuantity() const { return intitialQuantity_; }
	Quantity GetRemainingQuantity() const { return remainingQuantity_; }
	Quantity GetFilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity(); }

	void Fill(Quantity quantity)
	{
		if (quantity > GetRemainingQuantity())
			throw std::logic_error(std::format("Order ({}) cannot be filled for more than its remaining quantity.", GetOrderId()));

		remainingQuantity_ -= quantity;
	}

private:
	OrderType orderType_;
	OrderId orderId_;
	Side side_;
	Price price_;
	Quantity intitialQuantity_;
	Quantity remainingQuantity_;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

int main() {
    return 0;
}














