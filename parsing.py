import sys

from data_models import Cache, Endpoint

class Params:
  v = 0
  e = 0
  r = 0
  c = 0
  x = 0


class Endp:
  l = 0
  k = 0
  clist = []
  def __init__(self,l,k):
   self.l = l
   self.k = k

class Caches:
  l = 0
  c = 0
  def __init__(self,c,l):
    self.l = l
    self.c = c

class Req:
  rv = 0
  re = 0
  rn = 0
  def __init__(self,rv,re,rn):
    self.rv = rv
    self.re = re
    self.rn = rn

def parsing(filename):

  vs = []

  p = Params()

  f = open(filename)
  l1 = f.readline().split()
  p.v = int(l1[0])
  p.e = int(l1[1])
  p.r = int(l1[2])
  p.c = int(l1[3])
  p.x = int(l1[4])

  print "v = {}; e = {}; r = {}; c = {}; x = {}".format(p.v,p.e,p.r,p.c,p.x)

  vs = f.readline().split()

  endpointlist = []
  for i in range(p.e):
    li = f.readline().split()
    endp = Endp(int(li[0]),int(li[1]))
    print "{} : cache {}".format(i,endp.k)
    for j in range(endp.k):
      ln = f.readline().split()
      cache = Caches(int(ln[0]),int(ln[1]))
      endp.clist.append(cache)
    endpointlist.append(endp)
 
  reqlist = [] 
  for k in range(p.r):
    lr = f.readline().split()
    rv = int(lr[0])
    re = int(lr[1])
    rn = int(lr[2])
    reqlist.append(Req(rv,re,rn))

  print "endpoints = {}".format(endpointlist)
  print "requests= {}".format(reqlist)

  f.close()

  cachelist = []
  for i in range(p.c):
    cachelist.append(Cache(i))

  for i in range(len(endpointlist)):
    endpoint = endpointlist[i]
    for caches in endpoint.clist:
      cachelist[caches.c].add_endpoint(i)

  oendpointlist = []
  for i in range(p.e):
    oendpointlist.append(Endpoint(i))
    for cache in endpointlist[i].clist:
      oendpointlist[i].add_cache(cache.c,cache.l)
  for req in reqlist:
    oendpointlist[req.re].add_request(req.rv,req.rn)

  return cachelist, oendpointlist


parsing("input01.txt")
