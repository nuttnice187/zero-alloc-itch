# zero-alloc-itch
A zero-allocation, cache-aligned binary NASDAQ ITCH 4.1 protocol processing engine built in native C++20 for sub-microsecond state tracking.

## getting started
```bash
sudo apt-get update
sudo apt-get install -y build-essential
git clone https://github.com/nuttnice187/zero-alloc-itch.git
cd zero-alloc-itch/itch_engine/
make
./bin/itch_engine data/sample_itch_data.bin
```
