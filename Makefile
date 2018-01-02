INDICES   = $(addprefix data/,$(shell echo {apf107,ig_rz}.dat))
CCIR_URSI = $(addprefix data/,$(shell echo {ccir,ursi}{1{1,2,3,4,5,6,7,8,9},2{0,1,2}}.asc))
IRI_DATA  = $(addprefix data/,$(shell echo dgrf{19{45,{5,6,7,8,9}{0,5}},20{00,05,10}}.dat igrf2015{,s}.dat mcsat{1{1,2,3,4,5,6,7,8,9},2{0,1,2}}.dat))
IRI_CODE  = $(addprefix  src/,$(shell echo {cira,igrf,iri{dreg,flip,fun,sub,tec,test}}.for))

FC        = $(shell mkoctfile -p F77)
FFLAGS    = $(shell mkoctfile -p FFLAGS)
FLIBS     = $(shell mkoctfile -p FLIBS)

OBJS      = $(patsubst %.for, %.o, $(IRI_CODE))

all: $(INDICES) $(CCIR_URSI) $(IRI_DATA) $(OBJS) iri.oct

%.oct : %.cc $(OBJS)
	mkoctfile $< $(OBJS) -Wl,-no_compact_unwind $(FLIBS)

%.o : %.for
	 $(FC) $(FFLAGS) -c $< -o $@

$(INDICES):
	wget -N http://irimodel.org/indices/$(notdir $@) -O $@

$(CCIR_URSI): tar/00_ccir-ursi.tar
	tar xvf $< -O $(notdir $@) > $@
	touch $@

$(IRI_DATA) $(IRI_CODE): tar/00_iri2016.tar
	tar xvf $< -O $(notdir $@) > $@
	touch $@

tar/00_ccir-ursi.tar:
	cd tar && wget -N http://irimodel.org/COMMON_FILES/00_ccir-ursi.tar && cd ..

tar/00_iri2016.tar:
	cd tar && wget -N http://irimodel.org/IRI-2016/00_iri2016.tar && cd ..


clean:
	rm -f tar/00_{ccir-ursi,iri2016}.tar $(INDICES) $(CCIR_URSI) $(IRI_DATA) $(IRI_CODE) $(OBJS)
