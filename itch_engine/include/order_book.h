#pragma once
#include <cstdint>
#include <array>
#include <iostream>

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
    
    void print_top_of_book() const {
        if (active_bids == 0 && active_asks == 0) {
            std::cout << "[Book Status] No active levels recorded." << std::endl;
            return;
        }

        uint32_t best_bid = 0;
        uint32_t bid_vol = 0;
        // Linear scan over active cache blocks to find max bid price
        for (size_t i = 0; i < active_bids; ++i) {
            if (bid_levels[i].price > best_bid) {
                best_bid = bid_levels[i].price;
                bid_vol = bid_levels[i].total_volume;
            }
        }

        uint32_t best_ask = 0xFFFFFFFF; // Initialize to max int bounds
        uint32_t ask_vol = 0;
        // Linear scan over active cache blocks to find min ask price
        for (size_t i = 0; i < active_asks; ++i) {
            if (ask_levels[i].price < best_ask) {
                best_ask = ask_levels[i].price;
                ask_vol = ask_levels[i].total_volume;
            }
        }

        std::cout << "\n=============================================" << std::endl;
        std::cout << "        LIMIT ORDER BOOK TOP METRICS         " << std::endl;
        std::cout << "=============================================" << std::endl;
        if (active_bids > 0) {
            std::cout << " BEST BID (BUY)  : $" << (best_bid / 100.0) << " | Volume: " << bid_vol << std::endl;
        } else {
            std::cout << " BEST BID (BUY)  : NO DATA" << std::endl;
        }
        if (active_asks > 0 && best_ask != 0xFFFFFFFF) {
            std::cout << " BEST ASK (SELL) : $" << (best_ask / 100.0) << " | Volume: " << ask_vol << std::endl;
        } else {
            std::cout << " BEST ASK (SELL) : NO DATA" << std::endl;
        }
        std::cout << "=============================================\n" << std::endl;
    }
};