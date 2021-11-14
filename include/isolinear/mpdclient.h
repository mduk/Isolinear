#pragma once

#include <mpd/client.h>
#include <mpd/entity.h>
#include <mpd/message.h>
#include <mpd/search.h>
#include <mpd/status.h>
#include <mpd/tag.h>

namespace MPD {

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

  };

  using Queue_t = std::list<Song>;

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

      Queue_t Queue() {
        std::list<Song> queue;
        mpd_send_list_queue_meta(conn);
        struct mpd_song *song;
        while ((song = mpd_recv_song(conn)) != NULL) {
          queue.emplace_back(song);
        }
        return queue;
      }

      ~Client() {
        mpd_connection_free(conn);
      }
  };
}
