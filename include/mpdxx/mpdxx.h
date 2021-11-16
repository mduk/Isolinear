#pragma once

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
      mpd_output* output;

    public:
      Output(
          mpd_connection* c,
          mpd_output* o
        ) :
          conn(c),
          output(o)
      {}

      ~Output() {
        mpd_output_free(output);
      }

      OutputID ID() const {
        return mpd_output_get_id(output);
      }

      std::string Name() const {
        return mpd_output_get_name(output);
      }

      std::string Plugin() const {
        return mpd_output_get_plugin(output);
      }

      bool Enabled() const {
        return mpd_output_get_enabled(output);
      }

      bool EnabledToggle() const {
        return mpd_run_toggle_output(conn, mpd_output_get_id(output));
      }

      bool Disable() const {
        return mpd_run_disable_output(conn, mpd_output_get_id(output));
      }

      bool Enable() const {
        return mpd_run_enable_output(conn, mpd_output_get_id(output));
      }
  };

  class Song {
    protected:
      mpd_song* song;

    public:
      Song(mpd_song* s) : song(s) {}
      ~Song() {
        mpd_song_free(song);
      }

      std::string Uri() const {
        return mpd_song_get_uri(song);
      }

      std::string Tag(mpd_tag_type tag) const {
        auto value = mpd_song_get_tag(song, tag, 0);
        if (!value) {
          return "";
        }
        return value;
      }

      std::string Title() const {
        return Tag(MPD_TAG_TITLE);
      }

      std::string Artist() const {
        return Tag(MPD_TAG_ARTIST);
      }

      std::string Album() const {
        return Tag(MPD_TAG_ALBUM);
      }

      std::string Duration() const {
        auto const duration = mpd_song_get_duration(song);
        auto const minutes = duration / 60;
        auto const seconds = duration % 60;
        return fmt::format("{:02d}:{:02d}", minutes, seconds);
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

      std::string ElapsedTime() const {
        auto const elapsed_seconds = mpd_status_get_elapsed_ms(status) / 1000;
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
        return Song(mpd_run_current_song(conn));
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
          queue.emplace_back(song);
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
