<?php

require 'util/func_simulator.php';

/*
Patterns to test:
   fun1(x) + fun2(x)
   fun1(x) + fun2(-x)
   fun1(-x) + fun2(-x)
   fun1(x) - fun2(x)
   fun1(x) - fun2(-x)
   fun1(-x) - fun2(x)
   fun1(-x) - fun2(-x)
   -fun1(x) - fun2(x)
   -fun1(x) - fun2(-x)
   -fun1(-x) - fun2(x)
   -fun1(-x) - fun2(-x)
For functions:
   sin, cos, tan, sinh, cosh, tanh,  exp
   asin,acos,atan,asinh,acosh,atanh
*/
$outer = Array(
  ['s', 'sin','fp_sin'],
  ['c', 'cos','fp_cos'],
  ['t', 'tan','fp_tan'],
  ['as', 'asin','fp_asin'],
  ['ac', 'acos','fp_acos'],
  ['at', 'atan','fp_atan'],
  ['ex', 'exp', 'fp_exp'],
  ['l',  'log', 'fp_log'],
  ['sh', 'sinh','fp_sinh'],
  ['ch', 'cosh','fp_cosh'],
  ['th', 'tanh','fp_tanh'],
  ['ash', 'asinh','fp_asinh'],
  ['ach', 'acosh','fp_acosh'],
  ['ath', 'atanh','fp_atanh'],
);
$ways = Array(
  ['ppp', 'A(1a)+B(1a)',   'A(a)+B(a)',   'fp_add(A(a),B(a))'],
  ['ppm', 'A(1a)+B(-1a)',  'A(a)+B(-a)',  'fp_add(A(a),B(fp_mul(a,[-1,0])))'],
  ['pmm', 'A(-1a)+B(-1a)', 'A(-a)+B(-a)', 'fp_add(A(fp_mul(a,[-1,0])),B(fp_mul(a,[-1,0])))'],
  ['mpp', 'A(1a)-B(1a)',   'A(a)-B(a)',   'fp_sub(A(a),B(a))'],
  ['mpm', 'A(1a)-B(-1a)',  'A(a)-B(-a)',  'fp_sub(A(a),B(fp_mul(a,[-1,0])))'],
  ['mmp', 'A(-1a)-B(1a)',  'A(-a)-B(a)',  'fp_sub(A(fp_mul(a,[-1,0])),B(a))'],
  ['mmm', 'A(-1a)-B(-1a)', 'A(-a)-B(-a)', 'fp_sub(A(fp_mul(a,[-1,0])),B(fp_mul(a,[-1,0])))'],
  ['npp', '-A(1a)-B(1a)',   '-A(a)-B(a)',   'fp_sub(fp_mul(A(a),[-1,0]),B(a))'],
  ['npm', '-A(1a)-B(-1a)',  '-A(a)-B(-a)',  'fp_sub(fp_mul(A(a),[-1,0]),B(fp_mul(a,[-1,0])))'],
  ['nmm', '-A(-1a)-B(-1a)', '-A(-a)-B(-a)', 'fp_sub(fp_mul(A(fp_mul(a,[-1,0])),[-1,0]),B(fp_mul(a,[-1,0])))'],
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

    [-2, 0, 0.5],
    [-1, 0, 0.5],
    [-4, 0, 0.75],
    [-0.75, 0, 0.25],
    [-2, -0.1, 0.5],
    [-1, -0.1, 0.5],
    [-4, -0.1, 0.75],
    [-0.75, -0.25, 0.25],

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

foreach($outer as $f1)
foreach($outer as $f2)
foreach($ways as $w)
{
  if($w[0] == 'ppp' || $w[0] == 'nmm' || $w[0] == 'npp' || $w[0] == 'pmm')
  {
    // Skip some duplicate cases:
    //   ppp:  sin( a) + cos( a)   v.s.   cos( a) + sin( a)
    //   pmm:  sin( a) + cos( a)   v.s.   cos( a) + sin( a)
    //   npp: -sin( a) - cos( a)   v.s.  -cos( a) - sin( a)
    //   nmm: -sin(-a) - cos(-a)   v.s.  -cos(-a) - sin(-a)
    if($f1[0] > $f2[0]) continue;
  }
  // skip also:


  //
  $do_complex = true;
  $cplx_name = "{$w[0]}/{$f1[0]}_{$f2[0]}";
  $code =
"V=a\n". # Variables
"R=A,B,S\n".
"F=".str_replace('B',$f2[1], str_replace('A',$f1[1], $w[1]))."\n".
"C=".str_replace('B',$f2[2], str_replace('A',$f1[2], $w[2]))."\n".
"#X=".str_replace('B',$f2[2], str_replace('A',$f1[2], $w[3]))."\n";
  $cplx_code = valsubst($code, $cplx_name);

  //
  $do_complex = false;
  $real_name = "{$w[0]}/{$f1[0]}_{$f2[0]}_r";
  $code =
"V=a\n". # Variables
"R=A,B,S\n".
"F=".str_replace('B',$f2[1], str_replace('A',$f1[1], $w[1]))."\n".
"C=".str_replace('B',$f2[2], str_replace('A',$f1[2], $w[2]))."\n".
"#X=".str_replace('B',$f2[2], str_replace('A',$f1[2], $w[3]))."\n";
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
