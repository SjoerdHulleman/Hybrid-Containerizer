#include <Rcpp.h>
using namespace Rcpp;
// [[Rcpp::export]]
NumericMatrix intPP_mixed(
       NumericVector   zmax,
       NumericMatrix     kd,
       NumericMatrix    par,
       NumericMatrix   alfa,
       NumericMatrix   eopt,
       NumericMatrix   pmax,
       NumericMatrix height) {
  double parz , 
         zt, zg, zz,
         total,
         f1, f2, f3,
         ps   ;
  static double xg[] = {0.04691008, 0.23076534,    0.5,   0.76923466, 0.95308992};
  static double wg[] = {0.1184634,  0.2393143, 0.2844444, 0.2393143,  0.1184634};
  int  j, it, is;
  int  nt = kd.nrow();
  int  ns = zmax.size();
  NumericMatrix pp(nt, ns);
  for (is = 0; is < ns; ++is) {
    for (it = 0; it < nt; ++it) {              
      zt  = zmax[is] + height(it,is);
      if (zt > 0.0 && (par(it, is) > 0)) {
        zg    = -log(0.001)/kd(it, is); 
        if (zg > zt) zg = zt;
        f1  = 1.0/(alfa(it,is) * pow(eopt(it,is), 2.0));
        f2  = 1.0/pmax(it,is) - 2.0/(alfa(it,is) * eopt(it,is));
        f3  = 1.0/alfa(it,is);
        total = 0.0;
        for (j = 0; j < 5; ++j) {              
          zz   = xg[j]*zg;
          parz = par(it,is) * exp (-kd(it,is) * zz);
          ps = parz/(f1 * pow(parz, 2.0) + f2 * parz + f3);          
          total += wg[j]*ps*zg;
        }
      } else total = 0.0;
      pp(it,is) = total;
    }
  }
  return pp;
}
// [[Rcpp::export]]
NumericMatrix rad_bot(
    NumericVector   zmax,
    NumericMatrix     kd,
    NumericMatrix    par,
    NumericMatrix height) {
  int  it, is;
  int  nt = kd.nrow();
  int  ns = zmax.size();
  double zt; 
  NumericMatrix rad(nt, ns);
  for (is = 0; is < ns; ++is) {
    for (it = 0; it < nt; ++it) {              
      zt  = zmax[is] + height(it,is);
      if (zt > 0.0) {
              rad(it,is) = par(it,is) * exp (-kd(it,is) * zt);
      } else  rad(it,is) = par(it,is)  ;  
    }
  }
  return rad;
}
// [[Rcpp::export]]
NumericMatrix intPP_exp(       
        NumericVector     zmax,
        NumericVector       kd,
        NumericVector     pMud,
        NumericMatrix      par,
        NumericMatrix     alfa,
        NumericMatrix     eopt,
        NumericMatrix     pmax){
  double parz , zt, zg, zz,
         total, alfa_z, pmax_z, eopt_z, pChl_z, 
         f1, f2, f3,
         ps   ;
  static double xg[] = {0.04691008, 0.23076534,    0.5,   0.76923466, 0.95308992};
  static double wg[] = {0.1184634,  0.2393143, 0.2844444, 0.2393143,  0.1184634};
  int  j, it, is;
  int  nt = par.nrow();
  int  ns = zmax.size();
  NumericMatrix pp(nt, ns);
  for (is = 0; is < ns; ++is) {
    for (it = 0; it < nt; ++it) {              
      zt  = zmax[is] ;
      if (zt > 0.0 && (par(it,is) > 0)) {
        zg    =  -log(0.001)/kd[is];
        if (zg > zt) zg = zt; 
        total = 0.0;
        for (j = 0; j < 5; ++j) {      
          zz    = xg[j]*zg;                 
          pChl_z  = (1-pMud[is]) + pMud[is]*2.0*exp(1.0)*exp(-2.*exp(1.0)*zz/zt);
          alfa_z = alfa(it, is) * pChl_z;  
          pmax_z = pmax(it, is) * pChl_z;
          eopt_z = eopt(it, is);
          f1  = 1.0/(alfa_z * pow(eopt_z, 2.0));
          f2  = 1.0/pmax_z - 2.0/(alfa_z * eopt_z);
          f3  = 1.0/alfa_z;
          parz = par(it,is) * exp (-kd[is] * zz); 
          ps = parz/(f1 * pow(parz, 2.0) + f2 * parz + f3);          
          total += wg[j]*ps*zg;
        }
      } else total = 0.0;
      pp(it,is) = total;
    }
  }
  return pp;
}
