import sys


class Endpoint:

  def _init__(self, index, db_latency=None):
    self.index = index
    self.caches_lat = {}
    self.requests_nb = {}
    self.database_lat = db_latency

  def add_cache(self, cache_index, latency):
    self.caches_lat[cache_index] = latency

  def add_request(self, video_index, nb):
    self.requests_nb[video_index] = nb

  def get_video_scores(self, cache_index, video_index):
    return (self.db_latency - self.caches_lat[cache_index]) * self.requests_nb[video_index]

  def notify_caches(self, caches, cache_index, video_index, already_notified):
    score = self.get_video_scores(cache_index, video_index)
    for c in caches:
      if c.index in already_notified:
        continue
      if video_index in c.candidates_video.keys():
        c.candidates_video[video_index] -= self.get_video_scores(c.index, video_index)
        already_notified.append(c.index)
        #v2 c.candidates_video[video_index] -= score
    return already_notified


