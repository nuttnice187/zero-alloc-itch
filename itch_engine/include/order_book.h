#pragma once
#include <cstdint>
#include <array>

// Ensure strict network alignment boundary maps
#pragma pack(push, 1)
struct OrderAddMessage {
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    uint64_t order_reference_number;
    char buy_sell_indicator; // 'B' = Buy, 'S' = Sell
    uint32_t shares;
    char stock[8];           // ITCH stock symbols span 8 bytes padding spaces
    uint32_t price;
};
#pragma pack(pop)

// Clean host representation structural primitive
struct DecodedOrder {
    uint64_t timestamp;
    uint64_t order_id;
    uint32_t shares;
    uint32_t price;
    char side;
};

// Align to CPU cache boundaries to maximize low-latency iteration speed
struct alignas(64) PriceLevel {
    uint32_t price = 0;
    uint32_t total_volume = 0;
    uint32_t total_orders = 0;
};

class LimitOrderBook {
private:
    // Fixed pre-allocated stack boundaries to securely prevent memory leaks or runtime allocation delays
    std::array<PriceLevel, 5000> bid_levels; 
    std::array<PriceLevel, 5000> ask_levels;
    size_t active_bids = 0;
    size_t active_asks = 0;

public:
    LimitOrderBook() = default;

    inline void handle_add_order(const DecodedOrder& order) {
        if (order.side == 'B') {
            // Fast linear scan optimization: search if price level array block already exists
            for (size_t i = 0; i < active_bids; ++i) {
                if (bid_levels[i].price == order.price) {
                    bid_levels[i].total_volume += order.shares;
                    bid_levels[i].total_orders++;
                    return;
                }
            }
            // If new level matches, insert into pre-allocated memory cell sequence safely
            if (active_bids < bid_levels.size()) {
                bid_levels[active_bids] = {order.price, order.shares, 1};
                active_bids++;
            }
        } else {
            for (size_t i = 0; i < active_asks; ++i) {
                if (ask_levels[i].price == order.price) {
                    ask_levels[i].total_volume += order.shares;
                    ask_levels[i].total_orders++;
                    return;
                }
            }
            if (active_asks < ask_levels.size()) {
                ask_levels[active_asks] = {order.price, order.shares, 1};
                active_asks++;
            }
        }
    }
};