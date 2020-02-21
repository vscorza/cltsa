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

	if ($bin_val eq "1") {
            $result .=  $state_name . "[" . $j . "]";
        }
        else {
            $result .= "!" . $state_name . "[" . $j . "]";
        }
    }
    
    return $result;
}

###############################################
sub buildHMasterString {
    my ($master_bits, $value) = @_;
    return buildStateString("hmaster", "&amp;", $master_bits, $value);
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
	    $result .= " | ";
	}
	
	my $bin_val = "1";
	if($j < length($bin)) {
	    $bin_val = substr($bin, $j, 1);
	}
	if ($bin_val eq "1") {
            $result .= "hmaster[" . $j . "])";
        }
        else {
            $result .= "!hmaster[" . $j . "])";
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
my $master_bits_plus_one = ceil((log($num_masters+1))/(log 2));
if($master_bits == 0) {
    $master_bits = 1;
}

my $num_masters_minus_one = $num_masters - 1;
my $master_bits_minus_one = $master_bits - 1;

my $zero_state_string = buildStateString("stateA1", "&amp;", $master_bits_plus_one, 0);

my $env_initial = "";
my $sys_initial = "";
my $env_transitions = "";
my $sys_transitions = "";
my $env_fairness = "";
my $sys_fairness = "";
my $input_vars = "";
my $output_vars = "";
my $init_env = 0;
my $init_sys = 0;
my $trans_env = 0;
my $trans_sys = 0;
my $fair_env = 0;
my $fair_sys = 0;
my $name = "";

###############################################
# ENV_INITIAL and INPUT_VARIABLES
###############################################

$name = "InitEnv_" . $init_env; $init_env++;
$input_vars  .= "<signal><name>hready</name><kind>E</kind><type>boolean</type><notes></notes></signal>\n";
$env_initial .= "<requirement><name>$name</name><property>!hready</property><kind>A</kind><notes></notes><toggled>0</toggled></requirement>\n";

$input_vars .= "<signal><name>hbusreq</name><kind>E</kind><type>array 0.." . $num_masters_minus_one . " of boolean</type><notes></notes></signal>\n";
$name = "InitEnv_" . $init_env; $init_env++;
$env_initial .= "<requirement><name>$name</name><property>forall i in {0:" . $num_masters_minus_one .  "}: !hbusreq[i]</property><kind>A</kind><notes></notes><toggled>0</toggled></requirement>\n";
$input_vars .= "<signal><name>hlock</name><kind>E</kind><type>array 0.." . $num_masters_minus_one . " of boolean</type><notes></notes></signal>\n";
$name = "InitEnv_" . $init_env; $init_env++;
$env_initial .= "<requirement><name>$name</name><property>forall i in {0:" . $num_masters_minus_one .  "}: !hlock[i]</property><kind>A</kind><notes></notes><toggled>0</toggled></requirement>\n";

$input_vars .= "<signal><name>hburst</name><kind>E</kind><type>array 0..1 of boolean</type><notes></notes></signal>\n";
$name = "InitEnv_" . $init_env; $init_env++;
$env_initial .= "<requirement><name>$name</name><property>forall i in {0,1}: !hburst[i]</property><kind>A</kind><notes></notes><toggled>0</toggled></requirement>\n";


###############################################
# ENV_TRANSITION
###############################################
$name = "TransEnv_" . $trans_env; $trans_env++;
$env_transitions .= "<requirement><name>$name</name><property>forall i in {0:" . $num_masters_minus_one . "}: always ( hlock[i] -&gt; hbusreq[i] )</property><kind>A</kind><notes> </notes><toggled>0</toggled></requirement>\n";


###############################################
# ENV_FAIRNESS
###############################################
$name = "FairEnv_" . $fair_env; $fair_env++;
$env_fairness .= "<requirement><name>$name</name><property>always eventually! (".$zero_state_string.")</property><kind>A</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "FairEnv_" . $fair_env; $fair_env++;
$env_fairness .= "<requirement><name>$name</name><property>always eventually! (hready)</property><kind>A</kind><notes></notes><toggled>0</toggled></requirement>\n";

###############################################
# SYS_INITIAL + OUTPUT_VARIABLES
###############################################

$output_vars .= "<signal><name>hmaster</name><kind>S</kind><type>array 0.." . $master_bits_minus_one . " of boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>forall i in {0:" . $master_bits_minus_one . "}: !hmaster[i]</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";


$output_vars .= "<signal><name>hmastlock</name><kind>S</kind><type>boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>!hmastlock</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$output_vars .= "<signal><name>start</name><kind>S</kind><type>boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>start</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$output_vars .= "<signal><name>decide</name><kind>S</kind><type>boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>decide</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$output_vars .= "<signal><name>hlocked</name><kind>S</kind><type>boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>!hlocked</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

$output_vars .= "<signal><name>hgrant</name><kind>S</kind><type>array 0.." . $num_masters_minus_one . " of boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>hgrant[0] &amp; forall i in {1:" . $num_masters_minus_one . "}: !hgrant[i]</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";


#Assumption 1:
$output_vars .= "<signal><name>stateA1</name><kind>S</kind><type>array 0.." . $master_bits . " of boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>forall i in {0:" . $master_bits . "}: !stateA1[i]</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 2:
$output_vars .= "<signal><name>stateG2</name><kind>S</kind><type>array 0.." . $num_masters_minus_one . " of boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>forall i in {0:" . $num_masters_minus_one . "}: !stateG2[i]</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 3:
$output_vars .= "<signal><name>stateG3</name><kind>S</kind><type>array 0..2 of boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>forall i in {0:2}: !stateG3[i]</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 10:
$output_vars .= "<signal><name>stateG10</name><kind>S</kind><type>array 1.." . $num_masters_minus_one . " of boolean</type><notes></notes></signal>\n";
$name = "InitSys_" . $init_sys; $init_sys++;
$sys_initial .= "<requirement><name>$name</name><property>forall i in {1:" . $num_masters_minus_one . "}: !stateG10[i]</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";


###############################################
# SYS_TRANSITION
###############################################

#Assumption 1:
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((" . $zero_state_string;
$sys_transitions .= "&amp; ((!hmastlock) | (hburst[0] | hburst[1]))) -&gt;";
$sys_transitions .= " next(" . $zero_state_string . "))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

for(my $i = 0; $i < $num_masters; $i++) {
    my $i_state_string = buildStateString("stateA1", "&amp;",
					  $master_bits_plus_one, $i+1);
    my $hmaster = buildHMasterString($master_bits, $i);

    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always ((" . $zero_state_string." &amp; ((hmastlock) &amp; (";
    $sys_transitions .= $hmaster.") &amp; (!hburst[0] &amp; !hburst[1]))) -&gt;";
    $sys_transitions .= " next(" . $i_state_string . "))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always ((".$i_state_string."&amp;  hbusreq[".$i."]) -&gt;";
    $sys_transitions .= " next(" . $i_state_string . "))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always ((".$i_state_string."&amp;  !hbusreq[".$i."]) -&gt;";
    $sys_transitions .= " next(".$zero_state_string."))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
}

#Guarantee 1:
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!hready) -&gt; next(!start))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 2:
$zero_state_string = buildStateString("stateG2_", "&amp;", $master_bits_plus_one, 0);
for(my $i = 0; $i < $num_masters; $i++) {
    my $master_state_str = buildHMasterString($master_bits, $i);

    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( !stateG2[".$i."] &amp; (!hmastlock)) -&gt; next !stateG2[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( !stateG2[".$i."] &amp; (!start)) -&gt; next !stateG2[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( !stateG2[".$i."] &amp; hburst[0]) -&gt; next !stateG2[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( !stateG2[".$i."] &amp; hburst[1]) -&gt; next !stateG2[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( !stateG2[".$i."] &amp; !(".$master_state_str.")) -&gt; next !stateG2[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( !stateG2[".$i."] &amp; (hmastlock) &amp; (start) &amp; !hburst[0] &amp; !hburst[1] &amp; (".$master_state_str.")) -&gt; next stateG2[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( stateG2[".$i."] &amp; (!start) &amp;  hbusreq[".$i."]) -&gt; next stateG2[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( stateG2[".$i."] &amp; start) -&gt; next FALSE)</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always (( stateG2[".$i."] &amp; (!start) &amp;  !hbusreq[".$i."]) -&gt; next !stateG2[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
}

#Guarantee 3:
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2] &amp;";
$sys_transitions .= "  ((!hmastlock) | (!start) | (hburst[0] | !hburst[1]))) -&gt;";
$sys_transitions .= " next(!stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2] &amp;";
$sys_transitions .= "  ((hmastlock) &amp; (start) &amp; (!hburst[0] &amp; hburst[1]) &amp; (!hready))) -&gt;";
$sys_transitions .= " next(stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2] &amp;";
$sys_transitions .= "  ((hmastlock) &amp; (start) &amp; (!hburst[0] &amp; hburst[1]) &amp; (hready))) -&gt;";
$sys_transitions .= " next(!stateG3[0] &amp; stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$sys_transitions .= "";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2] &amp; ((!start) &amp; (!hready))) -&gt;";
$sys_transitions .= " next(stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2] &amp; ((!start) &amp; (hready))) -&gt;";
$sys_transitions .= " next(!stateG3[0] &amp; stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$sys_transitions .= "\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2] &amp; ((start))) -&gt; next FALSE)</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$sys_transitions .= "";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; stateG3[1] &amp; !stateG3[2] &amp; ((!start) &amp; (!hready))) -&gt;";
$sys_transitions .= " next(!stateG3[0] &amp; stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; stateG3[1] &amp; !stateG3[2] &amp; ((!start) &amp; (hready))) -&gt;";
$sys_transitions .= " next(stateG3[0] &amp; stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; stateG3[1] &amp; !stateG3[2] &amp; ((start))) -&gt; next FALSE)</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$sys_transitions .= "";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((stateG3[0] &amp; stateG3[1] &amp; !stateG3[2] &amp; ((!start) &amp; (!hready))) -&gt;";
$sys_transitions .= " next(stateG3[0] &amp; stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((stateG3[0] &amp; stateG3[1] &amp; !stateG3[2] &amp; ((!start) &amp; (hready))) -&gt;";
$sys_transitions .= " next(!stateG3[0] &amp; !stateG3[1] &amp; stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((stateG3[0] &amp; stateG3[1] &amp; !stateG3[2] &amp; ((start))) -&gt; next FALSE)</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$sys_transitions .= "";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; !stateG3[1] &amp; stateG3[2] &amp; ((!start) &amp; (!hready))) -&gt;";
$sys_transitions .= " next(!stateG3[0] &amp; !stateG3[1] &amp; stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; !stateG3[1] &amp; stateG3[2] &amp; ((!start) &amp; (hready))) -&gt;";
$sys_transitions .= " next(!stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2]))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
$sys_transitions .= "";
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!stateG3[0] &amp; !stateG3[1] &amp; stateG3[2] &amp; ((start))) -&gt; next FALSE)</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";


#Guarantee 4 and 5:
for(my $i = 0; $i < $num_masters; $i++) {
  my $hmaster = buildHMasterString($master_bits, $i);
  $name = "TransSys_" . $trans_sys; $trans_sys++;
  $sys_transitions .= "<requirement><name>$name</name><property>always ((hready) -&gt; ((hgrant[" . $i . "]=1) &lt;-&gt; next(" . $hmaster . ")))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
}
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((hready) -&gt; (!hlocked &lt;-&gt; next(!hmastlock)))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 6.1:
for(my $i = 0; $i < $num_masters; $i++) {
  my $hmaster = buildHMasterString($master_bits, $i);
  $name = "TransSys_" . $trans_sys; $trans_sys++;
  $sys_transitions .= "<requirement><name>$name</name><property>always (next(!start) -&gt; ((" . $hmaster . ") &lt;-&gt; (next(" . $hmaster . "))))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
}

#Guarantee 6.2:
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always (((next(!start))) -&gt; ((hmastlock) &lt;-&gt; next(hmastlock)))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 7:
my $norequest = "";
for(my $i = 0; $i < $num_masters; $i++) {
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always ((decide=1 &amp; hlock[$i] &amp; next hgrant[$i]) -&gt; next(hlocked=1))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    $name = "TransSys_" . $trans_sys; $trans_sys++;
    $sys_transitions .= "<requirement><name>$name</name><property>always ((decide=1 &amp; !hlock[$i] &amp; next hgrant[$i]) -&gt; next(!hlocked))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
    
    $norequest .= "!hbusreq[$i]";
    $norequest .= " &amp; " if ($i < $num_masters-1);
}
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((decide=1 &amp; ".$norequest.") -&gt; next hgrant[0])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 8:
#MW: Note, that this formula changes with respect to the number of grant signals
my $tmp_g8 = "";
for(my $i = 0; $i < $num_masters; $i++) {
    $name = "TransSys_" . $trans_sys; $trans_sys++;
  $sys_transitions .= "<requirement><name>$name</name><property>always ((!decide)-&gt;(((!hgrant[" . $i . "]) &lt;-&gt; next(!hgrant[" . $i . "]))))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
}
$name = "TransSys_" . $trans_sys; $trans_sys++;
$sys_transitions .= "<requirement><name>$name</name><property>always ((!decide) -&gt; (!hlocked &lt;-&gt; next(!hlocked)))</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 10:
for(my $i = 1; $i < $num_masters; $i++) {
  my $neghmaster = buildNegHMasterString($master_bits, $i);
  my $hmaster = buildHMasterString($master_bits, $i);

  $name = "TransSys_" . $trans_sys; $trans_sys++;
  $sys_transitions .= "<requirement><name>$name</name><property>always (( !stateG10[".$i."] &amp; (( hgrant[".$i."] |  hbusreq[".$i."]))) -&gt; next !stateG10[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
  $name = "TransSys_" . $trans_sys; $trans_sys++;
  $sys_transitions .= "<requirement><name>$name</name><property>always (( !stateG10[".$i."] &amp; ( !hgrant[".$i."] &amp;  !hbusreq[".$i."])) -&gt; next stateG10[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
  $name = "TransSys_" . $trans_sys; $trans_sys++;
  $sys_transitions .= "<requirement><name>$name</name><property>always (( stateG10[".$i."] &amp; ( !hgrant[".$i."] &amp;  !hbusreq[".$i."])) -&gt; next stateG10[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
  $name = "TransSys_" . $trans_sys; $trans_sys++;
  $sys_transitions .= "<requirement><name>$name</name><property>always (( stateG10[".$i."] &amp; (( hgrant[".$i."]) &amp;  !hbusreq[".$i."])) -&gt; next FALSE)</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
  $name = "TransSys_" . $trans_sys; $trans_sys++;
  $sys_transitions .= "<requirement><name>$name</name><property>always (( stateG10[".$i."] &amp;  hbusreq[".$i."]) -&gt; next !stateG10[".$i."])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
}


###############################################
# SYS_FAIRNESS
###############################################
#Guarantee 2:
for(my $i = 0; $i < $num_masters; $i++) {
    $name = "FairSys_" . $fair_sys; $fair_sys++;
    $sys_fairness .= "<requirement><name>$name</name><property>always eventually!  !stateG2[$i]</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
}

#Guarantee 3:
$name = "FairSys_" . $fair_sys; $fair_sys++;
$sys_fairness .= "<requirement><name>$name</name><property>always eventually! (!stateG3[0] &amp; !stateG3[1] &amp; !stateG3[2])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

#Guarantee 9:
for(my $i = 0; $i < $num_masters; $i++) {
    $name = "FairSys_" . $fair_sys; $fair_sys++;
    $sys_fairness .= "<requirement><name>$name</name><property>always eventually! ((" . buildHMasterString($master_bits, $i) . ") | !hbusreq[" . $i . "])</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";
}
#$name = "FairSys_" . $fair_sys; $fair_sys++;
#$sys_fairness .= "<requirement><name>$name</name><property>always eventually! TRUE</property><kind>G</kind><notes></notes><toggled>0</toggled></requirement>\n";

###############################################
# PRINT CONFIG FILE
###############################################

print "<project><signals>\n";
print $input_vars;
print $output_vars;
print "</signals><requirements>\n";
print $env_initial;
print $env_transitions;
print $env_fairness;
print $sys_initial;
print $sys_transitions;
print $sys_fairness;
print "</requirements>\n
  <property_assurance> <possibilities/> <assertions/> </property_assurance> 
  <property_simulation/>
  <categories>
    <category> <name> New </name> <editable> no </editable> <notes> This is the category of those traces that have been just created </notes> </category>
    <category> <name> Default </name> <editable> no </editable> <notes> This is the default category for traces </notes> </category>
    <category> <name> Out of Date </name> <editable> no </editable> <notes> Contains the traces whose dependencies might be no longer consistent </notes> </category>
    <category> <name> Trash </name> <editable> no </editable> <notes> Contains the traces that have been deleted </notes> </category>
  </categories>
  <active_view> re </active_view>
  <notes> an ARM AMBA AHB with $num_masters masters</notes>
</project>
";

