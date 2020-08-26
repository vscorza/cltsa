#! /usr/bin/perl -w 

###############################################
# perl script to generated LTL specification and
# config file to synthesize a generalized buffer
# between k senders and 2 receivers.
#
# Usage: ./genbuf_generator.pl <num_of_senders> <prefix>
#
# Generated files: prefix.ltl prefix.cfg
# Default prefix is genbuf
#
###############################################
use strict;
use POSIX; # qw(ceil floor);

sub slc {
    my $j = shift;
    my $bits = shift;
    my $assign = "";
    my $val;

    for (my $bit = 0; $bit < $bits; $bit++) {
	$val = $j % 2;
	if($val == 0){
		$assign .= "!slc$bit";
	}else{
		$assign .= "slc$bit";	
	}
	$assign .= " && " unless ($bit == $bits-1);
	$j = floor($j/2);
    }
    return $assign;
}

###############################################
# MAIN

if(! defined($ARGV[0])) {
    print "Usage: ./genbuf_generator.pl <num_of_senders> <prefix>\n";
    exit;
}

my $prefix = "genbuf";
if( defined($ARGV[1])) {
    $prefix = $ARGV[1];
}

#variables for LTL specification
my $num_senders = $ARGV[0];
my $slc_bits = ceil((log $num_senders)/(log 2));
$slc_bits = 1 if ($slc_bits == 0);
my $num_receivers = 2;
my $guarantees = "";
my @assert;
my $assumptions = "";
my @assume;

#variables for config file
my $input_vars  = "";
my $output_vars = "";
my $env_initial = "";
my $sys_initial = "";
my $env_transitions = "";
my $sys_transitions = "";
my $env_fairness = "";
my $sys_fairness = "";
my $env_fairness_list = "";
my $sys_fairness_list = "";

###############################################
# Communication with senders
#
my ($g, $a);
for (my $i=0; $i < $num_senders; $i++) {
    #variable definition
    $input_vars .= "sToB_REQ$i";
    $output_vars.= "bToS_ACK$i";

    #initial state
    $env_initial .= "!sToB_REQ$i";
    $sys_initial .= "!bToS_ACK$i";
    
    $input_vars .= ",";
    $output_vars.= ",";

    #initial state
    $env_initial .= " && ";
    $sys_initial .= " && ";    
    
    $guarantees .= "\n//#########################################\n";
    $guarantees .= "//Guarantees for sender $i\n";
    
    ##########################################
    # Guarantee 1
    $g = "ltl sys rho G1_$i in GENBUF\t = [](sToB_REQ$i -> F(bToS_ACK$i)).\t//G1\n";
    $guarantees .= $g; push (@assert, $g);
    # Guarantee 2
    $g = "ltl sys rho G2_$i in GENBUF\t = []((!sToB_REQ$i && X(sToB_REQ$i)) -> X(!bToS_ACK$i)).\t//G2\n";
    $guarantees .= $g; push (@assert, $g);
    $sys_transitions .= $g;
    $sys_fairness .= "ltl fluent G1_2_$i = ((sToB_REQ$i <-> bToS_ACK$i)).//G1+G2\n";    
    $sys_fairness_list .= "G1_2_$i,";
    
    # Guarantee 3
#    $g = "G(sToB_REQ$i=0 -> (bToS_ACK$i=1 + X(bToS_ACK$i=0)));\t#G3\n";
    $g = "ltl sys rho G3_$i in GENBUF\t = []((!bToS_ACK$i && !sToB_REQ$i) -> X(!bToS_ACK$i)).\t//G3\n";
    $guarantees .= $g; push (@assert, $g);
    $sys_transitions .= $g;
    
    # Guarantee 4
    $g = "ltl sys rho G4_$i in GENBUF\t = []((bToS_ACK$i && sToB_REQ$i) -> X(bToS_ACK$i)).\t//G4\n";
    $guarantees .= $g; push (@assert, $g);
    $sys_transitions .= $g;
    
    # Assumption 1
    $a = "ltl env rho A1_$i in GENBUF\t = []((sToB_REQ$i && !bToS_ACK$i) -> X(sToB_REQ$i)).\t//A1\n";
    $assumptions .= $a; push (@assume, $a);
    $env_transitions .= $a;
    $a = "ltl env rho A1_b_$i in GENBUF\t = [](bToS_ACK$i -> X(!sToB_REQ$i)).\t//A1\n";
    $assumptions .= $a; push (@assume, $a);
    $env_transitions .= $a;
    
    # Guarantee 5
    for (my $j=$i+1; $j < $num_senders; $j++) {
	$g = "ltl sys rho G_5_${i}_${j} in GENBUF\t = []((!bToS_ACK$i) || (!bToS_ACK$j)).\t//G5\n";
	$guarantees .= $g; push (@assert, $g);
	$sys_transitions .= $g;
    }
}

