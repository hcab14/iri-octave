## -*- octave -*-

function iri_test
  iri_config.yyyy     = 2016;
  iri_config.mmdd     = 1208;
  iri_config.dhour    = 24+12.5; ## UT
  iri_config.jf       = [1 0 0 0 1 0 1 1 1 1 ... #  1-10
                         1 1 1 1 1 1 1 1 1 1 ... # 11-20
                         0 1 0 1 1 1 1 0 0 0 ... # 21-30
                         1 1 0 1 0 1 1 1 0 1 ... # 31-40
                         1 1 1 1 1 1 1 1 1 1 ... # 41-50
                         1]; ## see src/irisub.for
  iri_config.jmag     = false;
  iri_config.data_dir = 'data/'; ## this is optional, default is 'data'


#  phi=[0:10:90]/180*pi;
#  x=[cos(phi);
#     zeros(size(phi));
#     sin(phi)];

#  ll0 = [52.239519,  6.850604] ## Twente
#  ll1 = [34.6210492,32.9459712] ## Cyprus RAF
#  u0    = ll2xyz(ll0);
#  u1    = ll2xyz(ll1);
#  u  = 0.5*(u0+u1);
#  u /= sqrt(dot(u,u));
#  u *= (6371+200)/6371;

#  ul = u1-u0;
#  ul /= sqrt(dot(ul,ul));
#  b=igrf_feldc(iri_config, u)
#  [dot(u,b) dot(ul,b)]
#  return;
  ll      = [53 -2];   ## lat, lon
  u = ll2xyz(ll)
  b = igrf_feldc(iri_config, u)
  height  = [0 998 2]; ## heibeg, heiend, heistp
  oarr    = zeros(1, 100, 'single');

  _tt_ = 1
  [outf, oarr] = iri(iri_config, ll, height, oarr);
  _tt_ = 2

  plot(height(1):height(3):height(2), outf(1,:));
  ylim([0 1.1*max(outf(1,:))])
  xlabel 'height (km)'
  ylabel 'Ne (m^{-3})'
  title '12/8/2016 12:30Z (53N,2W) IRI2016'

  figure 2
  f  = 1e6*[1:.01:20];
  vh = virtual_height(height(1):height(3):height(2), outf(1,:), f);
  f(vh<0)=[];
  vh(vh<0)=[];
  plot(f/1e6, vh, '-');
  xlabel 'frequency (MHz)'
  ylabel 'virtual vertical height (km)'
  title '12/8/2016 12:30Z (53N,2W) IRI2016'
endfunction

function xyz=ll2xyz(ll)
  llr = deg2rad(ll);
  xyz = [cos(llr(2))*cos(llr(1))
         sin(llr(2))*cos(llr(1))
         sin(llr(1))];
endfunction
