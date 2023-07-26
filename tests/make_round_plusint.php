<?php
$surround = Array(
  ['f', 'floor','fp_floor'],
  ['c', 'ceil','fp_ceil'],
  ['t', 'trunc','fp_trunc'],
  ['i', 'int','fp_int'],
);
$inner = Array(
  ['f', 'floor(y)','fp_floor(y)'],
  ['c', 'ceil(y)', 'fp_ceil(y)'],
  ['t', 'trunc(y)','fp_trunc(y)'],
  ['i', 'int(y)',  'fp_int(y)'],
  ['p', '(x<y)',   'fp_less(x,y)'],
  ['n', '-(abs(y)+1)',  '-(fp_abs(y)+1)'],
  ['af', 'abs(floor(y))','fp_abs(fp_floor(y))'],
  ['ac', 'abs(ceil(y))', 'fp_abs(fp_ceil(y))'],
  ['at', 'abs(trunc(y))','fp_abs(fp_trunc(y))'],
  ['ai', 'abs(int(y))',  'fp_abs(fp_int(y))'],
  ['an', 'abs(y)', 'fp_abs(y)']
);

foreach($surround as $sur)
foreach($inner    as $in)
{
  $code =
"T=ld f mf cd cf cld\n".
"V=x,y\n".
"R=-1.5,1.5,0.5\n".
"F={$sur[1]}(x+{$in[1]})\n".
"C={$sur[2]}(x+{$in[2]})\n";
  file_put_contents("20optimizer_optimizations/round_plusint/{$sur[0]}_{$in[0]}", $code);
}
