#pragma once

#include <iostream>

#include <fmt/core.h>

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


using std::cout;
using asio::ip::tcp;


namespace mpdxx {


  std::pair<std::string, std::string> line_to_pair(std::string &line) {
      return std::pair<std::string, std::string>{
        line.substr(0, line.find(": ")),
        line.substr(line.find(": ") + 2)
      };
  }

  std::vector<std::string> line_to_words(const std::string &line) {
      std::vector<std::string> words;

      std::istringstream iss(line);

      std::string word;
      while (std::getline(iss, word, ' ')) {
          words.emplace_back(word);
      }

      return words;
  }

  static inline void trim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
          return !std::isspace(ch);
      }));
      s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
          return !std::isspace(ch);
      }).base(), s.end());
  }


  class entity {
    protected:
      std::map<std::string, std::string> entitydata;

    public:
      template <class T>
      T value_or_default(std::string key, T default_value) const {
        if (entitydata.count(key)) {
          return entitydata.at(key);
        }
        return default_value;
      }

      void consume_pair(std::pair<std::string, std::string> pair) {
        entitydata.insert(pair);
      }

      friend std::ostream& operator<<(std::ostream& os, const entity& e);
  };

  std::ostream& operator<<(std::ostream& os, const entity& e) {
    for (auto& [key, value] : e.entitydata) {
      cout << fmt::format(" - {}: {}\n", key, value);
    }
    return os;
  }


  class status : public entity {
    public:
      std::string StateString() const {
        return entitydata.at("state");
      }

      bool const IsPaused() const {
        return StateString() == "pause";
      }

      bool const IsPlaying() const {
        return StateString() == "play";
      }

      bool const IsStopped() const {
        return StateString() == "stop";
      }

      bool const Consume() const {
        return entitydata.at("consume") == "1";
      }

      bool const Random() const {
        return entitydata.at("random") == "1";
      }

      bool const Repeat() const {
        return entitydata.at("repeat") == "1";
      }

      bool const Single() const {
        return entitydata.at("single") == "1";
      }

      unsigned const ElapsedTimeSeconds() const {
        auto elapsed = entitydata.at("elapsed");
        return std::stoi(elapsed.substr(0, elapsed.find('.')));
      }

      std::string ElapsedTimeString() const {
        auto const elapsed_seconds = ElapsedTimeSeconds();
        auto const minutes = elapsed_seconds / 60;
        auto const seconds = elapsed_seconds % 60;
        return fmt::format("{:02d}:{:02d}", minutes, seconds);
      }
  };


  class song : public entity {
    public:
      size_t size() const {
        return entitydata.size();
      }

      std::string const Title() const {
        return value_or_default<std::string>("Title", "");
      }

      std::string const Album() const {
        return value_or_default<std::string>("Album", "");
      }

      std::string const Artist() const {
        return value_or_default<std::string>("Artist", "");
      }

      std::string const DurationString() const {
        return entitydata.at("duration");
      }
  };


  class client {
    protected:
      asio::io_context& io_context;

      asio::ip::tcp::socket command_socket;
      asio::ip::tcp::socket idle_socket;

      asio::streambuf command_read_buffer;
      asio::streambuf idle_read_buffer;

      std::list<mpdxx::song> queue;
      std::list<mpdxx::song> current_song;

      mpdxx::status status;

    public:
      miso::signal<>                       signal_connected;
      miso::signal<mpdxx::status>          signal_status;
      miso::signal<mpdxx::song>            signal_current_song;
      miso::signal<std::list<mpdxx::song>> signal_queue;

    public:
      client(asio::io_context& ioc)
        : io_context(ioc)
        , command_socket(io_context)
        , idle_socket(io_context)
      { }

      void Connect(std::string host, std::string port) {
        tcp::resolver resolver(io_context);

        asio::async_connect(idle_socket, resolver.resolve(host, port),
            [this](std::error_code ec, tcp::endpoint) {
              if (ec) {
                cout << "connect error: " << ec.message() << "\n";
                return;
              }

              cout << "Idle socket connected\n";
              asio::read_until(idle_socket, idle_read_buffer, '\n');
              std::istream is(&idle_read_buffer);
              std::string line;
              std::getline(is, line);

              SendIdleRequest();
            });

        asio::async_connect(command_socket, resolver.resolve(host, port),
            [this](std::error_code ec, tcp::endpoint) {
              if (ec) {
                cout << "connect error: " << ec.message() << "\n";
                return;
              }

              ReadVersion();
            });
      }

      void RequestStatus() {
        SendCommandRequest("status", [this](){
          ReadStatusResponse();
        });
      }

      void RequestQueue() {
        SendCommandRequest("playlistinfo", [this](){
          queue.clear();
          ReadEntityResponse("queue", queue, "file");
        });
      }

      void RequestCurrentSong() {
        SendCommandRequest("currentsong",  [this](){
          current_song.clear();
          ReadEntityResponse("current_song", current_song, "file");
        });
      }

      bool TogglePause() {
        if (status.IsPaused()) {
          Resume();
        }
        else {
          Pause();
        }

        return !status.IsPaused();
      }

      void ToggleConsume() { SimpleCommand(fmt::format( "consume {}", status.Consume() ? "0" : "1")); }
      void ToggleRandom()  { SimpleCommand(fmt::format( "random {}",  status.Random()  ? "0" : "1")); }
      void ToggleSingle()  { SimpleCommand(fmt::format( "single {}",  status.Single()  ? "0" : "1")); }
      void ToggleRepeat()  { SimpleCommand(fmt::format( "repeat {}",  status.Repeat()  ? "0" : "1")); }

      void Stop()     { SimpleCommand("stop");     }
      void Play()     { SimpleCommand("play");     }
      void Pause()    { SimpleCommand("pause 1");  }
      void Resume()   { SimpleCommand("pause 0");  }
      void Next()     { SimpleCommand("next");     }
      void Previous() { SimpleCommand("previous"); }


    protected:

      void SendIdleRequest() {
        std::string send_command = "idle player playlist\n";

        asio::async_write(idle_socket, asio::buffer(send_command, send_command.size()),
            [this, send_command] (std::error_code ec, std::size_t length) {
              if (ec) {
                cout << fmt::format("SendIdleRequest: Error: {}\n", ec.message());
                return;
              }

              cout << fmt::format("SendIdleRequest: Sent {} bytes, string is {} bytes.\n", length, send_command.size());

              ReadIdleResponse();
            });
      }

      void ReadIdleResponse() {
        asio::async_read_until(idle_socket, idle_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadIdleResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&idle_read_buffer);
              std::string line;
              std::getline(is, line);

              trim(line);

              auto [key, val] = line_to_pair(line);
              if (val == "player") {
                cout << "Player changed, refreshing status\n";
                RequestStatus();
                SendIdleRequest();
              }
              if (val == "playlist") {
                cout << "Playlist changed, refreshing queue\n";
                RequestQueue();
                SendIdleRequest();
              }
            });
      }

      void SimpleCommand(std::string command) {
        SendCommandRequest(
            command,
            [this](){ ReadEmptyResponse(); }
          );
      }

      void ReadEmptyResponse() {
        asio::async_read_until(command_socket, command_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadEmptyResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&command_read_buffer);
              std::string line;
              std::getline(is, line);

              trim(line);

              if (line == "OK") {
                cout << fmt::format("ReadEmptyResponse: OK\n");
                RequestStatus();
              }
              else {
                cout << fmt::format("ReadEmptyResponse: Error: {}\n", line);
              }
            });
      }

      void ReadVersion() {
        asio::async_read_until(command_socket, command_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadVersion: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&command_read_buffer);
              std::string line;
              std::getline(is, line);

              auto words = line_to_words(line);
              std::string version = words[2];
              cout << fmt::format("server version: {}\n", version);

              emit signal_connected();
            });
      }

      void SendCommandRequest(std::string const command, std::function<void()> response_handler) {
        std::string send_command = command + '\n';

        asio::async_write(command_socket, asio::buffer(send_command, send_command.size()),
            [this, command, send_command, response_handler] (std::error_code ec, std::size_t length) {
              if (ec) {
                cout << fmt::format("SendCommandRequest<{}>: Error: {}\n", command, ec.message());
                return;
              }

              cout << fmt::format("SendCommandRequest<{}>: Sent {} bytes, string is {} bytes.\n", command, length, send_command.size());

              response_handler();
            });
      }

      void ReadStatusResponse() {
        asio::async_read_until(command_socket, command_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadStatusResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&command_read_buffer);
              std::string line;
              std::getline(is, line);

              //cout << fmt::format("ReadStatusResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                cout << fmt::format("ReadStatusResponse: OK\n");
                emit signal_status(status);
                return;
              }

              status.consume_pair(line_to_pair(line));

              ReadStatusResponse();
            });
      }

      template <class T>
      void ReadEntityResponse(std::string type, std::list<T>& into_list, std::string delimiter_key) {
        asio::async_read_until(command_socket, command_read_buffer, '\n',
            [type, delimiter_key, &into_list, this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadEntityResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&command_read_buffer);
              std::string line;
              std::getline(is, line);

              //cout << fmt::format("ReadEntityResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                cout << fmt::format("ReadEntityResponse: OK\n");

                if (type == "queue") {
                  emit signal_queue(queue);
                }

                if (type == "current_song") {
                  emit signal_current_song(current_song.back());
                }

                return;
              }

              auto pair = line_to_pair(line);

              if (pair.first == delimiter_key) {
                cout << fmt::format("ReadEntityResponse: Creating new {} instance\n", type);
                into_list.emplace_back();
              }

              into_list.back().consume_pair(pair);

              ReadEntityResponse<T>(type, into_list, delimiter_key);
            });
      }

  };

} // namespace mpdxx
