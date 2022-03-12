#pragma once

namespace mpdxx {

  static inline void trim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
          return !std::isspace(ch);
      }));
      s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
          return !std::isspace(ch);
      }).base(), s.end());
  }


  std::pair<std::string, std::string> line_to_pair(std::string &line) {
      std::string delimiter = ":";

      auto delimiter_length = delimiter.length();
      int delimiter_start_index = line.find(delimiter);
      int delimiter_end_index = delimiter_start_index + delimiter_length;

      std::string key = line.substr(0, delimiter_start_index);
      std::string val = line.substr(delimiter_end_index);
      trim(val);

      return std::pair<std::string, std::string>{ key, val };
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


  using event = std::string;


  class command_client {

    protected:
      std::string host;
      std::string port;

      asio::io_context& io_context;
      asio::ip::tcp::socket socket;
      asio::ip::tcp::resolver socket_resolver;
      asio::streambuf socket_read_buffer;

      std::string command;
      std::list<mpdxx::idle> entities;
      std::string entity_delimiter_key = "changed";


    public:
      command_client(asio::io_context& ioc, std::string h, std::string p, std::string c, std::string edk)
        : io_context(ioc)
        , host(h)
        , port(p)
        , socket(io_context)
        , socket_resolver(io_context)
        , command(c)
        , entity_delimiter_key(edk)
      {
        connect();
      }


    protected:

      std::string read_line() {
        std::istream is(&socket_read_buffer);
        std::string line;
        std::getline(is, line);
        trim(line);
        return line;
      }

      virtual void connect() {
        asio::async_connect(socket, socket_resolver.resolve(host, port),
            [this](std::error_code ec, asio::ip::tcp::endpoint) {
              if (ec) {
                cout << "connect error: " << ec.message() << "\n";
                return;
              }

              asio::read_until(socket, socket_read_buffer, '\n');
              std::istream is(&socket_read_buffer);
              std::string line;
              std::getline(is, line);

              send_command();
            });
      }

      virtual void send_command() {
        std::string send_str = command + "\n";
        asio::async_write(socket, asio::buffer(send_str, send_str.size()),
            [this](std::error_code ec, std::size_t length) {
              if (ec) {
                cout << fmt::format("command_client: send_command: Error: {}\n", ec.message());
                return;
              }

              read_response();
            });
      }

      void read_response() {
        asio::async_read_until(socket, socket_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("command_client: read_response: Error: {}\n", ec.message());
                return;
              }

              auto line = read_line();

              if (line == "OK") {
                complete();
                return;
              }
              auto pair = line_to_pair(line);

              if (pair.first == entity_delimiter_key) {
                entities.emplace_back();
              }

              entities.back().consume_pair(line_to_pair(line));
              read_response();
            });
      }

      virtual void complete() {
        socket.close();
      }

      virtual void process_line(std::string line) { }

  };

  class polling_client : public command_client {

    public:
      miso::signal<std::string> signal_change;

    public:
      polling_client(asio::io_context& ioc, std::string h, std::string p)
        : command_client(ioc, h, p, "idle playlist player mixer options", "changed")
      {}

    protected:
      virtual void complete() override {
        emit signal_change(entities.back().changed());
        send_command();
      }
  };




  class client {
    protected:
      asio::io_context& io_context;

      asio::ip::tcp::socket command_socket;
      asio::ip::tcp::resolver resolver;

      asio::streambuf command_read_buffer;

      std::list<mpdxx::song> queue;
      std::list<mpdxx::song> current_song;
      std::list<mpdxx::artist> artist_list;

      mpdxx::status status;

    public:
      miso::signal<>                         signal_connected;
      miso::signal<mpdxx::status>            signal_status;
      miso::signal<mpdxx::song>              signal_current_song;
      miso::signal<std::list<mpdxx::song>>   signal_queue;
      miso::signal<std::list<mpdxx::artist>> signal_artist_list;

    public:
      client(asio::io_context& ioc, std::string host, std::string port)
        : io_context(ioc)
        , resolver(io_context)
        , command_socket(io_context)
      {
        asio::async_connect(command_socket, resolver.resolve(host, port),
            [this](std::error_code ec, asio::ip::tcp::endpoint) {
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

      void RequestQueue() {
        SendCommandRequest("playlistinfo", [this](){
          queue.clear();
          ReadEntityResponse<mpdxx::song>("queue", queue, "file");
        });
      }

      void RequestCurrentSong() {
        SendCommandRequest("currentsong",  [this](){
          current_song.clear();
          ReadEntityResponse<mpdxx::song>("current_song", current_song, "file");
        });
      }

      void RequestArtistList() {
        SendCommandRequest("list artist", [this](){
            artist_list.clear();
            ReadEntityResponse<mpdxx::artist>("artist_list", artist_list, "Artist");
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

      void SimpleCommand(std::string command) {
        SendCommandRequest(
            command,
            [this](){ ReadEmptyResponse(); }
          );
      }

      std::string read_line() {
        std::istream is(&command_read_buffer);
        std::string line;
        std::getline(is, line);
        trim(line);
        return line;
      }

      void ReadEmptyResponse() {
        asio::async_read_until(command_socket, command_read_buffer, '\n',
            [this] (std::error_code ec, std::size_t bytes_transferred) {
              if (ec) {
                cout << fmt::format("ReadEmptyResponse: Error: {}\n", ec.message());
                return;
              }

              auto line = read_line();

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

              auto line = read_line();
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

              auto line = read_line();
              //cout << fmt::format("ReadStatusResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

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

              auto line = read_line();
              //cout << fmt::format("ReadEntityResponse: [{:2d} bytes] {}\n", bytes_transferred, line);

              if (line == "OK") {
                cout << fmt::format("ReadEntityResponse: OK\n");

                if (type == "queue") {
                  emit signal_queue(queue);
                }

                if (type == "current_song") {
                  emit signal_current_song(current_song.back());
                }

                if (type == "artist_list") {
                  emit signal_artist_list(artist_list);
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

};
