NAME='stats_pusher_mongodb'

CFLAGS = ['-I/usr/include/mongo','-I/usr/local/include/mongo']
LDFLAGS = []
LIBS = ['-lmongoclient', '-lboost_thread','-lboost_filesystem']

GCC_LIST = ['plugin', 'stats_pusher_mongodb.cc']
