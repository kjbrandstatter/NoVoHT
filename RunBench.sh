#!/bin/bash
logfile=runbench.log
datafile=testresults.csv
function persistent {
echo "Persistent test"
for i in {1,5}00000{,0,00,000}
#for i in {1,5}00000 #{,0,00,000}
do ./fbench $i fdata | tee -a $logfile | ./csvout.pl -out $datafile -type PERS
done
}
function ssd_persistent {
echo "Persistent test (SSD)"
for i in {1,5}00000{,0,00,000}
do ./fbench $i /mnt/SSD/kevin/fdata | tee -a $logfile | ./csvout.pl -out $datafile -type SSD
done
}
function hybrid_persistence {
echo "Hybrid Persistent test"
echo unavailable
for i in {1,5}00000{,0,00,000}
do ./fbench $i /mnt/Hybrid/fdata | tee -a $logfile | ./csvout.pl -out $datafile -type HYBRID
done
}
function no_persistence {
echo "Non persistent test"
for i in {1,5}00000{,0,00,000}
do ./fbench $i | tee -a $logfile | ./csvout.pl -out $datafile -type NOPERS
done
}
function basic {
persistent
no_persistence
}
function extra {
ssd_persistent
hybrid_persistence
}

function all {
basic
extra
}

[[ $# -gt 0 ]] && $1 || echo Invalid Benchmark
