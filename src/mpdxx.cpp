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

  using SongID = uint32_t;
  using StringMap = std::map<std::string, std::string>;



  class Song {
    protected:
      StringMap const songdata;

    public:
      Song(StringMap const sd)
        : songdata(sd)
      { }

      SongID const ID() const {
        return 0;
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
/*
      void Play() const {
        mpd_run_play_id(conn, ID());
      }
*/
  };


  using SongList = std::list<Song>;


  class Client {
    protected:
      asio::io_context& io_context;
      asio::ip::tcp::socket io_socket;

      StringMap status;
      StringMap current_song;
      std::list<StringMap> queue;
      std::list<StringMap> outputs;

      miso::signal<> signal_command_completed;

      asio::streambuf read_buffer;


    public:
      Client(asio::io_context& ioc)
        : io_context(ioc)
        , io_socket(io_context)
      {
        miso::connect(signal_command_completed, [&](){
          cout << "Status:\n";
          for (auto const& [ key, val ] : status) {
            cout << fmt::format("  - {} = {}\n", key, val);
          }

          cout << "Outputs:\n";
          for (auto const& output : outputs) {
            cout << fmt::format("  - {}\n", output.at("outputname"));
          }

          cout << "Current Song:\n";
          for (auto const& [ key, val ] : current_song) {
            cout << fmt::format("  - {} = {}\n", key, val);
          }

          auto current_file = current_song.at("file");

          cout << "Queue:\n";
          for (auto const& song : queue) {
            auto file = song.at("file");
            if (file == current_file) {
              cout << fmt::format(" => {}\n", song.at("file"));
            }
            else {
              cout << fmt::format("  - {}\n", song.at("file"));
            }
          }
        });
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
/*
      unsigned const ElapsedTimeSeconds() const {
        return mpd_status_get_elapsed_ms(status) / 1000;
      }

      std::string ElapsedTimeString() const {
        auto const elapsed_seconds = ElapsedTimeSeconds();
        auto const minutes = elapsed_seconds / 60;
        auto const seconds = elapsed_seconds % 60;
        return fmt::format("{:02d}:{:02d}", minutes, seconds);
      }
*/

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

      bool Consume() {}
      bool Random() {}

      void Stop() {}
      void Play() {}
      void Pause() {}
      void Resume() {}
      void Next() {}
      void Previous() {}

      bool PauseToggle() {}
      bool ConsumeToggle() {}
      bool RandomToggle() {}

      SongList Queue() {
        SongList songs;
        for (auto const& song : queue) {
          songs.emplace_back(song);
        }
        return songs;
      }


    protected:

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
                return;
              }

              auto [key, val] = line_to_pair(line);

              if (key == "outputid") {
                outputs.emplace_back();
                outputs.back()[key] = val;
              }
              else {
                outputs.back()[key] = val;
              }

              ReadOutputsResponse();
            });
      }

  };

} // namespace mpdxx

int main(int argc, char* argv[]) {
  cout << "MPDXX main()\n";

  try {
    asio::io_context io_context;
    auto work_guard = asio::make_work_guard(io_context);

    std::thread io_thread([&io_context](){
      io_context.run();
    });

    mpdxx::Client mpd(io_context);
    mpd.Connect("localhost", "6600");

    while(true);

    work_guard.reset();
    io_thread.join();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
