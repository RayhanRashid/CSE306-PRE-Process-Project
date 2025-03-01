echo "Outputs should match the specification in the handout"
echo "------TEST 01------"
./csv -f 05020004-eng.csv
echo "------TEST 02------"
./csv -r 05020004-eng.csv
echo "------TEST 03------"
./csv -min 0 05020004-eng.csv
echo "------TEST 04------"
./csv -h -min "Ref_Date" 05020004-eng.csv
echo "------TEST 05------"
./csv -max 0 05020004-eng.csv
echo "------TEST 06------"
./csv -h -max "Ref_Date" 05020004-eng.csv
echo "------TEST 07------"
./csv -mean 7 05020004-eng.csv
echo "------TEST 08------"
./csv -h -mean "Value" 05020004-eng.csv
echo "------TEST 09------"
./csv -min 0 -max 0 -mean 7 05020004-eng.csv
echo "------TEST 10------"
./csv -h -min "Ref_Date" -max "Ref_Date" -mean "Value" 05020004-eng.csv
echo "------TEST 11------"
./csv -records 7 61.1 05020004-eng.csv
echo "------TEST 12------"
./csv -h -records "Value" 61.1 05020004-eng.csv
echo "------TESTEND------"
