#!/usr/bin/env perl

use strict;
use warnings;

use JSON;

my $stminf = {
  'PROGRAM-ID' => 'i',
  AUDIO        => 'zqs',
  VIDEO        => 'zqs',
  SUBTITLES    => 'zqs',
  CODECS       => 'zqs',
  RESOLUTION   => 'res',
};

my %spec = (
  'EXT-X-MEDIA-SEQUENCE' => 'i',
  'EXT-X-TARGETDURATION' => 'i',
  'EXT-X-VERSION'        => 'i',
  'EXT-X-PLAYLIST-TYPE'  => ['EVENT', 'VOD'],
  'EXT-X-ALLOW-CACHE'    => ['YES', 'NO'],
  'EXT-X-MEDIA'          => {
    require => {},
    allow   => {
      URI        => 'zqs',
      TYPE       => ['AUDIO', 'VIDEO', 'SUBTITLES'],
      'GROUP-ID' => 'zqs',
      LANGUAGE   => 'zqs',
      NAME       => 'zqs',
      DEFAULT         => ['YES', 'NO'],
      AUTOSELECT      => ['YES', 'NO'],
      FORCED          => ['YES', 'NO'],
      CHARACTERISTICS => 'zqs',
    },
  },
  'EXT-X-I-FRAME-STREAM-INF' => {
    require => {
      BANDWIDTH => 'i',
      URI       => 'zqs',
    },
    allow => $stminf,
  },
  'EXT-X-STREAM-INF' => {
    require => { BANDWIDTH => 'i', },
    allow   => $stminf,
  },
  'EXT-X-BYTERANGE'         => 'br',
  EXTINF                    => 'extinf',
  'EXT-X-PROGRAM-DATE-TIME' => 'bs',
  'EXT-X-I-FRAMES-ONLY'     => [],
);

print JSON->new->pretty->canonical->encode( \%spec );

# vim:ts=2:sw=2:sts=2:et:ft=perl

