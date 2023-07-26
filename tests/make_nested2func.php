<?php

require 'util/func_simulator.php';

/*$w = -2; $x = -1; $y = -2; $z = 0;
print_r(fp_atanh(fp_polar([$x,$y], [$z,$w])));
exit;*/

$outer = Array(
  ['s', 'sin','fp_sin'],
  ['c', 'cos','fp_cos'],
  ['t', 'tan','fp_tan'],
  ['as', 'asin','fp_asin'],
  ['ac', 'acos','fp_acos'],
  ['at', 'atan','fp_atan'],
  ['e', 'sec','1/fp_cos'],
  ['a', 'csc','1/fp_sin'],
  ['o', 'cot','1/fp_tan'],
  ['sh', 'sinh','fp_sinh'],
  ['ch', 'cosh','fp_cosh'],
  ['th', 'tanh','fp_tanh'],
  ['ash', 'asinh','fp_asinh'],
  ['ach', 'acosh','fp_acosh'],
  ['ath', 'atanh','fp_atanh'],
  ['ex', 'exp','fp_exp'],
  ['e2', 'exp2','fp_exp2'],
  ['l', 'log','fp_log'],
  ['l2', 'log2','fp_log2'],
  ['lt', 'log10','fp_log10'],
  ['sq', 'sqrt','fp_sqrt'],
  ['cb', 'cbrt','fp_cbrt'],
  ['i', 'imag', 'fp_imag'],
  ['r', 'real', 'fp_real'],
  ['j', 'conj', 'fp_conj'],
  ['g', 'arg', 'fp_arg'],
  ['b', 'abs', 'fp_abs'],
  ['z', '',    ''],
);
$inner = Array(
  ['a', 'at', 'atan(1a)','fp_atan(a+c)'],
  ['a', 'atp', 'atan(abs(1a))','fp_atan(fp_abs(a+c))'],
  ['a', 'atn', 'atan(-abs(1a))','fp_atan(-fp_abs(a+c))'],

  ['a', 's', 'sin(1a)','fp_sin(a+c)'],
  ['a', 'c', 'cos(1a)','fp_cos(a+c)'],
  ['a', 't', 'tan(1a)','fp_tan(a+c)'],
  ['a', 'as', 'asin(1a)','fp_asin(a+c)'],
  ['a', 'ac', 'acos(1a)','fp_acos(a+c)'],
  ['a', 'e', 'sec(1a)','1/fp_cos(a+c)'],
  ['a', 'a', 'csc(1a)','1/fp_sin(a+c)'],
  ['a', 'o', 'cot(1a)','1/fp_tan(a+c)'],
  ['a', 'sh', 'sinh(1a)','fp_sinh(a+c)'],
  ['a', 'ch', 'cosh(1a)','fp_cosh(a+c)'],
  ['a', 'th', 'tanh(1a)','fp_tanh(a+c)'],
  ['a', 'ash', 'asinh(1a)','fp_asinh(a+c)'],
  ['a', 'ach', 'acosh(1a)','fp_acosh(a+c)'],
  ['a', 'ath', 'atanh(1a)','fp_atanh(a+c)'],
  ['a', 'ex', 'exp(1a)','fp_exp(a+c)'],
  ['a', 'e2', 'exp2(1a)','fp_exp2(a+c)'],
  ['a', 'l', 'log(1a)','fp_log(a+c)'],
  ['a', 'l2', 'log2(1a)','fp_log2(a+c)'],
  ['a', 'lt', 'log10(1a)','fp_log10(a+c)'],
  ['a', 'sq', 'sqrt(1a)','fp_sqrt(a+c)'],
  ['a', 'cb', 'cbrt(1a)','fp_cbrt(a+c)'],
  ['a', 'i', 'imag(1a)', 'fp_imag(a+c)'],
  ['a', 'r', 'real(1a)', 'fp_real(a+c)'],
  ['a', 'j', 'conj(1a)', 'fp_conj(a+c)'],
  ['a', 'g', 'arg(1a)', 'fp_arg(a+c)'],
  ['a', 'b', 'abs(1a)', 'fp_abs(a+c)'],

  /*
  Don't do these not-well-defined functions:
    less, greater, lessOrEq, greaterOrEq
    ceil, floor, int, trunc
    min, max
  */

  ['a',   'h1x','hypot(1,1a)','fp_hypot(1,a+c)'],
  ['a',   'h2x','hypot(2,1a)','fp_hypot(2,a+c)'],
  ['a',   'hx1','hypot(1a,1)','fp_hypot(a+c,1)'],
  ['a',   'hx2','hypot(1a,2)','fp_hypot(a+c,2)'],
  ['a,b', 'h', 'hypot(1a,1b)','fp_hypot(a+c,b+d)'],

  ['a',   'ih1x','1/hypot(1,1a)','1/fp_hypot(1,a+c)'],
  ['a',   'ih2x','1/hypot(2,1a)','1/fp_hypot(2,a+c)'],
  ['a',   'ihx1','1/hypot(1a,1)','1/fp_hypot(a+c,1)'],
  ['a',   'ihx2','1/hypot(1a,2)','1/fp_hypot(a+c,2)'],
  ['a,b', 'ih', '1/hypot(1a,1b)','1/fp_hypot(a+c,b+d)'],

  ['a,b', 'a2','atan2(1a,1b)','fp_atan2(a+c,b+d)'],
  ['a,b', 'ia2','1/atan2(1a,1b)','1/fp_atan2(a+c,b+d)'],
  ['a,b', 'po','polar(1a,1b)','fp_polar(a+c,b+d)'],
  ['a,b', 'p', 'pow(1a,1b)',  'fp_pow(a+c,b+d)'],
  ['a,b', 'eq','(1a)=(1b)',  'fp_equal(a+c,b+d)'],
  ['a,b', 'ne','(1a)!=(1b)', 'fp_nequal(a+c,b+d)'],

  ['a,b', 'atl', 'atan(1a!=1b)','fp_atan(fp_nequal(a+c,b+d))'],
  ['a,b', 'atd', 'atan(1a/1b)','fp_atan((a+c)/(b+d))'],
  ['a,b', 'iatd', '1/atan(1a/1b)','1/fp_atan((a+c)/(b+d))'],

  ['a,b', 'sl', 'sin(1a!=1b)','fp_sin(fp_nequal(a+c,b+d))'],
  ['a,b', 'cl', 'cos(1a!=1b)','fp_cos(fp_nequal(a+c,b+d))'],
  ['a,b', 'tl', 'tan(1a!=1b)','fp_tan(fp_nequal(a+c,b+d))'],
  ['a,b', 'asl', 'asin(1a!=1b)','fp_asin(fp_nequal(a+c,b+d))'],
  ['a,b', 'acl', 'acos(1a!=1b)','fp_acos(fp_nequal(a+c,b+d))'],
  ['a,b', 'el', 'sec(1a!=1b)','1/fp_cos(fp_nequal(a+c,b+d))'],
  ['a,b', 'al', 'csc(1a!=1b)','1/fp_sin(fp_nequal(a+c,b+d))'],
  ['a,b', 'ol', 'cot(1a!=1b)','1/fp_tan(fp_nequal(a+c,b+d))'],
  ['a,b', 'shl', 'sinh(1a!=1b)','fp_sinh(fp_nequal(a+c,b+d))'],
  ['a,b', 'chl', 'cosh(1a!=1b)','fp_cosh(fp_nequal(a+c,b+d))'],
  ['a,b', 'thl', 'tanh(1a!=1b)','fp_tanh(fp_nequal(a+c,b+d))'],
  ['a,b', 'ashl', 'asinh(1a!=1b)','fp_asinh(fp_nequal(a+c,b+d))'],
  ['a,b', 'achl', 'acosh(1a!=1b)','fp_acosh(fp_nequal(a+c,b+d))'],
  ['a,b', 'athl', 'atanh(1a!=1b)','fp_atanh(fp_nequal(a+c,b+d))'],
  ['a,b', 'exl', 'exp(1a!=1b)','fp_exp(fp_nequal(a+c,b+d))'],
  ['a,b', 'e2l', 'exp2(1a!=1b)','fp_exp2(fp_nequal(a+c,b+d))'],
  ['a,b', 'll', 'log(1a!=1b)','fp_log(fp_nequal(a+c,b+d))'],
  ['a,b', 'l2l', 'log2(1a!=1b)','fp_log2(fp_nequal(a+c,b+d))'],
  ['a,b', 'ltl', 'log10(1a!=1b)','fp_log10(fp_nequal(a+c,b+d))'],
  ['a,b', 'sql', 'sqrt(1a!=1b)','fp_sqrt(fp_nequal(a+c,b+d))'],
  ['a,b', 'cbl', 'cbrt(1a!=1b)','fp_cbrt(fp_nequal(a+c,b+d))'],

);
// a := x + y*1i
// b := z + w*1i

