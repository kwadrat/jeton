TARGET = jeton
CXX = c++
CC = cc
CMF = #-s -O6
LDFLAGS = $(CMF) #-g
CCFLAGS = $(CMF) -I include -MD
BDIR = build
all: $(BDIR) $(BDIR)/$(TARGET)
SDIR = src
VPATH = $(SDIR)
$(BDIR)/$(TARGET):
	$(CXX) $(LDFLAGS) -o $(BDIR)/$(TARGET) $(wildcard $(BDIR)/*.o) -lpthread
$(BDIR)/$(TARGET): $(BDIR)/helpx.o
$(BDIR)/$(TARGET): $(BDIR)/logx.o
$(BDIR)/$(TARGET): $(BDIR)/masterx.o
$(BDIR)/$(TARGET): $(BDIR)/procx.o
$(BDIR)/$(TARGET): $(BDIR)/scmpx.o
$(BDIR)/$(TARGET): $(BDIR)/fsi.o
$(BDIR)/$(TARGET): $(BDIR)/fso.o
$(BDIR)/$(TARGET): $(BDIR)/fkopier.o
$(BDIR)/$(TARGET): $(BDIR)/fy.o
$(BDIR)/$(TARGET): $(BDIR)/fsmar.o
$(BDIR)/$(TARGET): $(BDIR)/semx.o
$(BDIR)/$(TARGET): $(BDIR)/fcat.o
$(BDIR)/$(TARGET): $(BDIR)/rvrx.o
$(BDIR)/$(TARGET): $(BDIR)/ffi.o
$(BDIR)/$(TARGET): $(BDIR)/ffo.o
$(BDIR)/$(TARGET): $(BDIR)/ftoc.o
$(BDIR)/$(TARGET): $(BDIR)/ftis.o
$(BDIR)/$(TARGET): $(BDIR)/ftos.o
$(BDIR)/$(TARGET): $(BDIR)/ftic.o
$(BDIR)/$(TARGET): $(BDIR)/facptr.o
$(BDIR)/$(TARGET): $(BDIR)/fgi.o
$(BDIR)/$(TARGET): $(BDIR)/fmd5.o
$(BDIR)/$(TARGET): $(BDIR)/$(TARGET).o
%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $(BDIR)/$(@)
$(BDIR)/%.o: %.cpp
	$(CXX) $(CCFLAGS) -c $< -o $(@)
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
