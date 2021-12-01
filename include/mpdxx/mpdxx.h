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
      StringMap const outputdata;

    public:
      Output(StringMap const od)
        : outputdata(od)
      {}

      OutputID const ID() const {
        return std::stoi(outputdata.at("outputid"));
      }

      std::string const Name() const {
        return outputdata.at("outputname");
      }

      std::string const Plugin() const {
        return outputdata.at("plugin");
      }

      bool const Enabled() const {
        return outputdata.at("outputenabled") == "1";
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


  class Client {
    protected:
      asio::io_context& io_context;
      asio::ip::tcp::socket io_socket;

      StringMap status;
      StringMap current_song;
      std::list<StringMap> queue;
      std::list<StringMap> outputdata;

      asio::streambuf read_buffer;

    public:
      miso::signal<> signal_command_completed;

    public:
      Client(asio::io_context& ioc)
        : io_context(ioc)
        , io_socket(io_context)
      {
      }

      void Connect(std::string host, std::string port) {
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

      bool const Single() const {
        return status.at("single") == "1";
      }

      bool ConsumeToggle() {
        SimpleCommand(fmt::format(
            "consume {}",
            Consume() ? "0" : "1"
          ));
      }

      bool RandomToggle() {
        SimpleCommand(fmt::format(
            "random {}",
            Random() ? "0" : "1"
          ));
      }

      bool SingleToggle() {
        SimpleCommand(fmt::format(
            "single {}",
            Single() ? "0" : "1"
          ));
      }


      void Stop() {
        SimpleCommand("stop");
      }

      void Play() {
        SimpleCommand("play");
      }

      void Pause() {
        SimpleCommand("pause 1");
      }

      void Resume() {
        SimpleCommand("pause 0");
      }

      void Next() {
        SimpleCommand("next");
      }

      void Previous() {
        SimpleCommand("previous");
      }

      StringMap Status() {
        return status;
      }

      StringMap CurrentSong() {
        return current_song;
      }

      SongList Queue() {
        SongList songs;
        for (auto const& song : queue) {
          songs.emplace_back(song);
        }
        return songs;
      }

      OutputList Outputs() {
        OutputList outputs;
        for (auto const& output : outputdata) {
          outputs.emplace_back(output);
        }
        return outputs;
      }


    protected:

      void SimpleCommand(std::string command) {
        SendCommandRequest(
            command,
            [this](){ ReadEmptyResponse(); }
          );
      }

      void ReadEmptyResponse() {
        asio::async_read_until(io_socket, read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadEmptyResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&read_buffer);
              std::string line;
              std::getline(is, line);

              trim(line);

              if (line == "OK") {
                cout << fmt::format("ReadEmptyResponse: OK\n");
              }
              else {
                cout << fmt::format("ReadEmptyResponse: Error: {}\n", line);
              }
            });
      }

      void ReadVersion() {
        asio::async_read_until(io_socket, read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadVersion: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&read_buffer);
              std::string line;
              std::getline(is, line);

              auto words = line_to_words(line);
              std::string version = words[2];
              cout << fmt::format("server version: {}\n", version);

              SendCommandRequest("status", [this](){ ReadStatusResponse(); });
            });
      }

      void SendCommandRequest(std::string const command, std::function<void()> response_handler) {
        std::string send_command = command + '\n';

        asio::async_write(io_socket, asio::buffer(send_command, send_command.size()),
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
        asio::async_read_until(io_socket, read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadStatusResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&read_buffer);
              std::string line;
              std::getline(is, line);

              cout << fmt::format("ReadStatusResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                SendCommandRequest("currentsong", [this](){ ReadCurrentSongResponse(); });
                return;
              }

              auto [key, val] = line_to_pair(line);
              status[key] = val;

              ReadStatusResponse();
            });
      }

      void ReadCurrentSongResponse() {
        asio::async_read_until(io_socket, read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadCurrentSongResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&read_buffer);
              std::string line;
              std::getline(is, line);

              cout << fmt::format("ReadCurrentSongResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                SendCommandRequest("playlistinfo", [this](){ ReadQueueResponse(); });
                return;
              }

              auto [key, val] = line_to_pair(line);
              current_song[key] = val;

              ReadCurrentSongResponse();
            });
      }

      void ReadQueueResponse() {
        asio::async_read_until(io_socket, read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadQueueResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&read_buffer);
              std::string line;
              std::getline(is, line);

              cout << fmt::format("ReadStatusResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                SendCommandRequest("outputs", [this](){ ReadOutputsResponse(); });
                return;
              }

              auto [key, val] = line_to_pair(line);

              if (key == "file") {
                queue.emplace_back();
                queue.back()[key] = val;
              }
              else {
                queue.back()[key] = val;
              }

              ReadQueueResponse();
            });
      }

      void ReadOutputsResponse() {
        asio::async_read_until(io_socket, read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadOutputsResponse: Error: {}\n", ec.message());
                return;
              }

              std::istream is(&read_buffer);
              std::string line;
              std::getline(is, line);

              cout << fmt::format("ReadOutputsResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              trim(line);

              if (line == "OK") {
                emit signal_command_completed();
                ConsumeToggle();
                return;
              }

              auto [key, val] = line_to_pair(line);

              if (key == "outputid") {
                outputdata.emplace_back();
                outputdata.back()[key] = val;
              }
              else {
                outputdata.back()[key] = val;
              }

              ReadOutputsResponse();
            });
      }

  };

} // namespace mpdxx
