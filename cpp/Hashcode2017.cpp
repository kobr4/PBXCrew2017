#include <stdio.h>
#include <stdlib.h>

typedef struct {
 int v;
 int e;
 int r;
 int c;
 int x;
} T_PARAMS;

T_PARAMS g_params;

typedef struct {
  int cid;
  int latency;
} T_CONNECTION;

typedef struct {
  int latency;
  int cache_srv;
  T_CONNECTION * conlist;
} T_ENDPOINT;


typedef struct {
  int vid;
  int endid;
  int nreq;
  int vsize;
} T_REQUEST;


typedef struct {
  int vcount;
  int * vlist;
  int free;
} T_CACHE;

int * vsize;


T_CACHE * init_cache(T_PARAMS params) {
	T_CACHE * cachelist = (T_CACHE*)malloc(sizeof(T_CACHE)*params.c);

	for (int i = 0;i < params.c;i++) {
		cachelist[i].vcount = 0;
		cachelist[i].vlist = (int *)malloc(sizeof(int)*params.v);
		cachelist[i].free = params.x;
	}

	return cachelist;
}

int add_video(T_CACHE * cachelist,int i, int vid,int w) {
	if (cachelist[i].free < w) return -1;
	cachelist[i].vlist[cachelist[i].vcount] = vid;
	cachelist[i].vcount++;
	cachelist[i].free -= w;

	return 0;
}


int read_int(FILE * f) {
	char str[10];
	int c = fgetc(f);
	int count = 0;
	while (c != EOF && c != ' ' && c!= '\n') {
		str[count] = c; 
		count++;
		c = fgetc(f);
	}
	str[count] = 0;
	return atoi(str);
}

int compare2(void const *a, void const *b) {
	T_REQUEST * req1 = (T_REQUEST*)a;
	T_REQUEST * req2 = (T_REQUEST*)b;
	return (req2->nreq * req1->vsize) - (req1->nreq * req2->vsize);
}

int compare(void const *a, void const *b) {
	T_REQUEST * req1 = (T_REQUEST*)a;
	T_REQUEST * req2 = (T_REQUEST*)b;
	return req2->nreq - req1->nreq;
}

// Greedy solver
void solver(T_REQUEST * reqlist, T_ENDPOINT * endpointlist, T_CACHE * cachelist, int * vsize, int (*compare) (void const *a, void const *b)) {
	qsort(reqlist,g_params.r,sizeof(T_REQUEST),compare);
	for (int i=0;i<g_params.r;i++) {
		T_REQUEST req = reqlist[i];
		int l = endpointlist[req.endid].latency;
		int cid = -1;
		for (int j = 0;j < endpointlist[req.endid].cache_srv;j++) {
			T_CONNECTION con = endpointlist[req.endid].conlist[j];
			if (l > con.latency) {
				l = con.latency;
				cid = con.cid;
			}
		}

		if (cid != -1) add_video(cachelist,cid,req.vid,vsize[req.vid]);
	}
}

unsigned int get_endpoint_latency(T_ENDPOINT endp,int cid) {
	int latency = endp.latency;
	for (int i=0;i<endp.cache_srv;i++) {
		if (endp.conlist[i].cid == cid) {
			if (latency > endp.conlist[i].latency) {
				latency =  endp.conlist[i].latency;
			}
		}
	}
	return endp.latency-latency;
}

double score_cache(T_REQUEST * reqlist,T_ENDPOINT * endpointlist,int vid, int cid) {
	double acc = 0;
	for (int i = 0;i < g_params.r;i++) {
		
		if (reqlist[i].vid == vid) {
			acc += get_endpoint_latency(endpointlist[reqlist[i].endid], cid) * reqlist[i].nreq;
		}
		
	}
	
	return acc;
}

int cache_contains(T_CACHE cache, int vid) {
	for (int i =0;i < cache.vcount;i++) {
		if (cache.vlist[i] == vid) return 1;
	}
	return 0;
}

double score_cache(T_REQUEST * reqlist, T_ENDPOINT * endpointlist, T_CACHE * cachelist,int vid) {
	double sc = 0;
	for (int i = 0;i < g_params.r;i++) {
		if (vid == reqlist[i].vid) {
			unsigned int l = endpointlist[reqlist[i].endid].latency;

		
			for (int j = 0;j < endpointlist[reqlist[i].endid].cache_srv;j++) {
			
				T_CONNECTION con = endpointlist[reqlist[i].endid].conlist[j];
			
				for (int k = 0;k < cachelist[con.cid].vcount;k++) {
				
					if (cachelist[con.cid].vlist[k] == reqlist[i].vid) {
						if (l > con.latency) {
							l = con.latency;
						}
					}
				}	
			}
		
			sc += (endpointlist[reqlist[i].endid].latency - l) * reqlist[i].nreq;
		}
	}

	return sc;
}

double delta_score_cache(T_REQUEST * reqlist, T_ENDPOINT * endpointlist, T_CACHE * cachelist,int cid,int vid, double ori_sc) {
	
	cachelist[cid].vlist[cachelist[cid].vcount] = vid;
	cachelist[cid].vcount++;
	double sc = score_cache(reqlist,endpointlist,cachelist,vid) - ori_sc;
	cachelist[cid].vcount--;
	return sc;
}

