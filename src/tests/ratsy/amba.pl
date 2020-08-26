#! /usr/bin/perl -w 

# this specification generator is provided by Barbara Jobstmann 
# and adapted for NuSMV by Andrei Tchaltsev

###############################################
# perl script to generated a config file for
# synthesizing an ARM AMBA AHB with an arbitrary
# number of masters.
#
# Usage: ./ahb_spec_generator.pl <num_of_masters>
#
# Authors: Martin Weiglhofer, Barbara Jobstmann
#
###############################################
use strict;
use POSIX; # qw(ceil floor);

###############################################
sub buildStateString {
    my ($state_name, $op, $num_states, $value, $padd_value) = @_;
    my $result = "";
    
    if(! defined $padd_value) {
	$padd_value = "0";
    }
    
    my $bin = reverse sprintf("%b", $value);
    
    for(my $j = 0; $j < $num_states; $j++) {
	if(!($result eq "")) {
	    $result .= " " . $op . " ";
	}
	
	my $bin_val = $padd_value;
	if($j < length($bin)) {
	    $bin_val = substr($bin, $j, 1);
	}
	
	if($bin_val == "0"){
		$result .= "!" . $state_name . $j ;
	}else{
		$result .= $state_name . $j;
	}
	
    }
    
    return $result;
}

###############################################
sub buildHMasterString {
    my ($master_bits, $value) = @_;
    return buildStateString("hmaster", "&&", $master_bits, $value);
}

###############################################
sub buildNegHMasterString {
    my ($master_bits, $value) = @_;
    
    my $bin = sprintf("%b", $value);
    my $new_val = "";
    for(my $j = 0; $j < length($bin); $j++) {
	if(substr($bin, $j, 1) eq "0") {
	    $new_val .= "1";
	} else {
	    $new_val .= "0";
	}
    }
    
    $bin = reverse $new_val;
    my $result = "";
    for(my $j = 0; $j < $master_bits; $j++) {
	if(!($result eq "")) {
	    $result .= "||";
	}
	
	my $bin_val = "1";
	if($j < length($bin)) {
	    $bin_val = substr($bin, $j, 1);
	}
	if($bin_val == "0"){
		$result .= "!hmaster$j";
	}else{
		$result .= "hmaster$j";
	}	
    }
    
    return $result;
}


###############################################
#                MAIN
###############################################

if(! defined($ARGV[0])) {
    print "Usage: ./ahb_spec_generator.pl <num_of_masters>\n";
    exit;
}


my $num_masters = $ARGV[0];
my $master_bits = ceil((log $num_masters)/(log 2));
if($master_bits == 0) {
    $master_bits = 1;
}
my $master_bits_plus_one = ceil((log($num_masters||1))/(log 2));
if($master_bits == 0) {
    $master_bits = 1;
}

my $zero_state_string = buildStateString("stateA1_", "&&", $master_bits_plus_one, 0);

my $env_initial = "";
my $sys_initial = "";
my $env_transitions = "";
my $sys_transitions = "";
my $env_fairness = "";
my $sys_fairness = "";
my $env_fairness_list = "";
my $sys_fairness_list = "";
my $input_vars = "";
my $output_vars = "";

###############################################
# ENV_INITIAL and INPUT_VARIABLES
###############################################

$env_initial .= "!hready &&";
$input_vars  .= "hready, ";

for(my $i = 0; $i < $num_masters; $i++) {
    $env_initial .= "!hbusreq" . $i . " && ";
    $env_initial .= "!hlock" . $i . " && ";
    $input_vars .= "hbusreq" . $i . ", ";
    $input_vars .= "hlock" . $i . ", ";
}

$env_initial .= "!hburst0 &&";
$input_vars .= "hburst0, ";
$env_initial .= "!hburst1";
$input_vars .= "hburst1";

###############################################
# ENV_TRANSITION
###############################################
for(my $i = 0; $i < $num_masters; $i++) {
    $env_transitions .= "//Assumption 3:\n";
    $env_transitions .= "ltl env rho A3_$i in AHB = []( hlock$i -> hbusreq$i ).//A3\n";
}

