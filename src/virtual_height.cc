#include <octave/oct.h>

float integrate_profile(float f,         // [Hz]
                        int n,           // length of h,ne
                        const float* h,  // [in whatever length unit is provided]
                        const float* ne, // [m^{-3}]
                        float& real_height)
{
  if (!n || !h || !ne || f<0)
    return -1.0f;

  const float k   = 80.616f;         //  Hz^2 / m^{-3}
  const float fm2 = std::pow(f, -2); //  1/f^2

  // for integrating 1/sqrt(f(x)) we perform the analytic integration
  // of the linear interpolation of f(x) in [a b], i.e.
  //    int_a^b 1/sqrt(f(x)) = 2*(b-a) / (sqrt(f(a))+sqrt(f(b)))
  bool found_reflection = false;
  float vh_group = h[0];
  real_height = -1.0f;
  for (int i=0; i<n-1 && !found_reflection; ++i) {
#if 1
    const float mu2[2] = {
      1.0f - k*ne[i  ]*fm2,
      1.0f - k*ne[i+1]*fm2
    };
#else
    const float X[2] = {
      k*ne[i  ]*fm2,
      k*ne[i+1]*fm2
    };
    const float YT = 0.1*1.4e6/f;
    const float YL = 0.5*1.4e6/f;
    const float mu2[2] = {
      1.0f - 2*X[0]*(1-X[0])/(2*X[0]*(1-X[0]) - YT*YT + std::sqrt(YT*YT*YT*YT + 4*(1-X[0])*(1-X[0])*YL*YL)),
      1.0f - 2*X[1]*(1-X[1])/(2*X[1]*(1-X[1]) - YT*YT + std::sqrt(YT*YT*YT*YT + 4*(1-X[1])*(1-X[1])*YL*YL)),
    };
#endif
    if (mu2[0] > 0 && mu2[1] <= 0) {
      found_reflection = true;
      // here dh is the height difference to the reflection point
      const float dh = (h[i+1]-h[i]) * mu2[0] / (mu2[0]-mu2[1]);
      vh_group += 2*dh / std::sqrt(mu2[0]);
      real_height = h[i]+dh;
    } else {
      const float dh = h[i+1]-h[i];
      vh_group += 2*dh / (std::sqrt(mu2[1]) + std::sqrt(mu2[0]));
    }
  }
  return found_reflection ? vh_group : -1.0f;
}

DEFUN_DLD (virtual_height,
           args,
           ,
           "vh=virtual_height(h,ne,f);")
{
  const int n = args.length();
  if (n != 3)
    print_usage();

  const FloatNDArray h  = args(0).float_array_value();
  const FloatNDArray ne = args(1).float_array_value();
  const FloatNDArray f  = args(2).float_array_value();

  if (h.dims() != ne.dims())
    print_usage();

  FloatNDArray vh (f.dims(), -1.0f);
  FloatNDArray vhp(f.dims(), -1.0f);

  const float *pf  =   f.fortran_vec();
  float      *pvh  =  vh.fortran_vec();
  float      *pvhp = vhp.fortran_vec();
  for (int i=0,n=f.numel(),m=h.numel(); i<n; ++i) {
    pvh[i] = integrate_profile(pf[i], m, h.fortran_vec(), ne.fortran_vec(), pvhp[i]);
    if (pvh[i] < 0)
      break;
  }
  octave_value_list retval;
  retval(1) = vhp;
  retval(0) = vh;
  return retval;
}
