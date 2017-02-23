from parsing import parsing
import random



def iterative_assignement(caches, endpoints, videos):

  for cache in caches.values():
    cache.create_candidate_videos(endpoints)

  shuffled_indexes = random.shuffle(caches.keys())

  while len(shuffled_indexes) > 0:
    for k in shuffled_indexes:
      if not cache[k].pick_candidate(endpoints, caches, videos):
          shuffled_indexes.pop(k)




cachelist, endpoints, videos = parsing("./data/input01.txt")

iterative_assignement(cachelist, endpoints, videos)


