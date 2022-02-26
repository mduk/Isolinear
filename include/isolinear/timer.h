#pragma once

namespace isolinear {

  class timer {
    public:
      asio::io_context& io_context;
      std::chrono::time_point<std::chrono::system_clock> started;
      asio::high_resolution_timer asio_timer;
      int ticks_remaining = 1;
      int ticks_elapsed = 0;

    public:
      miso::signal<int, int> signal_tick;
      miso::signal<> signal_expired;

    public:
      timer(asio::io_context& ioc, long int s)
        : io_context(ioc)
        , ticks_remaining(s)
        , started(std::chrono::system_clock::now())
        , asio_timer(ioc, std::chrono::seconds(1))
      {
        asio_timer.async_wait(std::bind(&timer::tick_handler, this, std::placeholders::_1));
      }

    public:
      void tick_handler(std::error_code) {
        if (ticks_remaining == 1) {
          emit signal_tick(ticks_remaining, ticks_elapsed);
          emit signal_expired();
        }
        else {
          emit signal_tick(ticks_remaining, ticks_elapsed);

          asio_timer.expires_at(asio_timer.expires_at() + std::chrono::seconds(1));
          asio_timer.async_wait(std::bind(&timer::tick_handler, this, std::placeholders::_1));
        }

        --ticks_remaining;
        ++ticks_elapsed;
      };
  };

}
