import sys

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
vs = []

p = Params()

f = open(sys.argv[1])
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
    cache = Caches(ln[0],ln[1])
    endp.clist.append(cache)
  endpointlist.append(endp)

print "endpoints = {}".format(endpointlist)
