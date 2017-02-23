import sys


class Cache:

  def _init__(self, index, capacity=None):
    self.index = index
    self.endpoints_indexes = []
    self.candidates_videos = {}
    self.selected_videos = []
    self.capacity = capacity

  def add_endpoint(self, endpoint_index):
    self.endpoints_indexes.append(endpoint_index)

  def add_video_size(self, videos):
    self.videos = videos

  def create_candidate_videos(self, endpoints):

    for e in self.endpoints_indexes:
      for video_index in endpoints[e].request.keys():
        if video_index not in self.candidates_videos:
          self.candidates_videos[video_index] = 0
        self.candidates_videos[video_index] += endpoints[e].\
                                               get_video_scores(self.index, video_index)

    print(self.candidates_videos)

  def pick_candidate(self, endpoints, caches):
    best_index = sorted([(idx, self.candidates_videos[idx])
                         for idx in self.candidates_videos.keys()], key=lambda t: -t[1])[0]
    self.selected_videos.append(best_index)
    del self.selected_videos[best_index]

    already_notified = []
    for e in self.endpoints_indexes:
      already_notified = endpoints[e].notify_caches(caches=caches,
                                                    cache_index=self.index,
                                                    video_index=best_index,
                                                    already_notified=already_notified)