// Per video solver
void solverV(T_REQUEST * reqlist, T_ENDPOINT * endpointlist, T_CACHE * cachelist, int * vsize, int (*compare) (void const *a, void const *b)) {
	double score = 0;
	int selec_v = 0;
	int selec_c = 0;
	double global_score = 0.;
	do {
		score = 0;
		selec_v = 0;
		selec_c =0;
		for (int i = 0;i < g_params.v;i++) {
			printf("Analyze video %d/%d nb cache=%d score=%.0f global=%.0f\n",i,g_params.v,g_params.c,score,global_score);
			double ori_sc = score_cache(reqlist,endpointlist,cachelist,i);
			for (int k = 0;k < g_params.c;k++) {
				if ((cachelist[k].free > vsize[i]) && !cache_contains(cachelist[k],i)) {
					
					//double curr_score = score_cache(g_params, reqlist,endpointlist,i,k);
					double curr_score = delta_score_cache(reqlist, endpointlist, cachelist,k,i,ori_sc);
					if (curr_score > score) {
						score = curr_score;
						selec_v = i;
						selec_c = k;
					}
					
				}
			}
		}
		if (score != 0) {
			printf("Adding video : %d to cache %d score=%.0f\n",selec_v,selec_c,score);
			add_video(cachelist,selec_c,selec_v,vsize[selec_v]);
			global_score += score;
		}
	} while (score != 0);
}


unsigned int score_simple(T_REQUEST * reqlist, T_ENDPOINT * endpointlist, T_CACHE * cachelist) {
	double sc = 0;
	double totnr = 0;
	for (int i = 0;i < g_params.r;i++) {
		unsigned int l = endpointlist[reqlist[i].endid].latency;

		
		for (int j = 0;j < endpointlist[reqlist[i].endid].cache_srv;j++) {
			
			T_CONNECTION con = endpointlist[reqlist[i].endid].conlist[j];
			
			for (int k = 0;k < cachelist[con.cid].vcount;k++) {
				
				if (cachelist[con.cid].vlist[k] == reqlist[i].vid) {
					if (l > con.latency) {
						l = con.latency;
					}
				}
			}	
		}
		
		sc += (endpointlist[reqlist[i].endid].latency - l) * reqlist[i].nreq;
		totnr += reqlist[i].nreq;
	}

	return (sc * 1000.) / totnr ;
}



void output_solution(T_PARAMS g_params,T_CACHE * cachelist) {
	FILE * f = fopen("result.txt","w");

	int count = 0;
	for (int i = 0;i < g_params.c;i++) {
		if (cachelist[i].vcount > 0) count++;
	}

	fprintf(f,"%d\n",count);

	for (int i = 0;i < g_params.c;i++) {
		if (cachelist[i].vcount > 0) {
			fprintf(f,"%d",i);
			for (int j = 0;j < cachelist[i].vcount;j++) {
				fprintf(f," %d",cachelist[i].vlist[j]);
			}
			fprintf(f,"\n");
		}
	}

	fclose(f);
}


int main(int argc, char **argv) {
	FILE * f = fopen(argv[1],"r");
	
	fscanf(f,"%d %d %d %d %d\n",&g_params.v,&g_params.e,&g_params.r,&g_params.c,&g_params.x);

	vsize = (int*)malloc(sizeof(int)*g_params.v);
	
	for (int i = 0;i < g_params.v;i++) {
		vsize[i] = read_int(f);	
		printf("video %d : %d\n",i,vsize[i]);
	}

	T_ENDPOINT * endpointlist = (T_ENDPOINT*)malloc(sizeof(T_ENDPOINT)*g_params.e);

	for (int i = 0;i < g_params.e;i++) {
		fscanf(f,"%d %d\n",&endpointlist[i].latency,&endpointlist[i].cache_srv);
		endpointlist[i].conlist = (T_CONNECTION*)malloc(sizeof(T_CONNECTION)*endpointlist[i].cache_srv);
		for (int j = 0;j < endpointlist[i].cache_srv;j++) {
			fscanf(f,"%d %d\n",&endpointlist[i].conlist[j].cid,&endpointlist[i].conlist[j].latency);
		}
	}

	T_REQUEST * reqlist = (T_REQUEST*)malloc(sizeof(T_REQUEST)*g_params.r);

	for (int i = 0;i < g_params.r;i++) {
		fscanf(f,"%d %d %d\n",&reqlist[i].vid,&reqlist[i].endid,&reqlist[i].nreq);
		reqlist[i].vsize = vsize[reqlist[i].vid];
	}
	fclose(f);

	for (int i = 0;i < g_params.v;i++) {
		//printf("video %d : %d\n",i,vsize[i]);
	}

	for (int i = 0;i < g_params.e;i++) {
		//printf("endpoint %d : latency : %d\n",i, endpointlist[i].latency);
	}

	T_CACHE * cachelist = init_cache(g_params);

	//solver(g_params,reqlist,endpointlist,cachelist,vsize,&compare);
	solverV(reqlist,endpointlist,cachelist,vsize,&compare);

	printf("SOLVED\n");

	printf("Score: %d\n",score_simple(reqlist,endpointlist,cachelist));
	output_solution(g_params,cachelist);
	return 0;
}