function test($code, $const, $use_b)
{
  global $do_complex;

  $min=$const['A'];
  $max=$const['B'];
  $step=$const['S'];
  $res = str_replace('A', $min, $code);
  $res = str_replace('B', $max, $res);
  $res = str_replace('S', $step, $res);

  $eval = preg_replace('/.*C=/ms', '', $res);

  $c = $const['C'];
  $d = $const['C'];
  if($do_complex)
  {
    $eval = str_replace('((a+c)/(b+d))', '(fp_div([$x+'.$c.',$y],[$z+'.$d.',$w]))', $eval);
    $eval = str_replace('(a+c)',     '([$x+'.$c.',$y])',                $eval);
    $eval = str_replace('(b+d)',     '([$z+'.$d.',$w])',                $eval);
    $eval = str_replace('(a+c,b+d)', '([$x+'.$c.',$y],[$z+'.$d.',$w])', $eval);
    $eval = str_replace('(1,a+c)',   '([1,0],[$x+'.$c.',$y])',          $eval);
    $eval = str_replace('(2,a+c)',   '([2,0],[$x+'.$c.',$y])',          $eval);
    $eval = str_replace('(a+c,1)',   '([$x+'.$c.',$y],[1,0])',          $eval);
    $eval = str_replace('(a+c,2)',   '([$x+'.$c.',$y],[2,0])',          $eval);
  }
  else
  {
    $eval = str_replace('((a+c)/(b+d))', '(fp_div([$a+'.$c.',0],[$b+'.$d.',0]))', $eval);
    $eval = str_replace('(a+c)',     '([$a+'.$c.',0])',               $eval);
    $eval = str_replace('(b+d)',     '([$b+'.$d.',0])',               $eval);
    $eval = str_replace('(a+c,b+d)', '([$a+'.$c.',0],[$b+'.$d.',0])', $eval);
    $eval = str_replace('(1,a+c)',   '([1,0],[$a+'.$c.',0])',          $eval);
    $eval = str_replace('(2,a+c)',   '([2,0],[$a+'.$c.',0])',          $eval);
    $eval = str_replace('(a+c,1)',   '([$a+'.$c.',0],[1,0])',          $eval);
    $eval = str_replace('(a+c,2)',   '([$a+'.$c.',0],[2,0])',          $eval);
  }

  $eval = preg_replace('@1/(\$[_a-zA-Z0-9]+)@', 'fp_div([1,0],$1)', $eval);
  $let  = '[^)]';
  $eval = preg_replace('@1/([_a-zA-Z0-9]+[(]'.$let.'*[)])@', 'fp_div([1,0],$1)', $eval);
  $let  = '(?:'.$let.'|[(]'.$let.'*[)])';
  $eval = preg_replace('@1/([_a-zA-Z0-9]+[(]'.$let.'*[)])@', 'fp_div([1,0],$1)', $eval);
  $let  = '(?:'.$let.'|[(]'.$let.'*[)])';
  $eval = preg_replace('@1/([_a-zA-Z0-9]+[(]'.$let.'*[)])@', 'fp_div([1,0],$1)', $eval);
  $eval = preg_replace('@-([_a-zA-Z0-9]+[(]'.$let.'*[)])@', 'fp_mul([-1,0],$1)', $eval);
  print "eval: $eval\n";

  global $failed;
  $failed = false;
  if($do_complex)
  {
    for($x = $min; $x <= $max; $x += $step)
    for($y = $min; $y <= $max; $y += $step)
    for($z = $min; $z <= $max; $z += $step)
    for($w = $min; $w <= $max; $w += $step)
    {
      eval("\$k = $eval; if(0)print \"$eval = \$k\n\";");
      if($failed)
      {
        #print "failed with ".json_encode($const)."\n";
        return false;
      }
      if(!$use_b) continue 3; // Ignore looping for $z,$w
    }
  }
  else
  {
    for($a = $min; $a <= $max; $a += $step)
    for($b = $min; $b <= $max; $b += $step)
    {
      eval("\$k = $eval; if(0)print \"$eval = \$k\n\";");
      if($failed)
      {
        #print "failed with ".json_encode($const)."\n";
        return false;
      }
      if(!$use_b) continue; // Ignore looping for $b
    }
  }

  # Substitutions in fp code:
  # Substitutions in C code:
  #if($use_b)
  #  $res = str_replace('C=', 'C=Value_t b=z,u=i;X;u*=w;X;b+=u;X;return ', $res);
  #$res = str_replace('C=', 'C=Value_t a=x,t=i;X;t*=y;X;a+=t;X;return ', $res);
  #$res = str_replace('return Value_t', 'Value_t', $res);

  $res = str_replace('((a+c)/(b+d)','((a+'.$c.')/(b+'.$d.')', $res);
  $res = str_replace('(a+c)',       '(a+'.$c.')',         $res);
  $res = str_replace('(1,a+c)',     '(1,a+'.$c.')',         $res);
  $res = str_replace('(2,a+c)',     '(2,a+'.$c.')',         $res);
  $res = str_replace('(a+c,1)',     '(a+'.$c.',1)',         $res);
  $res = str_replace('(a+c,2)',     '(a+'.$c.',2)',         $res);
  $res = str_replace('(b+d)',       '(b+'.$d.')',         $res);
  $res = str_replace('(a+c,b+d)',   '(a+'.$c.',b+'.$d.')', $res);
  #$res = str_replace('(a)',   '(x+fp_make_imag(1)*y)',         $res);
  #$res = str_replace('(b)',   '(z+fp_make_imag(1)*w)',         $res);
  #$res = str_replace('(a,b)', '(x+fp_make_imag(1)*y, z+fp_make_imag(1)*w)', $res);
  #$res = preg_replace('@C=(.*)@', 'C=[=]{$1;}()', $res);
  // X is a separator for the sake of inhibiting math optimizations
  #$res = str_replace('X', 'FunctionParserBase<Value_t>::epsilon()', $res);

  # Cleanup for fp
  $res = str_replace('1a', '(a+'.$c.')', $res);
  $res = str_replace('1b', '(b+'.$d.')', $res);
  return $res;
}
function valsubst($code, $name)
{
  $const = array();
  // Include negative
  // Include minus1
  // Include zero
  // Include plus1
  // Include positive
  $val = [
    [-2,2,0.5], // all
    [-1,1,0.5], // all, less or equal than 1
    [-2,2,2],   // all, skips -1 and 1
    [-3,3,2],   // all, skips 0
    [-0.75, 0.75, 0.25], // less than 1, doesn't skip 0
    [-0.5,  0.5,  0.25], // less or equal than 0.5, doesn't skip 0
    [-0.25, 0.25, 0.05], // less or equal than 0.25, doesn't skip 0
    [-0.2,  0.2,  0.05], // less than 0.25,          doesn't skip 0
    [0,2,0.5], // 0, 0.5, 1, 1.5, 2              skips neg
    [0,1,0.5], // 0, 0.5, 1                      skips neg and 2
    [0,4,0.75], // 0, .75, 1.5, 2.75, 3.25, 4    skips neg and 1
    [0,0.75, 0.25], // 0, less than 1

    [0.5,2,0.5],
    [0.5,1,0.25],
    [0.25,3.25,0.5],
    [0.25,0.75,0.125]
  ];
  if($name == '12optimizer_nested2func/e_e'
  || $name == '12optimizer_nested2func/t_e'
  || $name == '12optimizer_nested2func/e_e_r'
  || $name == '12optimizer_nested2func/t_e_r'
    ) // complex<float> workaround
  {
    $val[0] = [-1,1,0.25];
  }
  $use_b = preg_match('/V=a,b/', $code);

  print "Trying $name: $code\n";
  for($step=0; $step< count($val)+200; ++$step)
  {
    $try = $step>>2;
    if(isset($val[$try]))
    {
      $const = Array('A' => $val[$try][0], 'B' => $val[$try][1], 'S' => $val[$try][2],
                     'C' => 0,
                     'D' => 0);
      if($step&3)
      {
        $const['C'] = rand(-50,50)/16;
        $const['D'] = rand(-50,50)/16;
      }
    }
    $test = test($code, $const, $use_b);
    if($test !== false) break;

    $const['A'] = (1/16) * rand(-100,100);
    $const['B'] = $const['A'] + (1/16) * rand(10,100);
    $const['S'] = 1/16;
    $const['C'] = rand(1,50)/100;
    $const['D'] = rand(1,50)/100;
  }
  return $test;
}