###############################################
# Communication with receivers
#
if ($num_receivers != 2) {
    print "Note that the DBW for Guarantee 7 works only for two receivers.\n";
    exit;
}
for (my $j=0; $j < $num_receivers; $j++) {
    #variable definition
    $input_vars .= "rToB_ACK$j";
    $output_vars.= "bToR_REQ$j";

    #initial state
    $env_initial .= "!rToB_ACK$j";
    $sys_initial .= "!bToR_REQ$j";
    
    $input_vars .= ",";
    $output_vars.= ",";

    #initial state
    $env_initial .= " && ";
    $sys_initial .= " && ";    
    
    $guarantees .= "\n##########################################\n";
    $guarantees .= "# Guarantees for receiver $j\n";
    
    # Assumption 2
    $a = "//ltl env rho A2_$j in GENBUF\t = [](bToR_REQ$j -> <>(rToB_ACK$j)).\t//A2\n";
    $assumptions .= $a; push (@assume, $a);
    $env_fairness .= "ltl fluent A2_$j = (bToR_REQ$j <-> rToB_ACK$j).\t//A2\n";
    if($j < ($num_receivers - 1)){
        $env_fairness_list .= "A2_$j,";
    }else{
        $env_fairness_list .= "A2_$j";
    }
    
    # Assumption 3
    $a = "ltl env rho A3_$j in GENBUF\t = [](!bToR_REQ$j -> X(!rToB_ACK$j)).\t//A3\n";
    $assumptions .= $a; push (@assume, $a);
    $env_transitions .= $a;
    
    # Assumption 4
    $a = "ltl env rho A4_$j in GENBUF\t = []((bToR_REQ$j && rToB_ACK$j) -> X(rToB_ACK$j)).\t//A4\n";
    $assumptions .= $a; push (@assume, $a);
    $env_transitions .= $a;

    # Guarantee 6
    $g = "ltl sys rho G6_$j in GENBUF\t = []((bToR_REQ$j && !rToB_ACK$j) -> X(bToR_REQ$j)).\t//G6\n";
    $guarantees .= $g; push (@assert, $g);
    $sys_transitions .= $g;

    # Guarantee 7
    for (my $k=$j+1; $k < $num_receivers; $k++) {
	$g = "ltl env rho G7_${j}_${k} in GENBUF\t = []((!bToR_REQ$j) || (!bToR_REQ$k)).\t//G7\n";
	$guarantees .= $g; push (@assert, $g);
	$sys_transitions .= $g;
    }
    # G7: rose($j) -> X (no_rose W rose($j+1 mod $num_receivers))
    my $n = ($j + 1)%$num_receivers; #next
    my $rose_j  = "(!bToR_REQ$j && X(bToR_REQ$j))";
    my $nrose_j = "(bToR_REQ$j || X(!bToR_REQ$j))";
    my $rose_n  = "(!bToR_REQ$n && X(bToR_REQ$n))";
    $g = "ltl sys rho G7_$j in GENBUF\t = []($rose_j ->\n X(($nrose_j U $rose_n) || []($nrose_j))).\t//G7\n";
    $guarantees .= $g; push (@assert, $g);
    #construct DBW for G7 - see below
    
    # Guarantee 6 and 8
    $g = "ltl sys rho G8_$j in GENBUF\t = [](rToB_ACK$j -> X(!bToR_REQ$j)).\t//G8\n";
    $guarantees .= $g; push (@assert, $g);
    $sys_transitions .= $g;
}

