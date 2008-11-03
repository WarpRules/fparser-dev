<?php
$inputfile = $argv[1]; // will be parsed (generated by bison -dv file.y)

/* PARSER PARSER TOOL
 * Copyright (C) 1992,2008 Joel Yliluoma (http://iki.fi/bisqwit/)
 *
 */

#require 'arraydebug.php';

$rules        = Array();
$terminals    = Array(); // i.e. tokens
$nonterminals = Array(); // i.e. named rules
$states       = Array();

define('STATE_UNKNOWN', 0);
define('STATE_GRAMMAR', 1);
define('STATE_TERMINALS', 2);
define('STATE_NONTERMINALS', 3);
define('STATE_STATE', 4);
$mode = STATE_UNKNOWN;

$state = 0;

$rule_result = '';

foreach(explode("\n", file_get_contents($inputfile)) as $line)
{
  if($line != '')
  switch($mode)
  {
    case STATE_UNKNOWN:
      if($line == 'Grammar') $mode = STATE_GRAMMAR;
      break;
    case STATE_GRAMMAR:
      if(ereg('^Terminals, ', $line)) { $mode = STATE_TERMINALS; break; }
      
      $rule   = ereg_replace('^ *[0-9]+ ', '', $line);

      if(!ereg('^ *\|', $rule))
        $rule_result = ereg_replace(' *:.*', '', $rule);
      
      $params = preg_replace('@^.*?[:|] *@', '', $line);
      $n_reduce = 0;
      $reduce_params = Array(0 => Array());
      if($params != '/* empty */')
      {
        preg_match_all('@(\'.\'|[^ ]+)@', $params, $reduce_params);
        $n_reduce = count($reduce_params[0]);
      }
      
      if($n_reduce <= 6)
      {
        foreach($reduce_params[0] as $k=>&$v)
        {
          if($v[0]!='\'')
          {
            if(strtoupper($v)==$v)
              $v = "T_$v";
            else
              $v = "NT_$v";
          }
          $v = ",$v";
        }
        $rule_template_param = "R$n_reduce(NT_$rule_result".join('', $reduce_params[0]).')';
      }
      else
      {
        $rule_template_param = 'TokenEOF';
        foreach(array_reverse($reduce_params[0]) as $param)
          $rule_template_param = "RP<T_$param, $rule_template_param> ";
        $rule_template_param = "RulePat<NT_$rule_result, $rule_template_param>";
      }

      $rule = str_replace(Array('/*', '*/'), Array('(*', '*)'), $rule);
      
      #print "Reduce<$rule_template_param >\n";
      
      $rules[(int)$line] = Array($rule, $n_reduce, $rule_template_param, "NT_$rule_result");
      break;
    case STATE_TERMINALS:
      if(ereg('^Nonterminals, ', $line)) { $mode = STATE_NONTERMINALS; break; }
      $token = preg_replace('@ \(.*@', '', $line);
      if(/*$token != '$end' &&*/ $token != 'error')
      {
        $terminals[$token] = "T_$token";
      }
      break;
    case STATE_NONTERMINALS: 
      if(ereg('^state [0-9]', $line)) { $mode = STATE_STATE; break; }
      if($line[0] == ' ') break;
      $token = preg_replace('@ \(.*@', '', $line);
      $nonterminals["NT_$token"] = count($nonterminals);
      break;
    case STATE_STATE:
      if(ereg('^ *[0-9]+', $line)) break;
      if(ereg('^state [0-9]', $line))
      {
        $state = (int)substr($line, 6);
        break;
      }
      $statedata = &$states[$state];
      if(!isset($statedata)) $statedata = Array('tok' => Array(), 'nt' => Array());
      
      #print "$line ...\n";
      
      if(ereg('shift, and go to', $line))
      {
        $tok       = preg_replace('@^ +(.*?) +\[?shift,.*@', '\1', $line);
        $newstate  = (int)preg_replace('@.*to state @', '', $line);
        
        $statedata['tok'][$tok][] = Array('s', $newstate);
      }
      elseif(ereg('reduce using rule', $line))
      {
        $tok       = preg_replace('@^ +(.*?) +\[?reduce .*@', '\1', $line);
        $usingrule = preg_replace('@.*using rule ([0-9]+) .*@', '\1', $line);
        $usingnt   = preg_replace('@.*using rule [0-9]+ \(?(.*?)\).*@', '\1', $line);

        $statedata['tok'][$tok][] = Array('r', $usingrule, $usingnt);
      }
      elseif(ereg('go to state', $line))
      {
        $usingrule = preg_replace('@^ +(.*?) +\[?go to.*@', '\1', $line);
        $newstate  = (int)preg_replace('@.*to state @', '', $line);
        
        $statedata['nt'][$usingrule] = $newstate;
      }
      elseif(ereg('accept', $line))
      {
        $tok = preg_replace('@^ +(.*?) +\[?accept.*@', '\1', $line);
        
        $statedata['tok'][$tok][] = Array('a');
      }
      else print "? $line\n";
      break;
  }
}
unset($statedata);