###############################################
# ENV_FAIRNESS
###############################################
$env_fairness .= "//Assumption 1:\n";
$env_fairness .= "ltl fluent A1 = ((".$zero_state_string.")).\n";
$env_fairness_list .= "A1, ";
$env_fairness .= "\n//Assumption 2:\n";
$env_fairness .= "ltl fluent A2 = ((hready)).\n";
$env_fairness_list .= "A2";

###############################################
# SYS_INITIAL || OUTPUT_VARIABLES
###############################################

for(my $i = 0; $i < $master_bits; $i++) {
    $sys_initial .= "!hmaster$i && ";
    $output_vars .= "hmaster$i, ";
}

$sys_initial .= "!hmastlock && ";
$output_vars .= "hmastlock, ";
$sys_initial .= "start && ";
$output_vars .= "start, ";
$sys_initial .= "decide &&";
$output_vars .= "decide, ";
$output_vars .= "hlocked, ";
$sys_initial .= "!hlocked &&";

for(my $i = 0; $i < $num_masters; $i++) {
    my $value = "0";
    if($i == 0) {
	    $sys_initial .= "hgrant$i && ";
	    $output_vars .= "hgrant$i, ";
    }else{
	    $sys_initial .= "!hgrant$i && ";
	    $output_vars .= "hgrant$i, ";    
    }
}

#Assumption 1:
for(my $i = 0; $i < $master_bits_plus_one; $i++) {
    $sys_initial .= "!stateA1_$i && ";
    $output_vars .= "stateA1_$i, ";
}

#Guarantee 2:
for(my $i = 0; $i < $num_masters; $i++) {
    $sys_initial .= "!stateG2_$i &&";
    $output_vars .= "stateG2_$i, ";
}

#Guarantee 3:
$sys_initial .= "!stateG3_0 && ";
$output_vars .= "stateG3_0, ";
$sys_initial .= "!stateG3_1 && ";
$output_vars .= "stateG3_1, ";
$sys_initial .= "!stateG3_2 && ";
$output_vars .= "stateG3_2, ";

#Guarantee 10:
for(my $i = 1; $i < $num_masters; $i++) {
	if($i < ($num_masters - 1)){
	    $sys_initial .= "1stateG10_$i && ";
	    $output_vars .= "stateG10_$i, ";
	}else{
	    $sys_initial .= "!stateG10_$i";
	    $output_vars .= "stateG10_$i";
	}
}

###############################################
# SYS_TRANSITION
###############################################

#Assumption 1:
$sys_transitions .= "//Assumption 1:\n";
$sys_transitions .= "ltl sys rho A1_a in AHB = [](($zero_state_string && ((!hmastlock)||((hburst0)||(hburst1)))) -> X($zero_state_string)).\n";

for(my $i = 0; $i < $num_masters; $i++) {
    my $i_state_string = buildStateString("stateA1_", "&&",
					  $master_bits_plus_one, $i||1);
    my $hmaster = buildHMasterString($master_bits, $i);

    $sys_transitions .= "//  Master ".$i.":\n";
    $sys_transitions .= "ltl sys rho A1_$i in AHB = []((${zero_state_string} && ((hmastlock)&&($hmaster)&&((!hburst0)&&(!hburst1)))) ->X($i_state_string)).\n";
    $sys_transitions .= "ltl sys rho A1_${i}_b in AHB = []((${i_state_string} && (hbusreq$i)) ->X($i_state_string)).\n";
    $sys_transitions .= "ltl sys rho A1_${i}_c in AHB = [](($i_state_string && (!hbusreq$i)) ->X($zero_state_string)).\n";
}

#Guarantee 1:
$sys_transitions .= "\n//Guarantee 1:\n";
$sys_transitions .= "ltl sys rho G1 in AHB = []((!hready) -> X(!start)).\n";