# DBW for guarantee 7
$output_vars .= "stateG7_0,";
$output_vars .= "stateG7_1,";
$sys_initial .= "!stateG7_0 &&";
$sys_initial .= "stateG7_1 &&";
$sys_transitions .= "ltl sys rho G_7_1 in GENBUF\t = []((bToR_REQ0 && bToR_REQ1) -> (bToR_REQ1 && !bToR_REQ1)).\t//G7\n";
$sys_transitions .= "ltl sys rho G_7_2 in GENBUF\t = []((!stateG7_1 && !bToR_REQ0 && bToR_REQ1) ->X(stateG7_1 && !stateG7_0)).//G7\n";
$sys_transitions .= "ltl sys rho G_7_3 in GENBUF\t = []((stateG7_1 && bToR_REQ0 && !bToR_REQ1) -> X(!stateG7_1 && !stateG7_0)).//#G7\n";
$sys_transitions .= "ltl sys rho G_7_4 in GENBUF\t = []((!stateG7_1 && !bToR_REQ0 && !bToR_REQ1) -> X(!stateG7_1 && stateG7_0)).//G7\n";
$sys_transitions .= "ltl sys rho G_7_5 in GENBUF\t = []((stateG7_1 && !bToR_REQ0 && !bToR_REQ1) -> X(stateG7_1 && stateG7_0)).//G7\n";
$sys_transitions .= "ltl sys rho G_7_6 in GENBUF\t = []((!stateG7_1 && !stateG7_0 && bToR_REQ0 && !bToR_REQ1) -> X(!stateG7_1 && !stateG7_0)).//G7\n";
$sys_transitions .= "ltl sys rho G_7_7 in GENBUF\t = []((stateG7_1 && !stateG7_0 && !bToR_REQ0 && bToR_REQ1) -> X(stateG7_1 && !stateG7_0)).//G7\n";
$sys_transitions .= "ltl sys rho G_7_8 in GENBUF\t = []((!stateG7_1 && stateG7_0 && bToR_REQ0) -> (bToR_REQ0 && !bToR_REQ0)).//G7\n";
$sys_transitions .= "ltl sys rho G_7_9 in GENBUF\t = []((stateG7_1 && stateG7_0 && bToR_REQ1) -> (bToR_REQ1 && !bToR_REQ1)).//G7\n";

###############################################
# Communication with FIFO and multiplexer
#
#variable definition
$input_vars .= "full,";
$input_vars .= "empty";
$output_vars .= "enq,";
$output_vars .= "deq,";
$output_vars .= "stateG12,";

#initial state
$env_initial .= "!full && ";
$env_initial .= "empty";
$sys_initial .= "!enq && ";
$sys_initial .= "!deq && ";
$sys_initial .= "!stateG12 && ";

for (my $bit=0; $bit < $slc_bits; $bit++) {
    $output_vars .= "slc$bit";
    $sys_initial .= "!slc$bit";
    if($bit < ($slc_bits - 1)){
    	$output_vars .= ",";
    	$sys_initial .= " && ";
    }
}

$guarantees .= "\n//##########################################\n";
$guarantees .= "// Guarantees for FIFO and multiplexer\n";

# Guarantee 9: enq and slc
$guarantees .= "\n//##########################################\n";
$guarantees .= "// enq <-> Exists i: rose(bToS_ACKi)\n";
my $rosebToS  = "";
my $rosebToSi = "";
for (my $i=0; $i < $num_senders; $i++) {
    $rosebToSi = "(!bToS_ACK$i && X(bToS_ACK$i))";
    $g = "ltl sys rho G_9_$i in GENBUF\t = []($rosebToSi -> X(enq)).//G9\n";
    $guarantees .= $g; push (@assert, $g);
    $sys_transitions .= $g;
    
    $rosebToS   .=   "(bToS_ACK$i || X(!bToS_ACK$i))";
    $rosebToS   .= " &&   " if ($i < ($num_senders - 1));
    if ($i == 0) {
	$g = "ltl sys rho G_9_${i}_slc in GENBUF\t = []($rosebToSi  -> X(".slc($i, $slc_bits).")).//G9\n";
    } else {
	$g = "ltl sys rho G_9_${i}_slc in GENBUF\t = []($rosebToSi <-> X(".slc($i, $slc_bits).")).//G9\n";
    }
    $guarantees .= $g; push (@assert, $g);
    $sys_transitions .= $g;
}
$g = "ltl sys rho G_9_b in GENBUF\t = [](($rosebToS) -> X(!enq)).//G9\n";
$guarantees .= $g; push (@assert, $g);
$sys_transitions .= $g;

# Guarantee 10
$guarantees .= "\n//##########################################\n";
$guarantees .= "//deq <-> Exists j: fell(rToB_ACKj)\n";
my $fellrToB = "";
for (my $j=0; $j < $num_receivers; $j++) {
    $g = "ltl sys rho G_10_$j in GENBUF\t = []((rToB_ACK$j && X(!rToB_ACK$j)) -> X(deq)).//G10\n";
    $guarantees .= $g; push (@assert, $g);
    $sys_transitions .= $g;
    $fellrToB   .=   "(!rToB_ACK$j || X(rToB_ACK$j))";
    $fellrToB   .= " &&   " if ($j < ($num_receivers - 1));
}
$g = "ltl sys rho G_10_b in GENBUF\t = [](($fellrToB) -> X(!deq)).//G10\n";
$guarantees .= $g; push (@assert, $g);
$sys_transitions .= $g;

