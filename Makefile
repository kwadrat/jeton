OBJS= helpx.o logx.o masterx.o procx.o scmpx.o fsi.o fso.o fkopier.o fy.o fsmar.o semx.o fcat.o rvrx.o ffi.o ffo.o ftoc.o ftis.o ftos.o ftic.o facptr.o fgi.o fmd5.o
CXX= c++
CC= cc
CMF= #-s -O6
LDFLAGS= $(CMF) #-g
CCFLAGS= $(CMF) #-g
all: 	jeton
jeton: $(OBJS) jeton.cpp
	$(CXX) $(LDFLAGS) -o jeton jeton.cpp $(OBJS) -lpthread
%.o: %.c
	$(CC) $(CCFLAGS) -c $<
%.o: %.cpp
	$(CXX) $(CCFLAGS) -c $<
jeton.o: typy.h helpx.h logx.h masterx.h
masterx.o: masterx.h typy.h procx.h
procx.o: procx.h typy.h
helpx.o: helpx.h scmpx.h
fsi.o: fsi.h procx.h typy.h
fso.o: fso.h procx.h typy.h
fmd5.o: fmd5.h procx.h typy.h
fgi.o: fgi.h procx.h typy.h
facptr.o: facptr.h procx.h typy.h rvrx.h
ffi.o: ffi.h procx.h typy.h rvrx.h
ffo.o: ffo.h procx.h typy.h rvrx.h
ftoc.o: ftoc.h procx.h typy.h rvrx.h
ftis.o: ftis.h procx.h typy.h rvrx.h
ftos.o: ftos.h procx.h typy.h rvrx.h
ftic.o: ftic.h procx.h typy.h rvrx.h
fkopier.o: fkopier.h procx.h typy.h
fsmar.o: fsmar.h procx.h semx.h typy.h
fy.o: fy.h procx.h typy.h
fcat.o: fcat.h procx.h typy.h
rvrx.o: rvrx.h typy.h
logx.o: logx.h masterx.h procx.h fsi.h fso.h fkopier.h fy.h fsmar.h fcat.h ffi.h ffo.h ftoc.h typy.h ftis.h ftos.h ftic.h facptr.h fmd5.h
semx.o: semx.h semx.h typy.h rvrx.h
clean:
	rm *.o jeton
backup:
	tar czvf /tmp/kopia_`date +%Y.%m.%d-%H.%M`.tgz *
win: ../j4win/Readme.txt
../j4win/Readme.txt: jeton
	SkrWin
# Edycja pliku Makefile
mk:
	vi -u mk.txt Makefile
