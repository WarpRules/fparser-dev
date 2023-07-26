<?php

$failed = false;
function fail_if($cond)
{
  global $failed;
  if($cond)
    $failed=true;
}
function fp_floor($a) { return floor($a); }
function fp_ceil($a)  { return ceil($a); }
function fp_less($a,$b) { return ($a<$b) ? 1 : 0;}
function fp_int($a)   { return $a<0?ceil($a-0.5) :  floor($a+0.5); }
function fp_trunc($a) { return $a<0?ceil($a):floor($a); }
function fp_log($a)   { fail_if($a<=0); return log($a); }
function fp_log2($a)  { fail_if($a<=0); return log($a)/log(2); }
function fp_log10($a) { fail_if($a<=0); return log($a)/log(10); }
function fp_exp2($a)  { return pow(2.0, $a); }
function fp_abs($a)   { return abs($a); }
function fp_atan($a)  { return atan($a); }
function fp_cbrt($a)  { fail_if($a<0); return pow($a, 1/3.0); }
function fp_sqrt($a)  { fail_if($a<0); return sqrt($a); }
function fp_sin($a)   { return sin($a); }
function fp_cos($a)   { return cos($a); }
function fp_tan($a)
{
  $pi = atan2(0,-1);
  fail_if(abs(abs($a) - $pi/2) < 1e-20);
  return tan($a);
}
function fp_sinh($a)  { return sinh($a); }
function fp_cosh($a)  { return cosh($a); }
function fp_tanh($a)  { return tanh($a); }
function fp_exp($a)   { return exp($a); }
function fp_acosh($a) { fail_if($a<1);          return acosh($a); }
function fp_asinh($a) { return asinh($a); }
function fp_atanh($a) { fail_if($a<=-1 || $a>=1); return atanh($a); }
function fp_asin($a)  { fail_if($a<-1 || $a>1); return asin($a); }
function fp_acos($a)  { fail_if($a<-1 || $a>1); return acos($a); }
function zerocheck($a)
{
  $zero = abs($a) < 1e-16;
  fail_if($zero);
  if($zero) return 0;
  return 1/$a;
}
function fp_atan2($a,$b) { return atan2($a,$b); }
function fp_pow($a,$b)
{
  fail_if($a == 0 && $b < 0);
  return pow($a,$b);
}
function fp_hypot($a,$b)
{
  return hypot($a,$b);
}
function zp_div($a,$b)
{
  return $a * zerocheck($b);
}
