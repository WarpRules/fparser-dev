<?php

# Complex numbers are [real, imag]

$do_complex = true;
$failed     = false;
function fail_if($cond)
{
  global $failed, $do_complex;
  if($cond)
    $failed=true;
  return $cond;
}

function is_bad($a)
{
  $s = (string)$a;
  return !is_finite($a) || abs($a) >= 1e12; // tan(acos(0)) generates approx. 1E+12
}
function fp_ret($f, $r, $a0, $a1=null)
{
  #print_r(debug_backtrace());
  global $do_complex;
  if(!$do_complex)
  {
    if(is_array($r)) fail_if($r[1] != 0 || $a0[1] != 0);
    if(isset($a1)) fail_if($a1[1] != 0);
  }
  if(is_array($r))
  {
    // Add a failing condition for values that are dangerously close to 0 but not exactly 0
    fail_if($r[0] != 0 && $r[1] != 0 && (abs($r[0]) < 1e-12 || abs($r[1]) < 1e-12));
  }

  if(is_array($a0))
  {
    fail_if(is_bad($r[0]) || is_bad($r[1]));
    fail_if(is_bad($a0[0]) || is_bad($a0[1]));
    if(isset($a1))
      fail_if(is_bad($a1[0]) || is_bad($a1[1]));
  }
  else
  {
    fail_if(is_bad($r));
    fail_if(is_bad($a0));
    if(isset($a1))
      fail_if(is_bad($a1));
  }
  /*if(isset($a1))
    printf("%s(%s,%s) = %s\n", $f, json_encode($a0), json_encode($a1), json_encode($r));
  else
    printf("%s(%s) = %s\n", $f, json_encode($a0), json_encode($r));*/
  return $r;
}

/*
These functions are sensitive to differences between -0 and +0:
  arg() on both real and imag
*/

function fp_real($a)
{
  global $do_complex; fail_if(!$do_complex);
  return fp_ret(__FUNCTION__, [$a[0], 0], $a);
}
function fp_imag($a)
{
  global $do_complex; fail_if(!$do_complex);
  return fp_ret(__FUNCTION__, [$a[1], 0], $a);
}
function fp_arg($a)
{
  global $do_complex; fail_if(!$do_complex);
  // if real < 0 and imag = 0, then result is pi * sign(imag)
  // If real = 0 and imag = 0, then result can be ±pi/2 or ±0 depending on sign
  if(fail_if($a[0] <= 0  && abs($a[1]) < 1e-20)) return [0,0];
  return fp_ret(__FUNCTION__, [atan2($a[1], $a[0]), 0], $a);
}
function fp_conj($a)
{
  global $do_complex; fail_if(!$do_complex);
  return fp_ret(__FUNCTION__, [$a[0], -$a[1]], $a);
}
function fp_polar($a,$b)
{
  global $do_complex; fail_if(!$do_complex);
  return fp_ret(__FUNCTION__, [$a[0]*cos($b[0]), $a[0]*sin($b[0])], $a,$b);
}

function fp_add($a,$b) { return fp_ret(__FUNCTION__, [$a[0]+$b[0], $a[1]+$b[1]], $a,$b); }
function fp_sub($a,$b) { return fp_ret(__FUNCTION__, [$a[0]-$b[0], $a[1]-$b[1]], $a,$b); }
function fp_mul($a,$b) { return fp_ret(__FUNCTION__, [$a[0]*$b[0] - $a[1]*$b[1],
                                 $a[1]*$b[0] + $a[0]*$b[1]], $a,$b); }
function fp_div($a,$b) { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [$a[0]*zerocheck_real($b[0]),0], $a,$b);
                         return fp_ret(__FUNCTION__, [($a[0]*$b[0]+$a[1]*$b[1])*zerocheck_real($a[0]*$a[0]+$b[1]*$b[1]),
                                 ($a[1]*$b[0]-$a[0]*$b[1])*zerocheck_real($b[0]*$b[0]+$b[1]*$b[1]) ], $a,$b); }

