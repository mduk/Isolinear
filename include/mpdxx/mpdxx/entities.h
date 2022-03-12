#pragma once


using std::cout;

namespace mpdxx {

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

      void clear() {
        entitydata.clear();
      }

      size_t size() const {
        return entitydata.size();
      }


      friend std::ostream& operator<<(std::ostream& os, const entity& e);
  };

  std::ostream& operator<<(std::ostream& os, const entity& e) {
    for (auto& [key, value] : e.entitydata) {
      cout << fmt::format(" - {}: {}\n", key, value);
    }
    return os;
  }


  class idle : public entity {
    public:
      std::string changed() const {
        return entitydata.at("changed");
      }
  };


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

      std::string const file() const {
        return entitydata.at("file");
      }

      std::string const Header() const {
        auto artist = Artist();
        auto album = Album();
        auto title = Title();

        if (artist != "" && title != "") {
          return fmt::format("{} - {}", Artist(), Title());
        }

        if (artist != "" && album != "") {
          return fmt::format("{} - {}", Artist(), Album());
        }

        if (album != "") {
          return album;
        }

        return file();
      }
  };


  class artist : public entity {
    public:
      std::string const Name() const {
        return value_or_default<std::string>("Artist", "");
      }

      std::string const Header() const {
        return Name();
      }
  };

}
