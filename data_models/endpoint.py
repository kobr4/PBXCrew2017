import sys


class Endpoint:

  def __init__(self, index, db_latency=None):
    self.index = index
    self.caches_lat = {}
    self.requests_nb = {}
    self.database_lat = db_latency

  def add_cache(self, cache_index, latency):

    self.caches_lat[cache_index] = latency

  def add_request(self, video_index, nb):
    print video_index
    self.requests_nb[video_index] = nb

  def get_video_scores(self, cache_index, video_index):
    print "video_score", self.caches_lat, self.requests_nb, cache_index, video_index
    return (self.database_lat - self.caches_lat[cache_index]) * self.requests_nb[video_index]

  def notify_caches(self, caches, cache_index, video_index, already_notified):

    # if not concerned
    if video_index not in self.requests_nb:
      return already_notified

    score = self.get_video_scores(cache_index, video_index)
    for c in caches:
      if caches[c].index in already_notified:
        continue
      if video_index in caches[c].candidates_videos.keys():
        caches[c].candidates_videos[video_index] -= self.get_video_scores(caches[c].index, video_index)
        already_notified.append(caches[c].index)
        #v2 c.candidates_video[video_index] -= score
    return already_notified


