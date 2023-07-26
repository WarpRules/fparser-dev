<?php

require 'util/func_simulator.php';

/*
Patterns to test:
   func(1i * x)
   func(3i * x)
   func(-1i * x)
   func(-3i * x)
   func(x)
   func( 2 * x)
   func(-1 * x)
   func(-4 * x)
For functions:
   sin, cos, tan, sinh, cosh, tanh
   asin,acos,atan,asinh,acosh,atanh
*/
$outer = Array(
  ['sin', 'sin','fp_sin'],
  ['cos', 'cos','fp_cos'],
  ['tan', 'tan','fp_tan'],
  ['asin', 'asin','fp_asin'],
  ['acos', 'acos','fp_acos'],
  ['atan', 'atan','fp_atan'],
  ['sec', 'sec','1/fp_cos'],
  ['csc', 'csc','1/fp_sin'],
  ['cot', 'cot','1/fp_tan'],
  ['sinh', 'sinh','fp_sinh'],
  ['cosh', 'cosh','fp_cosh'],
  ['tanh', 'tanh','fp_tanh'],
  ['asinh', 'asinh','fp_asinh'],
  ['acosh', 'acosh','fp_acosh'],
  ['atanh', 'atanh','fp_atanh'],
);
$inner = Array(
  ['1i', '1i', 'i',                '[0,1]'],
  ['3i', '3i', '(i+i+i)',          '[0,3]'],
  ['mi', '-1i', 'fp_conj(i)',      '[0,-1]'],
  ['m3i', '-3i', 'fp_conj(i+i+i)', '[0,-3]'],
# ['1',  '1',  '1',                '[1,0]'],
  ['2',  '2',  '2',                '[2,0]'],
  ['m1', '-1', '-1',               '[-1,0]'],
  ['m2', '-2', '-2',               '[-2,0]'],
);

function test($code, $const)
{
  global $do_complex, $failed;

  $min=$const['A'];
  $max=$const['B'];
  $step=$const['S'];
  $res = str_replace('A', $min, $code);
  $res = str_replace('B', $max, $res);
  $res = str_replace('S', $step, $res);

  $eval = preg_replace('/.*X=/ms', '', $res);

  if($do_complex)
  {
    $eval = str_replace('(a',     '([$x,$y]',         $eval);
  }
  else
  {
    $eval = str_replace('(a',     '([$a,0]',          $eval);
  }

  $eval = preg_replace('@1/(\$[_a-zA-Z0-9]+)@', 'zerocheck($1)', $eval);
  $let  = '[^)]';
  $eval = preg_replace('@1/([_a-zA-Z0-9]+[(]'.$let.'*[)])@', 'zerocheck($1)', $eval);
  $let  = '(?:'.$let.'|[(]'.$let.'*[)])';
  $eval = preg_replace('@1/([_a-zA-Z0-9]+[(]'.$let.'*[)])@', 'zerocheck($1)', $eval);
  $let  = '(?:'.$let.'|[(]'.$let.'*[)])';
  $eval = preg_replace('@1/([_a-zA-Z0-9]+[(]'.$let.'*[)])@', 'zerocheck($1)', $eval);
  #print "eval: $eval\n";

  $failed = false;
  if($do_complex)
  {
    for($x = $min; $x <= $max; $x += $step)
    for($y = $min; $y <= $max; $y += $step)
    {
      eval("\$k = $eval; if(0)print \"$eval = \$k\n\";");
      if($failed)
      {
        #print "failed with ".json_encode($const)."\n";
        return false;
      }
    }
  }
  else
  {
    for($a = $min; $a <= $max; $a += $step)
    {
      eval("\$k = $eval; if(0)print \"$eval = \$k\n\";");
      if($failed)
      {
        #print "failed with ".json_encode($const)." when a=$a\n";
        return false;
      }
    }
  }

  # Substitutions in fp code:
  # Substitutions in C code:
  #$res = str_replace('C=', 'C=Value_t a=x,t=i;X;t*=y;X;a+=t;X;return ', $res);
  #$res = str_replace('return Value_t', 'Value_t', $res);

  #$res = str_replace('(a)',   '(x+fp_make_imag(1)*y)',         $res);
  #$res = str_replace('(b)',   '(z+fp_make_imag(1)*w)',         $res);
  #$res = str_replace('(a,b)', '(x+fp_make_imag(1)*y, z+fp_make_imag(1)*w)', $res);
  #$res = preg_replace('@C=(.*)@', 'C=[=]{$1;}()', $res);
  // X is a separator for the sake of inhibiting math optimizations
  #$res = str_replace('X', 'FunctionParserBase<Value_t>::epsilon()', $res);

  # Cleanup for fp
  $res = str_replace('1a', 'a', $res);
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

  print "Trying $name: $code\n";
  for($step=0; $step< count($val)+2000; ++$step)
  {
    $try = $step;
    if(isset($val[$try]))
    {
      $const = Array('A' => $val[$try][0], 'B' => $val[$try][1], 'S' => $val[$try][2]);
    }
    $test = test($code, $const);
    if($test !== false) break;

    $const['A'] = (1/16) * rand(-100,100);
    $const['B'] = $const['A'] + (1/16) * rand(10,100);
    $const['S'] = 1/16;
  }
  return $test;
}

foreach($outer as $f)
foreach($inner as $m)
{
  //
  $do_complex = true;
  $cplx_name = "13optimizer_trig_constfactor/{$f[0]}_{$m[0]}";
  $code =
"V=a\n". # Variables
"R=A,B,S\n".
"F={$f[1]}(1a*{$m[1]})\n".
"C={$f[2]}(a*{$m[2]})\n".
"#X={$f[2]}(fp_mul(a,{$m[3]}))\n";
  $cplx_code = valsubst($code, $cplx_name);

  //
  $do_complex = false;
  $real_name = "13optimizer_trig_constfactor/{$f[0]}_{$m[0]}_r";
  $code =
"V=a\n". # Variables
"R=A,B,S\n".
"F={$f[1]}(1a*{$m[1]})\n".
"C={$f[2]}(a*{$m[2]})\n".
"#X={$f[2]}(fp_mul(a,{$m[3]}))\n";
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
