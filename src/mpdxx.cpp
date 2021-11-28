#include <iostream>
#include <thread>
#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include <asio.hpp>
#include <fmt/core.h>


using std::cout;
using asio::ip::tcp;


asio::streambuf read_buffer;


std::vector<std::string> line_to_words(const std::string &line) {
    std::vector<std::string> words;

    std::istringstream iss(line);

    std::string word;
    while (std::getline(iss, word, ' ')) {
        words.emplace_back(word);
    }

    return words;
}


class Client {
  protected:
    std::string host = "127.0.0.1";
    std::string port = "6600";

    asio::io_context& io_context;
    asio::ip::tcp::socket io_socket;

  public:
    Client(asio::io_context& ioc)
      : io_context(ioc)
      , io_socket(io_context)
    {}

    void Connect() {
      tcp::resolver resolver(io_context);
      auto io_endpoints = resolver.resolve(host, port);

      asio::async_connect(io_socket, io_endpoints,
          [this](std::error_code ec, tcp::endpoint) {
            if (ec) {
              cout << "connect error: " << ec.message() << "\n";
              return;
            }

            ReadVersion();
          });
    }

    void ReadVersion() {
      asio::async_read_until(io_socket, read_buffer, '\n',
          [this] (std::error_code ec, std::size_t bytes_transferred) {
            std::string line(
                asio::buffers_begin(read_buffer.data()),
                asio::buffers_begin(read_buffer.data())
                  + bytes_transferred
              );

            read_buffer.consume(bytes_transferred);
            auto words = line_to_words(line);
            std::string version = words[2];
            cout << fmt::format("version: {}", version);

            RequestStatus();
          });
    }

    void ReadLine(std::list<std::string>& acc) {
      asio::async_read_until(io_socket, read_buffer, '\n',
          [this, &acc] (std::error_code ec, std::size_t bytes_transferred) {
            std::string line(
                asio::buffers_begin(read_buffer.data()),
                asio::buffers_begin(read_buffer.data())
                  + bytes_transferred
              );

            if (line == "OK\n") {
              cout << "All done\n";
              return;
            }

            acc.push_back(line);
            cout << fmt::format("recv: {}", line);
            read_buffer.consume(bytes_transferred);
            ReadLine(acc);
          });
    }

    void RequestStatus() {
      asio::async_write(io_socket, asio::buffer("status\n"),
          [this] (std::error_code ec, std::size_t length) {
            cout << fmt::format("sent: status\n");
            std::list<std::string> acc{};
            ReadLine(acc);
          });
    }

};


int main(int argc, char* argv[]) {
  cout << "MPDXX main()\n";

  try {
    asio::io_context io_context;
    auto work_guard = asio::make_work_guard(io_context);

    std::thread io_thread([&io_context](){
      io_context.run();
    });

    Client mpd(io_context);
    mpd.Connect();

    while(true);

    work_guard.reset();
    io_thread.join();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
