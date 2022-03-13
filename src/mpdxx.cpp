#include <iostream>
#include <thread>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <iterator>

#include <asio.hpp>
#include <fmt/core.h>
#include <miso.h>

#include <mpdxx.h>


using std::cout;
using asio::ip::tcp;


auto h = "localhost";
auto p = "6600";


int main(int argc, char* argv[]) {
  cout << "MPDXX main()\n";

  try {
    asio::io_context io_context;
    auto work_guard = asio::make_work_guard(io_context);

    std::thread io_thread([&io_context](){
      io_context.run();
    });

    mpdxx::status_client sc(io_context, h, p);
    mpdxx::queue_client qc(io_context, h, p);

    mpdxx::polling_client mpd(io_context, h, p);
    miso::connect(mpd.signal_change, [&](std::string subsystem){
      cout << fmt::format("Change: {}\n", subsystem);
    });

    while(true);

    work_guard.reset();
    io_thread.join();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
