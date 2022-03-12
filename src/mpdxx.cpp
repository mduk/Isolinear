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


int main(int argc, char* argv[]) {
  cout << "MPDXX main()\n";

  try {
    asio::io_context io_context;
    auto work_guard = asio::make_work_guard(io_context);

    std::thread io_thread([&io_context](){
      io_context.run();
    });

    mpdxx::status_client sc(io_context, "localhost", "6600");
/*
    mpdxx::polling_client mpd(io_context, "localhost", "6600");
    miso::connect(mpd.signal_change, [&](std::string subsystem){
      if (subsystem == "player") {
        cout << "PLAYER!\n";
      }
      else if (subsystem == "playlist") {
        cout << "QUEUE!\n";
      }
    });
*/
    while(true);

    work_guard.reset();
    io_thread.join();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
