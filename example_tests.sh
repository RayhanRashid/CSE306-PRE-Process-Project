./csv -f 05020004-eng.csv
./csv -r 05020004-eng.csv
./csv -min 0 05020004-eng.csv
./csv -h -min "Ref_Date" 05020004-eng.csv
./csv -max 0 05020004-eng.csv
./csv -h -max "Ref_Date" 05020004-eng.csv
./csv -mean 7 05020004-eng.csv
./csv -h -mean "Value" 05020004-eng.csv
./csv -min 0 -max 0 -mean 7 05020004-eng.csv
./csv -h -min "Ref_Date" -max "Ref_Date" -mean "Value" 05020004-eng.csv
./csv -records 7 61.1 05020004-eng.csv
./csv -h -records "Value" 61.1 05020004-eng.csv
