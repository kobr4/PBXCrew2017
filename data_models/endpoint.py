import sys


class Endpoint:

  def _init__(self, db_latency=None):
    self.caches = {}
    self.requests = {}
    self.database = db_latency

  def add_cache(self, cache_index, latency):
    self.caches[cache_index] = latency

  def add_request(self, video_index, nb):
    self.requests[video_index] = nb

  def get_video_scores(self, cache_index, video_index):
    return self.caches[cache_index] * self.requests[video_index]

