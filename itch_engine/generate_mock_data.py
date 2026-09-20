import struct

def generate_mock_itch_file(filename="data/sample_itch_data.bin"):
    # Target structure format for packed C++ layout:
    # char(1), uint16(2), uint16(2), uint64(8), uint64(8), char(1), uint32(4), char(1), uint32(4)
    # Total Message Size = 36 bytes. Big-Endian network format denoted by '>'
    itch_struct_format = '>cHHQQcIcI'
    
    # List of mock orders to write to stream (Side, Shares, Price)
    mock_orders = [
        (b'B', 500,  15050),  # Buy 500 shares at $150.50
        (b'B', 1000, 15050),  # Buy 1000 shares at $150.50 (Increases existing level volume)
        (b'S', 200,  15100),  # Sell 200 shares at $151.00
        (b'B', 100,  14980),  # Buy 100 shares at $149.80 (Creates new level slot)
        (b'S', 800,  15220),  # Sell 800 shares at $152.20
    ]
    
    with open(filename, 'wb') as f:
        for i, (side, shares, price) in enumerate(mock_orders):
            # 1. Pack the individual order message fields into binary representation
            packed_message = struct.pack(
                itch_struct_format,
                b'A',          # message_type ('A' = Add Order)
                1,             # stock_locate
                101,           # tracking_number
                1718902800 + i,# timestamp
                100000 + i,    # order_reference_number
                side,          # buy_sell_indicator ('B' or 'S')
                shares,        # shares count
                b' ',          # stock padding dummy
                price          # price in integer format
            )
            
            # 2. NASDAQ ITCH prefixes every packet frame with a 2-byte Big-Endian length indicator
            message_length = len(packed_message)
            packed_length = struct.pack('>H', message_length)
            
            # 3. Stream data frames out onto disk storage
            f.write(packed_length)
            f.write(packed_message)
            
    print(f"[+] Successfully generated binary sample dataset containing {len(mock_orders)} messages at: {filename}")

if __name__ == "__main__":
    import os
    os.makedirs("data", exist_ok=True)
    generate_mock_itch_file()