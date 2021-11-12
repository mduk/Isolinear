#pragma once

#include <mpd/client.h>
#include <mpd/entity.h>
#include <mpd/message.h>
#include <mpd/search.h>
#include <mpd/status.h>
#include <mpd/tag.h>

namespace MPD {

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

      bool IsPaused() {
        return Status() == MPD_STATE_PAUSE;
      }

      bool IsPlaying() {
        return Status() == MPD_STATE_PLAY;
      }

      bool IsStopped() {
        return Status() == MPD_STATE_STOP;
      }

      std::string CurrentlyPlayingAlbum() {
        return CurrentlyPlayingTag(MPD_TAG_ALBUM);
      }

      std::string CurrentlyPlayingArtist() {
        return CurrentlyPlayingTag(MPD_TAG_ARTIST);
      }

      std::string CurrentlyPlayingTitle() {
        return CurrentlyPlayingTag(MPD_TAG_TITLE);
      }

      std::string CurrentlyPlayingTag(mpd_tag_type tag) {
        song = mpd_run_current_song(conn);
        if (!song) {
          return " ";
        }

        auto value = mpd_song_get_tag(song, tag, 0);
        if (!value) {
          return " ";
        }
        return std::string(value);
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

      ~Client() {
        mpd_connection_free(conn);
      }
  };
}
