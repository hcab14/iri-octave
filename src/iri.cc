#include <octave/oct.h>
#include <octave/f77-fcn.h>

#include "cwd.hpp"

extern"C" {
  F77_RET_T F77_FUNC(iri_sub, IRI_SUB)
    (const F77_LOGICAL *jf,      // logical jf(50)
     const F77_LOGICAL &jmag,    //
     const F77_REAL    &alati,   //
     const F77_REAL    &along,   //
     const F77_INT     &iyyyy,   //
     const F77_INT     &mmdd,    //
     const F77_REAL    &dhour,   //
     const F77_REAL    &heibgeg, //
     const F77_REAL    &heiend,  //
     const F77_REAL    &heistp,  //
     F77_REAL          *outf,    // OUTF(1:20,1:1000)
     F77_REAL          *oarr     // OARR(1,100)
     );
  F77_RET_T F77_FUNC(read_ig_rz, READ_IG_RZ)();
  F77_RET_T F77_FUNC(readapf107, READAPF107)();
}

octave_value getfield(const octave_scalar_map& map, const char* key, octave_value default_value) {
  return map.contains(key) ?  map.getfield(key) : default_value;
}
octave_value getfield(const octave_scalar_map& map, const char* key) {
  if (!map.contains(key))
    error("key '%s' does not exist", key);
  return map.getfield(key);
}

DEFUN_DLD (iri,
           args,
           ,
           "[outf,oarr]=iri(config,ll,height,oarr);")
{
  octave_value_list retval;

  const int n = args.length();
  if (n < 3)
    print_usage();

  const octave_scalar_map config = args(0).scalar_map_value();
  const Array<F77_LOGICAL>    jf = getfield(config, "jf").array_value();
  const F77_LOGICAL         jmag = getfield(config, "jmag").bool_value();
  const F77_INT             yyyy = getfield(config, "yyyy").int_value();
  const F77_INT             mmdd = getfield(config, "mmdd").int_value();
  const F77_REAL           dhour = getfield(config, "dhour").double_value();
  const std::string     data_dir = getfield(config, "data_dir", "data").string_value();
  const NDArray               ll = args(1).array_value();
  const NDArray              hei = args(2).array_value();
  const F77_REAL alati(ll(0)), along(ll(1));
  const F77_REAL heibeg(hei(0)), heiend(hei(1)), heistp(hei(2));

  Array<F77_REAL> outf(dim_vector(20,1000), -1.0f);
  Array<F77_REAL> oarr(n>3 ? args(3).float_array_value() : FloatNDArray(dim_vector(1,100), -1.0f));
  oarr.resize2(1,100, -1.0f);

  octave::unwind_protect frame;
  frame.add(new octave::action_container::delete_ptr_elem<cwd>(new cwd(data_dir.c_str())));

  static bool is_initialized = false;
  if (not is_initialized) {
    F77_XFCN (read_ig_rz, READ_IG_RZ, ());
    F77_XFCN (readapf107, READAPF107, ());
    is_initialized = true;
  }

  F77_XFCN (iri_sub, IRI_SUB,
            (jf.fortran_vec(),
             jmag, alati, along,
             yyyy, mmdd, dhour,
             heibeg, heiend, heistp,
             outf.fortran_vec(),
             oarr.fortran_vec()));

  outf.resize(dim_vector(20, int(1.5+(heiend-heibeg)/heistp)));

  retval(1) = oarr;
  retval(0) = outf;
  return retval;
}
