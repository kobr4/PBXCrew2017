from data_models import Cache, Endpoint

def display_result(cachelist):
  f = open('output.txt',w)
  f.write('{}',len(cachelist))

  for cache in cachelist:
    s = "" 
    for i,score in cache.selected_videos:
      s += i+" "
    s += "\n"
    f.write(s)

  f.close()
    
