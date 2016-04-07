#!/bin/bash

BINDIR="../bin"
DFILE="data/test.c3db"
IFILE="data/input.txt"
OFILE="data/dump.sec.txt"
TFILE="data/dump.tval.txt"
UFILE="data/dump.usec.txt"
QFILE="data/query.txt"
RETAIN="10:10d;60:40d;1200:500d"
SECONDS=$(date +%s)
CLEAN=0


while getopts "hcr:" o; do
	case $o in
		h)	"Usage: test.sh [-c] [-r <retain>]"
			;;
		r)	RETAIN=$OPTARG
			;;
		c)	CLEAN=1
			;;
	esac
done


rm -f data/*

echo "Generating some test data into $IFILE"
POINTS=2000
START=$(($SECONDS - 20 - ($POINTS * 3)))
TS=$START

i=0
while [ $i -lt $POINTS ]; do
	VAL=$((50 + ($RANDOM % 50)))
	echo "${TS}:${VAL}" >> $IFILE
	TS=$(($TS + ($RANDOM % 7)))
	((i++))
done

echo "Creating a C3DB database $DFILE retaining $RETAIN"
${BINDIR}/c3db_create -V -r $RETAIN -f $DFILE | sed 's/^/    /'
if [ $? -ne 0 ]; then
	echo "Failed to create database.  Test ended badly."
	exit 1
fi

echo "Checking file status of $DFILE"
${BINDIR}/c3db_dump -D -f $DFILE | sed 's/^/    /'
if [ $? -ne 0 ]; then
	echo "Failed to dump database header.  Test ended badly."
	exit 1
fi

echo "Updating from points in $IFILE"
${BINDIR}/c3db_update -V -f $DFILE -i $IFILE | sed 's/^/    /'
if [ $? -ne 0 ]; then
	echo "Failed update $i of database.  Test ended badly."
	exit 1
fi

echo "Dumping database into $OFILE"
${BINDIR}/c3db_dump -E -f $DFILE -o $OFILE
if [ $? -ne 0 ]; then
	echo "Failed to dump database (sec).  Test ended badly."
	exit 1
fi

echo "Dumping database into $TFILE"
${BINDIR}/c3db_dump -E -f $DFILE -t tval -o $TFILE
if [ $? -ne 0 ]; then
	echo "Failed to dump database (tval).  Test ended badly."
	exit 1
fi

echo "Dumping database into $UFILE"
${BINDIR}/c3db_dump -E -f $DFILE -t usec -o $UFILE
if [ $? -ne 0 ]; then
	echo "Failed to dump database (usec).  Test ended badly."
	exit 1
fi

# pick a time range in the middle of our test data
SPAN=$(($TS - $START))
QSTART=$(($START + ($SPAN / 3)))
QEND=$(($TS - ($SPAN / 3)))

echo "Querying selected timestamps into $QFILE"
${BINDIR}/c3db_query -f $DFILE -b $QSTART -e $QEND -m mean -o $QFILE
if [ $? -ne 0 ]; then
	echo "Failed to dump database.  Test ended badly."
	exit 1
fi

if [ $CLEAN -gt 0 ]; then
	echo "Cleaning up test data."
	rm -f data/*
	rm -f ./gmon.out
fi

echo "Tests complete."

