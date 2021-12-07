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


  using StringMap = std::map<std::string, std::string>;
  using OutputID = int;
  using SongID = int;


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


  class Output {
    protected:
      StringMap const outputs;

    public:
      Output(StringMap const od)
        : outputs(od)
      {}

      OutputID const ID() const {
        return std::stoi(outputs.at("outputid"));
      }

      std::string const Name() const {
        return outputs.at("outputname");
      }

      std::string const Plugin() const {
        return outputs.at("plugin");
      }

      bool const Enabled() const {
        return outputs.at("outputenabled") == "1";
      }

  };


  class Song {
    protected:
      StringMap const songdata;

    public:
      Song(StringMap const sd)
        : songdata(sd)
      { }

      SongID const ID() const {
        return std::stoi(songdata.at("Id"));
      }

      std::string const Uri() const {
        return songdata.at("file");
      }

      std::string const Title() const {
        return songdata.at("Title");
      }

      std::string const Artist() const {
        return songdata.at("Artist");
      }

      std::string const Album() const {
        return songdata.at("Album");
      }

      unsigned const DurationSeconds() const {
        return 100;
      }

      std::string const DurationString() const {
        auto duration_seconds = DurationSeconds();
        auto const minutes = duration_seconds / 60;
        auto const seconds = duration_seconds % 60;
        return fmt::format("{:02d}:{:02d}", minutes, seconds);
      }
  };


  using SongList = std::list<Song>;
  using OutputList = std::list<Output>;


  class Queue {
    protected:
      SongList songs;
      std::string version;
  };


  class Client {
    protected:
      asio::io_context& io_context;

      asio::ip::tcp::socket command_socket;
      asio::ip::tcp::socket idle_socket;

      asio::streambuf command_read_buffer;
      asio::streambuf idle_read_buffer;

      std::mutex command_mutex;

      StringMap status;
      StringMap current_song;
      std::list<StringMap> queue;
      std::list<StringMap> outputs;


    public:
      miso::signal<>                     signal_connected;
      miso::signal<StringMap>            signal_status;
      miso::signal<StringMap>            signal_current_song;
      miso::signal<std::list<StringMap>> signal_queue;
      miso::signal<std::list<StringMap>> signal_outputs;

    public:
      Client(asio::io_context& ioc)
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
          status.clear();
          ReadStatusResponse();
        });
      }

      void RequestOutputs() {
        SendCommandRequest("outputs", [this](){
          outputs.clear();
          ReadOutputsResponse();
        });
      }

      void RequestQueue() {
        SendCommandRequest("playlistinfo", [this](){
          queue.clear();
          ReadQueueResponse();
        });
      }

      void RequestCurrentSong() {
        SendCommandRequest("currentsong",  [this](){
          current_song.clear();
          ReadCurrentSongResponse();
        });
      }

      std::string const StatusString() const {
        return status.at("state");
      }

      unsigned const ElapsedTimeSeconds() const {
        auto elapsed = status.at("elapsed");
        return std::stoi(elapsed.substr(0, elapsed.find('.')));
      }

      std::string ElapsedTimeString() const {
        auto const elapsed_seconds = ElapsedTimeSeconds();
        auto const minutes = elapsed_seconds / 60;
        auto const seconds = elapsed_seconds % 60;
        return fmt::format("{:02d}:{:02d}", minutes, seconds);
      }

      bool const IsPaused() const {
        return StatusString() == "pause";
      }

      bool const IsPlaying() const {
        return StatusString() == "play";
      }

      bool const IsStopped() const {
        return StatusString() == "stop";
      }

      Song const CurrentlyPlaying() const {
        return Song(current_song);
      }

      bool const Consume() const {
        return status.at("consume") == "1";
      }

      bool const Random() const {
        return status.at("random") == "1";
      }

      bool const Repeat() const {
        return status.at("repeat") == "1";
      }

      bool const Single() const {
        return status.at("single") == "1";
      }

      bool TogglePause() {
        if (IsPaused()) {
          Resume();
        }
        else {
          Pause();
        }

        return !IsPaused();
      }

      void ToggleConsume() { SimpleCommand(fmt::format( "consume {}", Consume() ? "0" : "1")); }
      void ToggleRandom()  { SimpleCommand(fmt::format( "random {}",  Random()  ? "0" : "1")); }
      void ToggleSingle()  { SimpleCommand(fmt::format( "single {}",  Single()  ? "0" : "1")); }
      void ToggleRepeat()  { SimpleCommand(fmt::format( "repeat {}",  Repeat()  ? "0" : "1")); }

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
                command_mutex.unlock();
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
        command_mutex.lock();
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
                command_mutex.unlock();
                cout << fmt::format("ReadStatusResponse: OK\n");
                emit signal_status(status);
                return;
              }

              auto [key, val] = line_to_pair(line);
              status[key] = val;

              ReadStatusResponse();
            });
      }

      void ReadCurrentSongResponse() {
        asio::async_read_until(command_socket, command_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadCurrentSongResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&command_read_buffer);
              std::string line;
              std::getline(is, line);

              //cout << fmt::format("ReadCurrentSongResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                command_mutex.unlock();
                cout << "ReadCurrentSongResponse: OK\n";

                if (current_song.size() == 0) {
                  cout << "ReadCurrentSongResponse: No current song.\n";
                }
                else {
                  cout << "ReadCurrentSongResponse: Emit signal_current_song.\n";
                  emit signal_current_song(current_song);
                }

                return;
              }

              auto [key, val] = line_to_pair(line);
              current_song[key] = val;

              ReadCurrentSongResponse();
            });
      }

      void ReadQueueResponse() {
        asio::async_read_until(command_socket, command_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadQueueResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&command_read_buffer);
              std::string line;
              std::getline(is, line);

              //cout << fmt::format("ReadQueueResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                command_mutex.unlock();
                cout << fmt::format("ReadQueueResponse: OK\n");
                emit signal_queue(queue);
                return;
              }

              auto [key, val] = line_to_pair(line);

              if (key == "file") {
                queue.emplace_back();
              }

              queue.back()[key] = val;

              ReadQueueResponse();
            });
      }

      void ReadOutputsResponse() {
        asio::async_read_until(command_socket, command_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadOutputsResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&command_read_buffer);
              std::string line;
              std::getline(is, line);

              //cout << fmt::format("ReadOutputsResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                command_mutex.unlock();
                cout << fmt::format("ReadOutputsResponse: OK\n");
                emit signal_outputs(outputs);
                return;
              }

              auto [key, val] = line_to_pair(line);

              if (key == "outputid") {
                outputs.emplace_back();
              }

              outputs.back()[key] = val;

              ReadOutputsResponse();
            });
      }

  };

} // namespace mpdxx
