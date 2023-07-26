<?php
/*
func(logical)
func(if(logical, const1, const2))
func(logical + const1)
func(logical * const1)
*/
require 'util/func_simulator_realonly.php';

$func = Array(
  ['s', 'sin',  'fp_sin'],
  ['c', 'cos',  'fp_cos'],
  ['t', 'tan',  'fp_tan'],
  ['as', 'asin', 'fp_asin'],
  ['ac', 'acos', 'fp_acos'],
  ['at', 'atan', 'fp_atan'],
  ['e', 'sec',  '1/fp_cos'],
  ['a', 'csc',  '1/fp_sin'],
  ['o', 'cot',  '1/fp_tan'],
  ['sh', 'sinh', 'fp_sinh'],
  ['ch', 'cosh', 'fp_cosh'],
  ['th', 'tanh', 'fp_tanh'],
  ['ash', 'asinh','fp_asinh'],
  ['ach', 'acosh','fp_acosh'],
  ['ath', 'atanh','fp_atanh'],
  ['l',  'log',  'fp_log'],
  ['l2', 'log2', 'fp_log2'],
  ['lt', 'log10','fp_log10'],
  ['ex', 'exp',  'fp_exp'],
  ['e2', 'exp2', 'fp_exp2'],
  ['fl', 'floor','fp_floor'],
  ['cl' ,'ceil', 'fp_ceil'],
  ['i', 'int',  'fp_int'],
  ['tr', 'trunc','fp_trunc'],
  ['ab', 'abs',  'fp_abs'],
  ['sq', 'sqrt', 'fp_sqrt'],
  ['cb', 'cbrt', 'fp_cbrt'],
);
$mode = Array(
  ['l', 'x<y',           'fp_less(x,y)'],
  ['i', 'if(x<y, D, E)', '(fp_less(x,y)!=0 ? D : E)'],
  ['p', '(x<y)+D',       'fp_less(x,y)+D'],
  ['mc', '(x<y)*D',      'fp_less(x,y)*D'],
  ['mv', '(x<y)*x*y*D',  'fp_less(x,y)*x*y*D']
);
function test($code, $const)
{
  $min=$const['A'];
  $max=$const['B'];
  $step=$const['S'];
  $res = str_replace('A', $min, $code);
  $res = str_replace('B', $max, $res);
  $res = str_replace('S', $step, $res);
  $res = str_replace('D', $const['D'], $res);
  $res = str_replace('E', $const['E'], $res);
  $eval = preg_replace('/.*C=/ms', '', $res);
  $eval = str_replace('(x', '($x', $eval);
  $eval = str_replace('*x', '*$x', $eval);
  $eval = str_replace('y', '$y', $eval);
  if(substr($eval, 0, 2) == '1/')
  {
    $eval = "zerocheck(".substr($eval,2).")";
  }
  global $failed;
  $failed = false;
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
  return $res;
}
function valsubst($code)
{
  $const = array('D' => -1.1, 'E' => '1.2');
  // Include negative
  // Include minus1
  // Include zero
  // Include plus1
  // Include positive
  $val = [
    [-2,2,0.5], // all
    [-1,1,0.5], // all, skips -2 and 2
    [-2,2,2],   // all, skips -1 and 1
    [-3,3,2],   // skips 0
    [-0.75, 0.75, 0.25], // less than 1
    [0,2,0.5], // 0, 0.5, 1, 1.5, 2              skips neg
    [0,1,0.5], // 0, 0.5, 1                      skips neg and 2
    [0,4,0.75], // 0, .75, 1.5, 2.75, 3.25, 4    skips neg and 1
    [0,0.75, 0.25], // 0, less than 1

    [0.5,2,0.5],
    [0.5,1,0.25],
    [0.25,3.25,0.5],
    [0.25,0.75,0.125]
  ];
  print "Trying $code\n";
  for($try=0; $try<10*count($val)+300; ++$try)
  {
    $tt = (int)($try / 10);
    if(isset($val[$tt]))
    {
       $const['A'] = $val[$tt][0];
       $const['B'] = $val[$tt][1];
       $const['S'] = $val[$tt][2];
    }

    $test = test($code, $const);
    if($test !== false) break;

    $const['D'] = rand(-14,14)/10;
    $const['E'] = rand(-14,14)/10;
    $const['A'] = rand(-20,10)/10;
    $const['B'] = $const['A'] + rand(10,100)/10;
    $const['C'] = rand(1,50)/100;
  }
  return $test;
}
foreach($func as $f)
foreach($mode as $m)
{
  $name = "20optimizer_optimizations/if_hoist/{$f[0]}_{$m[0]}";
  $code =
"T=d ld f\n".
"V=x,y\n".
"R=A,B,S\n".
"F={$f[1]}({$m[1]})\n".
"C={$f[2]}({$m[2]})\n";
  $code = valsubst($code);
  if($code != '')
    file_put_contents($name, $code);
}