#Guarantee 2:
$sys_transitions .= "\n//Guarantee 2:\n";
$zero_state_string = buildStateString("stateG2_", "&&", $master_bits_plus_one, 0);
for(my $i = 0; $i < $num_masters; $i++) {
    my $master_state_str = buildHMasterString($master_bits, $i);

    $sys_transitions .= "//  Master ".$i.":\n";
    $sys_transitions .= "ltl sys rho G2_${i}_a in AHB = [](((!stateG2_".$i.")&&(!hmastlock))->X(!stateG2_".$i.")).\n";
    $sys_transitions .= "ltl sys rho G2_${i}_b in AHB = [](((!stateG2_".$i.")&&(!start))    ->X(!stateG2_".$i.")).\n";
    $sys_transitions .= "ltl sys rho G2_${i}_c in AHB = [](((!stateG2_".$i.")&&(hburst0))  ->X(!stateG2_".$i.")).\n";
    $sys_transitions .= "ltl sys rho G2_${i}_d in AHB = [](((!stateG2_".$i.")&&(hburst1))  ->X(!stateG2_".$i.")).\n";
    $sys_transitions .= "ltl sys rho G2_${i}_e in AHB = [](((!stateG2_".$i.")&&(!(".$master_state_str.")))->X(!stateG2_".$i.")).\n";
    $sys_transitions .= "ltl sys rho G2_${i}_f in AHB = [] (((!stateG2_".$i.")&&(hmastlock)&&(start)&&(!hburst0)&&(!hburst1)&&(".$master_state_str."))->X(stateG2_".$i.")).\n";
    $sys_transitions .= "ltl sys rho G2_${i}_g in AHB = [](((stateG2_".$i.")&&(!start)&&(hbusreq".$i."))->X(stateG2_".$i.")).\n";
    $sys_transitions .= "ltl sys rho G2_${i}_h in AHB = [](((stateG2_".$i.")&&(start))->(start && !start)).\n";
    $sys_transitions .= "ltl sys rho G2_${i}_i in AHB = [](((stateG2_".$i.")&&(!start)&&(!hbusreq".$i."))  ->X(!stateG2_".$i.")).\n";
}

#Guarantee 3:
$sys_transitions .= "\n//Guarantee 3:\n";
$sys_transitions .= "ltl sys rho G3_a in AHB = [](((!stateG3_0)&&(!stateG3_1)&&(!stateG3_2)&&((!hmastlock)||(!start)||((hburst0)||(!hburst1)))) -> X((!stateG3_0)&&(!stateG3_1)&&(!stateG3_2))). \n";
$sys_transitions .= "ltl sys rho G3_b in AHB = [](((!stateG3_0)&&(!stateG3_1)&&(!stateG3_2)&&((hmastlock)&&(start)&&((!hburst0)&&(hburst1))&&(!hready))) ->  X((stateG3_0)&&(!stateG3_1)&&(!stateG3_2))). \n";
$sys_transitions .= "ltl sys rho G3_c in AHB = [](((!stateG3_0)&&(!stateG3_1)&&(!stateG3_2)&& ((hmastlock)&&(start)&&((!hburst0)&&(hburst1))&&(hready))) -> X((!stateG3_0)&&(stateG3_1)&&(!stateG3_2))). \n";
$sys_transitions .= " \n";
$sys_transitions .= "ltl sys rho G3_d in AHB = [](((stateG3_0)&&(!stateG3_1)&&(!stateG3_2)&&((!start)&&(!hready))) ->  X((stateG3_0)&&(!stateG3_1)&&(!stateG3_2))). \n";
$sys_transitions .= "ltl sys rho G3_e in AHB = [](((stateG3_0)&&(!stateG3_1)&&(!stateG3_2)&&((!start)&&(hready))) ->  X((!stateG3_0)&&(stateG3_1)&&(!stateG3_2))). \n";
$sys_transitions .= "\n";
$sys_transitions .= "ltl sys rho G3_f in AHB = [](((stateG3_0)&&(!stateG3_1)&&(!stateG3_2)&&((start))) -> (start && !start)). \n";
$sys_transitions .= "\n";
$sys_transitions .= " \n";
$sys_transitions .= "ltl sys rho G3_e in AHB = [](((!stateG3_0)&&(stateG3_1)&&(!stateG3_2)&&((!start)&&(!hready))) -> \n";
$sys_transitions .= " X((!stateG3_0)&&(stateG3_1)&&(!stateG3_2))). \n";
$sys_transitions .= "ltl sys rho G3_f in AHB = [](((!stateG3_0)&&(stateG3_1)&&(!stateG3_2)&&((!start)&&(hready))) -> \n";
$sys_transitions .= " X((stateG3_0)&&(stateG3_1)&&(!stateG3_2))). \n";
$sys_transitions .= "ltl sys rho G3_g in AHB = [](((!stateG3_0)&&(stateG3_1)&&(!stateG3_2)&&((start))) -> (start && !start)). \n";
$sys_transitions .= " \n";
$sys_transitions .= "ltl sys rho G3_h in AHB = [](((stateG3_0)&&(stateG3_1)&&(!stateG3_2)&&((!start)&&(!hready))) -> \n";
$sys_transitions .= " X((stateG3_0)&&(stateG3_1)&&(!stateG3_2))). \n";
$sys_transitions .= "ltl sys rho G3_i in AHB = [](((stateG3_0)&&(stateG3_1)&&(!stateG3_2)&&((!start)&&(hready))) -> \n";
$sys_transitions .= " X((!stateG3_0)&&(!stateG3_1)&&(stateG3_2))). \n";
$sys_transitions .= "ltl sys rho G3_j in AHB = [](((stateG3_0)&&(stateG3_1)&&(!stateG3_2)&&((start))) -> (start && !start)). \n";
$sys_transitions .= " \n";
$sys_transitions .= "ltl sys rho G3_k in AHB = [](((!stateG3_0)&&(!stateG3_1)&&(stateG3_2)&&((!start)&&(!hready))) -> \n";
$sys_transitions .= " X((!stateG3_0)&&(!stateG3_1)&&(stateG3_2))). \n";
$sys_transitions .= "ltl sys rho G3_l in AHB = [](((!stateG3_0)&&(!stateG3_1)&&(stateG3_2)&&((!start)&&(hready))) -> \n";
$sys_transitions .= " X((!stateG3_0)&&(!stateG3_1)&&(!stateG3_2))).\n";
$sys_transitions .= "\n";
$sys_transitions .= "ltl sys rho G3_m in AHB = [](((!stateG3_0)&&(!stateG3_1)&&(stateG3_2)&&((start))) -> (start && !start)). \n";


