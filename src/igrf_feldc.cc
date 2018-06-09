#include <iostream>
#include <vector>

#include <octave/oct.h>
#include <octave/ov-struct.h>
#include <octave/f77-fcn.h>

#include "cwd.hpp"

//SUBROUTINE MODA(IN,IYEAR,MONTH,IDAY,IDOY,NRDAYMO)
extern "C" {
  F77_RET_T F77_FUNC(feldcof, IGRF_FELDCOF) (const F77_REAL &year);
  F77_RET_T F77_FUNC(feldc,   IGRF_FELDC)   (const F77_REAL *v, F77_REAL *b);
  F77_RET_T F77_FUNC(moda,    MODA)         (const F77_INT&, F77_INT&, F77_INT&, F77_INT&, F77_INT&, F77_INT&);
}

octave_value getfield(const octave_scalar_map& map, const char* key) {
  if (!map.contains(key))
    error("key '%s' does not exist", key);
  return map.getfield(key);
}
octave_value getfield(const octave_scalar_map& map, const char* key, octave_value default_value) {
  return map.contains(key) ?  map.getfield(key) : default_value;
}

DEFUN_DLD (igrf_feldc,
           args,
           nargsout,
           "b=igrf_feldc(config, x); # config -> see iri.cc;"
           // " x ... cartesian coordinates in Earth's radii (6371.2km)"
           // " b ... magnetic field strength in Gauss"
           )
{
  if (args.length() != 2)
    print_usage();

  const octave_scalar_map config = args(0).scalar_map_value();
  F77_INT                   yyyy = getfield(config, "yyyy").int_value();
  const F77_INT             mmdd = getfield(config, "mmdd").int_value();
  const std::string     data_dir = getfield(config, "data_dir", "data").string_value();
  const Array<F77_REAL>        v = args(1).array_value();

  octave::unwind_protect frame;
  frame.add(new octave::action_container::delete_ptr_elem<cwd>(new cwd(data_dir.c_str())));

  const dim_vector dv = v.dims();
  if (dv(0) != 3)
    print_usage();

  F77_INT in=0, month=mmdd/100, iday=mmdd%100, idoy=0, nrdaymo=0;
  F77_XFCN (moda, MODA, (in, yyyy, month, iday, idoy, nrdaymo));

  static F77_REAL year_old = -1.0f;
  const  F77_REAL year = yyyy + idoy/(365.0f + ((yyyy%4)==0));
  if (year != year_old) {
    F77_XFCN (feldcof, IGRF_FELDCOF, (year));
    year_old = year;
  }

  Array<F77_REAL> b(dv);
  for (int i=0, n=dv(1); i<n; ++i)
    F77_XFCN (feldc, IGRF_FELDC, (v.fortran_vec()+3*i, b.fortran_vec()+3*i));

  return octave_value(b);
}
