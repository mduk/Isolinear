#include <iostream>
#include <thread>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include <asio.hpp>
#include <fmt/core.h>
#include <miso.h>


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

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

class Client {
  protected:
    std::string host = "127.0.0.1";
    std::string port = "6600";

    asio::io_context& io_context;
    asio::ip::tcp::socket io_socket;

    std::map<std::string, std::string> status;
    miso::signal<> signal_command_completed;

  public:
    Client(asio::io_context& ioc)
      : io_context(ioc)
      , io_socket(io_context)
    {
      miso::connect(signal_command_completed, [&](){
        for (auto const& [ key, val ] : status) {
          cout << fmt::format("  - {} = {}\n", key, val);
        }
      });
    }

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
            cout << fmt::format("server version: {}", version);

            SendStatusRequest();
          });
    }

    void SendStatusRequest() {
      std::string command = "status\n";
      asio::async_write(io_socket, asio::buffer(command, command.size()),
          [this, command] (std::error_code ec, std::size_t length) {
            ReadStatusResponse();
            if (ec) {
              cout << fmt::format("SendStatusRequest: Error: {}\n", ec.message());
              return;
            }

            cout << fmt::format("SendStatusRequest: Sent {} bytes, string is {} bytes.\n", length, command.size());
          });
    }

    void ReadStatusResponse() {
      asio::async_read_until(io_socket, read_buffer, '\n',
          [this] (std::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
              cout << fmt::format("ReadStatusResponse: Error: {}\n", ec.message());
              return;
            }

            std::string line(
                asio::buffers_begin(read_buffer.data()),
                asio::buffers_begin(read_buffer.data())
                  + bytes_transferred
              );
            read_buffer.consume(bytes_transferred);

            trim(line);

            if (line == "OK") {
              SendQueueRequest();
              return;
            }

            auto key = line.substr(0, line.find(": "));
            auto val = line.substr(line.find(": ") + 2);

            status[key] = val;


            ReadStatusResponse();
          });
    }

    void SendQueueRequest() {
      std::string command = "playlistinfo\n";
      asio::async_write(io_socket, asio::buffer(command, command.size()),
          [this, command] (std::error_code ec, std::size_t length) {
            ReadQueueResponse();
            if (ec) {
              cout << fmt::format("SendQueueRequest: Error: {}\n", ec.message());
              return;
            }

            cout << fmt::format("SendQueueRequest: Sent {} bytes, string is {} bytes.\n", length, command.size());
            ReadQueueResponse();
          });
    }

    void ReadQueueResponse() {
      asio::async_read_until(io_socket, read_buffer, '\n',
          [this] (std::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
              cout << fmt::format("ReadQueueResponse: Error: {}\n", ec.message());
              return;
            }

            std::string line(
                asio::buffers_begin(read_buffer.data()),
                asio::buffers_begin(read_buffer.data())
                  + bytes_transferred
              );
            read_buffer.consume(bytes_transferred);

            cout << "recv: " << line;
            trim(line);

            if (line == "OK") {
              cout << "All done\n";
              return;
            }

            ReadQueueResponse();
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