function fp_floor($a) { return fp_ret(__FUNCTION__, [floor($a[0]), floor($a[1])], $a); }
function fp_ceil($a)  { return fp_ret(__FUNCTION__, [ceil($a[0]), ceil($a[1])], $a); }

function fp_int($a)   { return fp_ret(__FUNCTION__, [$a[0]<0?ceil($a[0]-0.5) :  floor($a[0]+0.5),
                                $a[1]<0?ceil($a[1]-0.5) :  floor($a[1]+0.5)], $a); }
function fp_trunc($a) { return fp_ret(__FUNCTION__, [$a[0]<0?ceil($a[0]):floor($a[0]),
                                $a[1]<0?ceil($a[1]):floor($a[1])], $a); }
function fp_norm($a)  { return fp_ret(__FUNCTION__, [$a[0]*$a[0] + $a[1]*$a[1], 0], $a); }

function fp_log($a)   { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [log($a[0]),0], $a);
                        if(fail_if(fp_abs($a)[0] < 1e-30)) return [0,0];
                        return fp_ret(__FUNCTION__, [log(fp_abs($a)[0]), fp_arg($a)[0]], $a); }
function fp_log2($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [log($a[0])/log(2),0], $a);
                        return fp_ret(__FUNCTION__, fp_mul(fp_log($a), [1/log(2),  0]), $a); }
function fp_log10($a) { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [log($a[0])/log(10),0], $a);
                        return fp_ret(__FUNCTION__, fp_mul(fp_log($a), [1/log(10), 0]), $a); }

function fp_pow($a,$b) { return fp_ret(__FUNCTION__, fp_exp(fp_mul($b, fp_log($a))), $a,$b); }
function fp_exp2($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [pow(2.0, $a[0]),0], $a);
                        return fp_ret(__FUNCTION__, fp_pow([2.0, 0], $a), $a); }
function fp_cbrt($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [pow($a[0], 1/3),0], $a);
                        return fp_ret(__FUNCTION__, fp_pow($a, [1/3.0,0]), $a); }
function fp_sqrt($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [sqrt($a[0]),0], $a);
                        return fp_ret(__FUNCTION__, fp_pow($a, [1/2.0,0]), $a); }

function fp_abs($a)   { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [abs($a[0]),0], $a);
                        return fp_ret(__FUNCTION__, [sqrt(fp_norm($a)[0]), 0], $a); }
function fp_sin($a)   { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [sin($a[0]),0], $a);
                        return fp_ret(__FUNCTION__, [sin($a[0])*cosh($a[1]),
                                cos($a[0])*sinh($a[1])], $a); }
function fp_cos($a)   { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [cos($a[0]),0], $a);
                        return fp_ret(__FUNCTION__, [cos($a[0])*cosh($a[1]),
                               -sin($a[0])*sinh($a[1])], $a); }
function fp_sinh($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [sinh($a[0]),0], $a);
                        return fp_ret(__FUNCTION__, [sinh($a[0])*cos($a[1]),
                                cosh($a[0])*sin($a[1])], $a); }
function fp_cosh($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [cosh($a[0]),0], $a);
                        return fp_ret(__FUNCTION__, [cosh($a[0])*cos($a[1]),
                                sinh($a[0])*sin($a[1])], $a); }
function fp_tan($a)   { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [tan($a[0]),0], $a);
                        return fp_ret(__FUNCTION__, fp_div(fp_sin($a), fp_cos($a)), $a); }
function fp_tanh($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [tanh($a[0]),0], $a);
                        return fp_ret(__FUNCTION__, fp_div(fp_sinh($a), fp_cosh($a)), $a); }
function fp_exp($a)   { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [exp($a[0]),0], $a);
                        $e = exp($a[0]); return fp_ret(__FUNCTION__, [$e*cos($a[1]), $e*sin($a[1])], $a); }

