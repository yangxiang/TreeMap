#!/bin/sh
echo "qsub script_file_TreeMap_HighwayCentric_amaze"
qsub script_file_TreeMap_HighwayCentric_amaze
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_citeseer"
qsub script_file_TreeMap_HighwayCentric_citeseer
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_hpycyc"
qsub script_file_TreeMap_HighwayCentric_hpycyc
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_kegg"
qsub script_file_TreeMap_HighwayCentric_kegg
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_p2p08"
qsub script_file_TreeMap_HighwayCentric_p2p08
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_p2p09"
qsub script_file_TreeMap_HighwayCentric_p2p09
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_reactome"
qsub script_file_TreeMap_HighwayCentric_reactome
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_vchocyc"
qsub script_file_TreeMap_HighwayCentric_vchocyc
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_Wiki-Vote"
qsub script_file_TreeMap_HighwayCentric_Wiki-Vote
sleep 10

echo "qsub script_file_TreeMap_HighwayCentric_xmark"
qsub script_file_TreeMap_HighwayCentric_xmark
sleep 10

echo "qsub script_file_TreeMap_TEDI_Dutsch"
qsub script_file_TreeMap_TEDI_Dutsch
sleep 10

echo "qsub script_file_TreeMap_TEDI_Eva"
qsub script_file_TreeMap_TEDI_Eva
sleep 10

echo "qsub script_file_TreeMap_TEDI_Geom"
qsub script_file_TreeMap_TEDI_Geom
sleep 10

echo "qsub script_file_TreeMap_TEDI_Homo"
qsub script_file_TreeMap_TEDI_Homo
sleep 10

echo "qsub script_file_TreeMap_TEDI_Inter"
qsub script_file_TreeMap_TEDI_Inter
sleep 10

echo "qsub script_file_TreeMap_TEDI_Pfei"
qsub script_file_TreeMap_TEDI_Pfei
sleep 10

echo "qsub script_file_TreeMap_TEDI_PPI"
qsub script_file_TreeMap_TEDI_PPI
sleep 10

echo "qsub script_file_TreeMap_TEDI_Yeast"
qsub script_file_TreeMap_TEDI_Yeast
exit 0