#Guarantee 4 and 5:
$sys_transitions .= "\n//Guarantee 4 and 5:\n";
for(my $i = 0; $i < $num_masters; $i++) {
  my $hmaster = buildHMasterString($master_bits, $i);
  $sys_transitions .= "//  Master ".$i.":\n";
  $sys_transitions .= "ltl sys rho G4_$i in AHB = []((hready) -> ((hgrant" . $i . ") <-> X(" . $hmaster . "))).\n";
}
$sys_transitions .= "//  HMASTLOCK:\n";
$sys_transitions .= "ltl sys rho G5 in AHB = []((hready) -> (!hlocked <-> X(!hmastlock))).\n";

#Guarantee 6.1:
$sys_transitions .= "\n//Guarantee 6.1:\n";
for(my $i = 0; $i < $num_masters; $i++) {
  my $hmaster = buildHMasterString($master_bits, $i);
  $sys_transitions .= "//  Master ".$i.":\n";
  $sys_transitions .= "ltl sys rho G6_$i in AHB = [](X(!start) -> ((" . $hmaster . ") <-> (X(" . $hmaster . ")))).\n";
}

#Guarantee 6.2:
$sys_transitions .= "\n//Guarantee 6.2:\n";
$sys_transitions .= "ltl sys rho G6_b in AHB = [](((X(!start))) -> ((hmastlock) <-> X(hmastlock))).\n";

#Guarantee 7:
$sys_transitions .= "\n//Guarantee 7:\n";
my $norequest = "";
for(my $i = 0; $i < $num_masters; $i++) {
    $sys_transitions .= "ltl sys rho G7_${i}_a in AHB = []((decide && hlock$i && X(hgrant$i))->X(hlocked)).\n";
    $sys_transitions .= "ltl sys rho G7_${i}_b in AHB = []((decide && !hlock$i && X(hgrant$i))->X(!hlocked)).\n";
    
    $norequest .= "!hbusreq$i";
    $norequest .= " && " if ($i < $num_masters-1);
}
$sys_transitions .= "//default master\n";
$sys_transitions .= "ltl sys rho G7 in AHB = []((decide && ".$norequest.") -> X(hgrant0)).\n";

#Guarantee 8:
#MW: Note, that this formula changes with respect to the number of grant signals
$sys_transitions .= "\n//Guarantee 8:\n";
my $tmp_g8 = "";
for(my $i = 0; $i < $num_masters; $i++) {
  $sys_transitions .= "ltl sys rho G8_$i in AHB = []((!decide)->(((!hgrant" . $i . ")<->X(!hgrant" . $i . ")))).\n";
}
$sys_transitions .= "ltl sys rho G8 in AHB = []((!decide)->(!hlocked <-> X(!hlocked))).\n";