$c=0;
foreach($terminals as $t) $$t = $c++;
$c=0;
foreach($nonterminals as $nt => $dummy) $$nt = $c++;

ob_start();

$max_t_len = 0;
foreach($terminals as $t) $max_t_len = max($max_t_len, strlen($t));
$max_nt_len = 0;
foreach($nonterminals as $nt=>$dummy) $max_nt_len = max($max_nt_len, strlen($nt));

?>
enum Terminals
{
    <?php echo wordwrap(join(', ', $terminals), 60, "\n    ");?>,
    NUM_TERMINALS
};
enum NonTerminals
{
    <?php echo wordwrap(join(', ', array_keys($nonterminals)), 60, "\n    ");?>,
    NUM_NONTERMINALS
};
static const char TerminalNames[NUM_TERMINALS][1+<?=$max_t_len?>] =
{
    <?php echo preg_replace('/([A-Z0-9_a-z$@]+)/', '"\1"',
         wordwrap(join(', ', $terminals), 60, "\n    "));?> 
};
static const char NonTerminalNames[NUM_NONTERMINALS][1+<?=$max_nt_len?>] =
{
    <?php echo preg_replace('/([A-Z0-9_a-z$@]+)/', '"\1"',
         wordwrap(join(', ', array_keys($nonterminals)), 60, "\n    "));?> 
};
static const struct BisonState
{
    char Actions[NUM_TERMINALS], Goto[NUM_NONTERMINALS];
} States[] =
{
<?php
  $stateno = 0;
  foreach($states as $state)
  {
    $actions = Array();
    $goto    = Array();
    
    $default_action = 0;
    foreach($state['tok'] as $tokname => $tokactions)
      foreach($tokactions as $action)
      {
        if($action[0] == 's')
          $action = $action[1];
        elseif($action[0] == 'a')
          $action = 127;
        else
          $action = -$action[1];
        
        if($tokname == '$default')
          $default_action = $action;
        else
          $actions[${"T_$tokname"}] = $action;
      }
    foreach(array_values($terminals) as $c=>$t)
      if(!isset($actions[$c]))
        $actions[$c] = $default_action;
    ksort($actions);
    
    foreach(array_keys($nonterminals) as $c=>$t)
      $goto[$c] = 0;
    
    foreach($state['nt'] as $ntname => $newstate)
      $goto[${"NT_$ntname"}] = $newstate;
    
    foreach($actions as &$c) $c = sprintf('%3d',$c);
    unset($c);
    
    if(!($stateno % 5))
      echo "  // state $stateno\n";
    
    echo "  { { ", join(',', $actions), '}, { ', join(',', $goto), "} },\n";
    ++$stateno;
  }
?>
};
static const struct BisonReduce
{
  int          n_reduce:8;
  NonTerminals produced_nonterminal:8;
} BisonGrammar[] =
{
<?php
  foreach($rules as $ruleno => $ruledata)
    printf("  {%3d, %s}, /* %s */\n",
      $ruledata[1],
      str_pad($ruledata[3], $max_nt_len),
      $ruledata[0]);
?>
};
<?php

$s = ob_get_clean();

print str_replace(Array('$','@'),Array('Z','A'), $s);
