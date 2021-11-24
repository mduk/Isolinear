#pragma once

#include <iostream>

#include <fmt/core.h>

#include <mpd/client.h>
#include <mpd/entity.h>
#include <mpd/message.h>
#include <mpd/search.h>
#include <mpd/status.h>
#include <mpd/tag.h>

namespace MPDXX {

  using OutputID = uint32_t;

  class Output {
    protected:
      mpd_connection* conn;
      unsigned id;
      bool enabled;
      std::string name;
      std::string plugin;

    public:
      Output(mpd_connection* c, mpd_output* o)
        : conn(c)
        , id(mpd_output_get_id(o))
        , name(mpd_output_get_name(o))
        , plugin(mpd_output_get_plugin(o))
        , enabled{mpd_output_get_enabled(o)}
      {
        mpd_output_free(o);
      }

      OutputID ID() const {
        return id;
      }

      std::string Name() const {
        return name;
      }

      std::string Plugin() const {
        return plugin;
      }

      bool Enabled() const {
        return enabled;
      }

      bool Disable() {
        mpd_run_disable_output(conn, id);
        enabled = false;
        return enabled;
      }

      bool Enable() {
        mpd_run_enable_output(conn, id);
        enabled = true;
        return enabled;
      }
  };

  using SongID = uint32_t;

  class Song {
    protected:
      mpd_connection* conn;
      mpd_song* song;

      unsigned int id;
      unsigned int duration_seconds;
      std::string uri;
      std::string title;
      std::string artist;
      std::string album;

    public:
      Song(mpd_connection* c, mpd_song* s)
        : conn(c)
        , song(s)
        , id(mpd_song_get_id(s))
        , duration_seconds(mpd_song_get_duration(s))
        , uri(mpd_song_get_uri(s))
        , title(mpd_song_get_tag(s, MPD_TAG_TITLE, 0) ?: "")
        , artist(mpd_song_get_tag(s, MPD_TAG_ARTIST, 0) ?: "")
        , album(mpd_song_get_tag(s, MPD_TAG_ALBUM, 0) ?: "")
      {
        mpd_song_free(s);
      }

      SongID const ID() const {
        return id;
      }

      std::string const Uri() const {
        return uri;
      }

      std::string const Title() const {
        return title;
      }

      std::string const Artist() const {
        return artist;
      }

      std::string const Album() const {
        return album;
      }

      unsigned const DurationSeconds() const {
        return duration_seconds;
      }

      std::string const DurationString() const {
        auto const minutes = duration_seconds / 60;
        auto const seconds = duration_seconds % 60;
        return fmt::format("{:02d}:{:02d}", minutes, seconds);
      }

      void Play() const {
        mpd_run_play_id(conn, ID());
      }

  };

  using SongList = std::list<Song>;
  using OutputList = std::list<Output>;

  class Client {
    protected:
      struct mpd_connection* conn;
      struct mpd_status* status;
      struct mpd_song* song;

    public:
      Client() {
        conn = mpd_connection_new(NULL, 0, 30000);
      }

      int Status() {
        status = mpd_run_status(conn);
        return mpd_status_get_state(status);
      }

      std::string StatusString() {
        status = mpd_run_status(conn);
        switch (mpd_status_get_state(status)) {
          case MPD_STATE_PAUSE: return "PAUSED";
          case MPD_STATE_PLAY: return "PLAYING";
          case MPD_STATE_STOP: return "STOPPED";
          case MPD_STATE_UNKNOWN: return "UNKNOWN";
        }
        return "";
      }

      unsigned const ElapsedTimeSeconds() const {
        return mpd_status_get_elapsed_ms(status) / 1000;
      }

      std::string ElapsedTimeString() const {
        auto const elapsed_seconds = ElapsedTimeSeconds();
        auto const minutes = elapsed_seconds / 60;
        auto const seconds = elapsed_seconds % 60;
        return fmt::format("{:02d}:{:02d}", minutes, seconds);
      }

      bool IsPaused() {
        return Status() == MPD_STATE_PAUSE;
      }

      bool IsPlaying() {
        return Status() == MPD_STATE_PLAY;
      }

      bool IsStopped() {
        return Status() == MPD_STATE_STOP;
      }

      Song CurrentlyPlaying() {
        return Song(conn, mpd_run_current_song(conn));
      }



      bool Consume() {
        status = mpd_run_status(conn);
        return mpd_status_get_consume(status);
      }

      bool Random() {
        status = mpd_run_status(conn);
        return mpd_status_get_random(status);
      }

      void Stop() {
        mpd_run_stop(conn);
      }

      void Play() {
        mpd_run_play(conn);
      }

      void Pause() {
        mpd_run_pause(conn, true);
      }

      void Resume() {
        mpd_run_pause(conn, false);
      }

      void Next() {
        mpd_run_next(conn);
      }

      void Previous() {
        mpd_run_previous(conn);
      }

      bool PauseToggle() {
        bool newstate = !IsPaused();
        mpd_run_pause(conn, newstate);
        return newstate;
      }

      bool ConsumeToggle() {
        status = mpd_run_status(conn);
        bool newstate = !mpd_status_get_consume(status);
        mpd_run_consume(conn, newstate);
        return newstate;
      }

      bool RandomToggle() {
        status = mpd_run_status(conn);
        bool newstate = !mpd_status_get_random(status);
        mpd_run_random(conn, newstate);
        return newstate;
      }

      SongList Queue() {
        std::list<Song> queue;
        mpd_send_list_queue_meta(conn);
        struct mpd_song *song;
        while ((song = mpd_recv_song(conn)) != NULL) {
          queue.emplace_back(conn, song);
        }
        return queue;
      }

      OutputList Outputs() {
        OutputList outputs;
        mpd_send_outputs(conn);
        struct mpd_output *output;
        while ((output = mpd_recv_output(conn)) != NULL) {
          outputs.emplace_back(conn, output);
        }
        return outputs;
      }

      ~Client() {
        mpd_connection_free(conn);
      }
  };
}
