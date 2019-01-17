TARGET = jeton
OBJS = helpx.o logx.o masterx.o procx.o scmpx.o fsi.o fso.o fkopier.o fy.o fsmar.o semx.o fcat.o rvrx.o ffi.o ffo.o ftoc.o ftis.o ftos.o ftic.o facptr.o fgi.o fmd5.o $(TARGET).o
CXX = c++
CC = cc
CMF = #-s -O6
LDFLAGS = $(CMF) #-g
CCFLAGS = $(CMF) -I /home/kwadrat/jgd/jeton/include -MD #-g
BDIR = build
all: $(BDIR) $(BDIR)/$(TARGET)
SDIR = src
VPATH = $(SDIR)
$(BDIR)/$(TARGET):
	cd $(BDIR); $(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS) -lpthread
$(BDIR)/$(TARGET): helpx.o
$(BDIR)/$(TARGET): helpx.o
$(BDIR)/$(TARGET): logx.o
$(BDIR)/$(TARGET): masterx.o
$(BDIR)/$(TARGET): procx.o
$(BDIR)/$(TARGET): scmpx.o
$(BDIR)/$(TARGET): fsi.o
$(BDIR)/$(TARGET): fso.o
$(BDIR)/$(TARGET): fkopier.o
$(BDIR)/$(TARGET): fy.o
$(BDIR)/$(TARGET): fsmar.o
$(BDIR)/$(TARGET): semx.o
$(BDIR)/$(TARGET): fcat.o
$(BDIR)/$(TARGET): rvrx.o
$(BDIR)/$(TARGET): ffi.o
$(BDIR)/$(TARGET): ffo.o
$(BDIR)/$(TARGET): ftoc.o
$(BDIR)/$(TARGET): ftis.o
$(BDIR)/$(TARGET): ftos.o
$(BDIR)/$(TARGET): ftic.o
$(BDIR)/$(TARGET): facptr.o
$(BDIR)/$(TARGET): fgi.o
$(BDIR)/$(TARGET): fmd5.o
$(BDIR)/$(TARGET): $(TARGET).o
%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $(BDIR)/$(@)
%.o: %.cpp
	$(CXX) $(CCFLAGS) -c $< -o $(BDIR)/$(@)
clean:
	rm $(BDIR)/*.d $(BDIR)/*.o $(BDIR)/$(TARGET)
backup:
	tar czvf /tmp/kopia_`date +%Y.%m.%d-%H.%M`.tgz *
win: ../j4win/Readme.txt
../j4win/Readme.txt: $(TARGET)
	SkrWin
# Edycja pliku Makefile
mk:
	vi -u mk.txt Makefile
${BDIR}:
	mkdir ${BDIR}
ifeq ($(MAKECMDGOALS), all)
    include ${wildcard $(BDIR)/*.d}
endif