function fp_acosh($a) { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [acosh($a[0]),0], $a);
                        // log(x + sqrt(x^2 - 1))
                        return fp_ret(__FUNCTION__, fp_log(fp_add($a, fp_sqrt(fp_add(fp_mul($a,$a), [-1,0])))), $a); }
function fp_asinh($a) { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [asinh($a[0]),0], $a);
                        // log(x + sqrt(x^2 + 1))
                        return fp_ret(__FUNCTION__, fp_log(fp_add($a, fp_sqrt(fp_add(fp_mul($a,$a), [ 1,0])))), $a); }
function fp_acos($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [acos($a[0]),0], $a);
                        // -i * log(x + i * sqrt(1 - x^2))
                        return fp_ret(__FUNCTION__, fp_mul([0,-1],
                                      fp_log(fp_add($a, fp_mul([0,1], fp_sqrt(fp_add([1,0], fp_mul(fp_mul($a,$a),[-1,0]))))))), $a); }
function fp_asin($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [asin($a[0]),0], $a);
                        // -i * log(x*i + sqrt(1 - x^2))
                        return fp_ret(__FUNCTION__, fp_mul([0,-1],
                                      fp_log(fp_add(fp_mul([0,1], $a), fp_sqrt(fp_add([1,0], fp_mul(fp_mul($a,$a),[-1,0])))))), $a); }
function fp_atanh($a) { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [atanh($a[0]),0], $a);
                        // log( (1+x) / (1-x) )
                        if(fail_if(abs($a[0])==1)) return [0,0];
                        return fp_ret(__FUNCTION__, fp_log(fp_div(fp_add([1,0], $a),
                                                                  fp_add([1,0], [-$a[0],-$a[1]]))), $a); }
function fp_atan($a)  { global $do_complex; if(!$do_complex) return fp_ret(__FUNCTION__, [atan($a[0]),0], $a);
                        // -0.5i * log( (1+i*x) / (1-i*x) )
                        // Division by zero if x == ±i
                        return fp_ret(__FUNCTION__, fp_mul([0,-0.5], fp_log(fp_div(fp_add([1,0], fp_mul($a,[0, 1])),
                                                                                   fp_add([1,0], fp_mul($a,[0,-1]))))), $a);
                      }
function fp_atan2($a,$b)
{
  global $do_complex;
  if(!$do_complex) return fp_ret(__FUNCTION__, [atan2($a[0], $b[0]),0], $a,$b);
  if(fp_abs($a)[0] < 1e-16) return fp_ret(__FUNCTION__, fp_arg($b), $a,$b);
  if(fp_abs($b)[0] < 1e-16) return fp_ret(__FUNCTION__, [atan2(-1,0),0], $a,$b); // -pi/2
  // 2 * atan(y / (hypot(x,y) + x))
  return fp_ret(__FUNCTION__, fp_mul([2,0], fp_atan(fp_div($a, fp_add($b, fp_hypot($a,$b))))), $a,$b);
}
function fp_hypot($a,$b)
{
  return fp_ret(__FUNCTION__, fp_sqrt(fp_add(fp_mul($a,$a), fp_mul($b,$b))), $a,$b);
}

function zerocheck_real($a)
{
  $zero = abs($a) < 1e-16;
  fail_if($zero);
  if($zero) return fp_ret(__FUNCTION__, 0, $a);
  return fp_ret(__FUNCTION__, 1/$a, $a);
}
function zerocheck($a)
{
  return fp_ret(__FUNCTION__, fp_div([1,0], $a), $a);
}

function fp_equal($a, $b)  { return fp_ret(__FUNCTION__, [ ($a[0]==$b[0] && $a[1]==$b[1]) ? 1 : 0, 0 ], $a,$b); }
function fp_nequal($a, $b) { return fp_ret(__FUNCTION__, [ ($a[0]!=$b[0] || $a[1]!=$b[1]) ? 1 : 0, 0 ], $a,$b); }

function fp_less($a, $b)
{
  return fp_ret(__FUNCTION__, [ ($a[0]<$b[0]) ? 1 : 0, 0 ], $a,$b);
}
