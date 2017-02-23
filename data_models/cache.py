import sys


class Cache:

  def _init__(self, capacity=None):
    self.endpoints = []
    self.candidates_videos = {}
    self.selected_videos = []
    self.capacity = capacity

  def add_endpoint(self, endpoint_index):
    self.endpoints.append(endpoint_index)


