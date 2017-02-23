from parsing import parsing
import random



def iterative_assignement(caches, endpoints, videos):

  for cache in caches.values():
    cache.create_candidate_videos(endpoints)
    print("\n\n")

  print("Chahce {}".format(caches.keys()))
  shuffled_indexes = caches.keys()
  random.shuffle(shuffled_indexes)

  while len(shuffled_indexes) > 0:
    for k in shuffled_indexes:
      if not caches[k].pick_candidate(endpoints, caches, videos):
          shuffled_indexes.remove(k)




cachelist, endpoints, videos = parsing("./data/input01.txt")
caches = {c.index: c for c in cachelist}
endpoints_ = {e.index: e for e in endpoints}

print("\n\n")
print([c.endpoints_indexes for c in caches.values()], "\n",
      [e.requests_nb for e in endpoints_.values()], "\n",
       videos)

iterative_assignement(caches, endpoints_, videos)

print("Result : {}".format(sum([v.get_scores() for v in caches.values()])))


