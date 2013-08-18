#!/usr/bin/env perl

use strict;
use warnings;

use JSON;

my $stminf = {
  PROGRAM_ID => 'i',
  AUDIO      => 'zqs',
  VIDEO      => 'zqs',
  SUBTITLES  => 'zqs',
  CODECS     => 'zqs',
  RESOLUTION => 'res',
};

my %spec = (
  EXT_X_MEDIA_SEQUENCE => 'i',
  EXT_X_TARGETDURATION => 'i',
  EXT_X_VERSION        => 'i',
  EXT_X_PLAYLIST_TYPE  => ['EVENT', 'VOD'],
  EXT_X_ALLOW_CACHE    => ['YES', 'NO'],
  EXT_X_MEDIA          => {
    require => {},
    allow   => {
      URI      => 'zqs',
      TYPE     => ['AUDIO', 'VIDEO', 'SUBTITLES'],
      GROUP_ID => 'zqs',
      LANGUAGE => 'zqs',
      NAME     => 'zqs',
      DEFAULT         => ['YES', 'NO'],
      AUTOSELECT      => ['YES', 'NO'],
      FORCED          => ['YES', 'NO'],
      CHARACTERISTICS => 'zqs',
    },
  },
  EXT_X_I_FRAME_STREAM_INF => {
    require => {
      BANDWIDTH => 'i',
      URI       => 'zqs',
    },
    allow => $stminf,
  },
  EXT_X_STREAM_INF => {
    require => { BANDWIDTH => 'i', },
    allow   => $stminf,
  },
  EXT_X_BYTERANGE         => 'br',
  EXTINF                  => 'extinf',
  EXT_X_PROGRAM_DATE_TIME => 'bs',
  EXT_X_I_FRAMES_ONLY     => [],
);

print JSON->new->pretty->canonical->encode( \%spec );

# vim:ts=2:sw=2:sts=2:et:ft=perl

