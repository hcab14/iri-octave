## -*- octave -*-

function example
  iri_config.yyyy     = 2016;
  iri_config.mmdd     = 1208;
  iri_config.dhour    = 24+17.5; ## UT
  iri_config.jf       = [1 0 0 0 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1 0 1 1 1 1 0 0 0 1 1 0 1 0 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1]; ## see src/irisub.for
  iri_config.jmag     = false;
  iri_config.data_dir = 'data'; ## this is optional, default is 'data'

  ll      = [53 -2];   ## lat, lon
  height  = [0 998 2]; ## heibeg, heiend, heistp
  oarr    = zeros(1, 100, 'single');

  [outf, oarr] = iri(iri_config, ll, height, oarr);

  plot(height(1):height(3):height(2), outf(1,:));
  ylim([0 1.8e11])
  xlabel 'height (km)'
  ylabel 'Ne (m^{-3})'
  title '12/8/2016 17:30Z (53N,2W) IRI2016'
endfunction