#Guarantee 10:
$sys_transitions .= "\n//Guarantee 10:\n";
for(my $i = 1; $i < $num_masters; $i++) {
  my $neghmaster = buildNegHMasterString($master_bits, $i);
  my $hmaster = buildHMasterString($master_bits, $i);

  $sys_transitions .= "//  Master ".$i.":\n";
  $sys_transitions .= "ltl sys rho G10_${i}_a in AHB = [](((!stateG10_".$i.")&&(((hgrant".$i.")||(hbusreq".$i."))))->X(!stateG10_".$i.")).\n";
  $sys_transitions .= "ltl sys rho G10_${i}_b in AHB = [](((!stateG10_".$i.")&&((!hgrant".$i.")&&(!hbusreq".$i.")))->X(stateG10_".$i.")).\n";
  $sys_transitions .= "ltl sys rho G10_${i}_c in AHB = [](((stateG10_".$i.")&&((!hgrant".$i.")&&(!hbusreq".$i.")))->X(stateG10_".$i.")).\n";
  $sys_transitions .= "ltl sys rho G10_${i}_d in AHB = [](((stateG10_".$i.")&&(((hgrant".$i."))&&(!hbusreq".$i.")))->(start && !start)).\n";
  $sys_transitions .= "ltl sys rho G10_${i}_e in AHB = [](((stateG10_".$i.")&&(hbusreq".$i."))->X(!stateG10_".$i.")).\n";
}


###############################################
# SYS_FAIRNESS
###############################################
#Guarantee 2:
$sys_fairness .= "\n//Guarantee 2:\n";
for(my $i = 0; $i < $num_masters; $i++) {
    $sys_fairness .= "ltl fluent G2_$i = ((!stateG2_$i)).\n";
    $sys_fairness_list .= "G2_$i, ";
}

#Guarantee 3:
$sys_fairness .= "\n//Guarantee 3:\n";
$sys_fairness .= "ltl fluent G3 = (((!stateG3_0) && (!stateG3_1) && (!stateG3_2))).\n";
$sys_fairness_list .= "G3, ";

#Guarantee 9:
$sys_fairness .= "\n//Guarantee 9:\n";
for(my $i = 0; $i < $num_masters; $i++) {
  $sys_fairness .= "ltl fluent G9_i = (((" . buildHMasterString($master_bits, $i) . ") || !hbusreq" . $i . ")).\n";
  if($i < ($num_masters - 1)){
  $sys_fairness_list .= "G9_$i, ";
  }else{
  $sys_fairness_list .= "G9_$i";
  }
}

###############################################
# PRINT CONFIG FILE
###############################################
print "set Global_Alphabet = {}\n";
print "set Controllable_Alphabet = {}\n";

print "set Global_Signals	= {";
print $input_vars;
print ",";
print $output_vars;
print "}\n";
print "set Output_Signals	= {";
print $output_vars;
print "}\n";
print "order	= {}\n";

print "ltl env theta Init_Buffer_e in AHB	= (";
print $env_initial;
print ").\n";
print "ltl sys theta Init_Buffer_s in AHB	= (";
print $sys_initial;
print ").\n";

print "//##############################################\n";
print "// Environment specification\n";
print "//##############################################\n";


print "//##############################################\n";
print "// Input variable definition\n";
print "//##############################################\n";
print "//[ENV_TRANSITIONS]\n";
print $env_transitions;
print "\n";

print "//##############################################\n";
print "// System specification\n";
print "//##############################################\n";

print "//##############################################\n";
print "// Output variable definition\n";
print "//##############################################\n";
print "//[SYS_TRANSITIONS]\n";
print $sys_transitions;
print "\n";


print "//##############################################\n";
print "// PROPERTY\n";
print "//##############################################\n";

print "//[ENV_FAIRNESS]\n";
print $env_fairness;


print "//[SYS_FAIRNESS]\n";
print $sys_fairness;
print "/*###############################################\n";
print "# Synthesis\n";
print "###############################################*/\n";
print "|f| ENV =(AHB).\n";
print "|gr1| <{$env_fairness_list}><{$sys_fairness_list}> STRAT = ENV.\n";