# Guarantee 11
$guarantees .= "\n";
$g = "ltl sys rho G_11 in GENBUF\t = []((full && !deq) -> !enq).//G11\n";
$guarantees .= $g; push (@assert, $g);
$sys_transitions .= $g;

$g = "ltl sys rho G_11_b in GENBUF\t = [](empty -> !deq).//G11\n";
$guarantees .= $g; push (@assert, $g);
$sys_transitions .= $g;

# Guarantee 12
$g = "//ltl sys rho G_12 in GENBUF\t = [](!empty -> <>(deq)).//G12\n";
$guarantees .= $g; push (@assert, $g);
$sys_transitions .= "ltl sys rho G_12_b_1 in GENBUF\t = []((!stateG12 && empty) -> X(!stateG12)).//G12\n";
$sys_transitions .= "ltl sys rho G_12_b_2 in GENBUF\t = []((!stateG12 && deq  ) -> X(!stateG12)).//G12\n";
$sys_transitions .= "ltl sys rho G_12_b_3 in GENBUF\t = []((!stateG12 && !empty && !deq) -> X(stateG12)).//G12\n";
$sys_transitions .= "ltl sys rho G_12_b_4 in GENBUF\t = []((stateG12 && !deq  ) -> X(stateG12)).//G12\n";
$sys_transitions .= "ltl sys rho G_12_b_5 in GENBUF\t = []((stateG12 && deq  ) -> X(!stateG12)).//G12\n";
$sys_fairness .= "ltl fluent G_12 = (!stateG12).//G12\n";
$sys_fairness_list .= "G_12,";
$sys_fairness .= "//This is an additional fiarness guarantee to make the spec unrealizable\n";
$sys_fairness .= "ltl fluent G_12_b = (stateG12).//G12\n";
$sys_fairness_list .= "G_12_b";

# Assumption 4
$a = "ltl env rho A_4 in GENBUF\t = []((enq && !deq) -> X(!empty)).//A4\n";
$assumptions .= $a; push (@assume, $a);
$env_transitions .= $a;

$a = "ltl env rho A_4 in GENBUF\t = []((deq && !enq) -> X(!full)).//A4\n";
$assumptions .= $a; push (@assume, $a);
$env_transitions .= $a;

$a  = "ltl env rho A_4 in GENBUF\t = []((enq <-> deq) -> ((full <-> X(full)) && (empty <-> X(empty)))).//A4\n";
$assumptions .= $a; push (@assume, $a);
$env_transitions .= $a;

  
###############################################
# PRINT CONFIG FILE
###############################################
print "Generating $prefix.cfg\n";
open (CFG, ">$prefix.cfg");

print CFG "set Global_Alphabet = {}\n";
print CFG "set Controllable_Alphabet = {}\n";

print CFG "set Global_Signals	= {";
print CFG $input_vars;
print CFG ",";
print CFG $output_vars;
print CFG "}\n";
print CFG "set Output_Signals	= {";
print CFG $output_vars;
print CFG "}\n";
print CFG "order	= {}\n";

print CFG "ltl env theta Init_Buffer_e in GENBUF	= (";
print CFG $env_initial;
print CFG ").\n";
print CFG "ltl sys theta Init_Buffer_s in GENBUF	= (";
print CFG $sys_initial;
print CFG ").\n";

print CFG "/*###############################################\n";
print CFG "# Environment specification\n";
print CFG "###############################################*/\n";
print CFG "\n";
print CFG "//[ENV_TRANSITIONS]\n";
print CFG $env_transitions;
print CFG "\n";
print CFG "//[ENV_FAIRNESS]\n";
print CFG $env_fairness;
print CFG "\n";

print CFG "/*###############################################\n";
print CFG "# System specification\n";
print CFG "###############################################*/\n";
print CFG "\n";
print CFG "//[SYS_TRANSITIONS]\n";
print CFG $sys_transitions;
print CFG "\n";
print CFG "//[SYS_FAIRNESS]\n";
print CFG $sys_fairness;
print CFG "\n";
print CFG "/*###############################################\n";
print CFG "# Synthesis\n";
print CFG "###############################################*/\n";
print CFG "|f| ENV =(GENBUF).\n";
print CFG "|gr1| <{$env_fairness_list}><{$sys_fairness_list}> STRAT = ENV.\n";
close CFG;