foreach($outer as $f)
foreach($inner as $m)
{
  //
  $do_complex = true;
  $cplx_name = "12optimizer_nested2func/{$f[0]}_{$m[1]}";

  $code =
"V={$m[0]}\n". # Variables
"R=A,B,S\n".
"F={$f[1]}({$m[2]})\n".
"C={$f[2]}({$m[3]})\n";
  $cplx_code = valsubst($code, $cplx_name);

  //
  $do_complex = false;
  $real_name = "12optimizer_nested2func/{$f[0]}_{$m[1]}_r";
  $code =
"V={$m[0]}\n". # Variables
"R=A,B,S\n".
"F={$f[1]}({$m[2]})\n".
"C={$f[2]}({$m[3]})\n";
  $real_code = valsubst($code, $real_name);

  //
  if($real_code == $cplx_code)
  {
    if($real_code == '') continue;
    $code = "T=f d ld mf cf cd cld\n$real_code";
    $comb_name = "{$cplx_name}_";
    file_put_contents($comb_name, $code);
  }
  else
  {
    if($cplx_code != '')
    {
      $code = "T=cf cd cld\n$cplx_code";
      file_put_contents($cplx_name, $code);
    }
    if($real_code != '')
    {
      $code = "T=f d ld mf\n$real_code";
      file_put_contents($real_name, $code);
    }
  }
}
